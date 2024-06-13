#include "midi.hpp"

struct MidiEvent;
struct MidiNote;
struct MidiTrack;

MIDI::MIDI(const std::string& file_name){
    midi.open(file_name, std::fstream::in | std::ios::binary);
    if(!midi.is_open())
        std::cerr << "Midi file open failed." << std::endl;

    ParseFile();
    ConvertEventNote();
}


MIDI::~MIDI(){
    this->midi.close();
    std::cout << "MIDI object destroyed." << std::endl;
}

void MIDI::ParseFile(){
    std::cout << "Parsing file." << std::endl;
    uint16_t n16;
    uint32_t n32;

    auto Swap32 = [](uint32_t n)
    {
        return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
    };

    // Swaps byte order of 16-bit integer
    auto Swap16 = [](uint16_t n)
    {
        return ((n >> 8) | (n << 8));
    };

    auto ReadValue = [this](){
        uint32_t nValue = 0;
        uint8_t nByte = 0;
        nValue = midi.get();

        // Check MSB, if set, more bytes need reading
        if (nValue & 0x80){
            // Extract bottom 7 bits of read byte
            nValue &= 0x7F;
            do{
                // Read next byte
                nByte = midi.get();
                // Construct value by setting bottom 7 bits, then shifting 7 bits
                nValue = (nValue << 7) | (nByte & 0x7F);
            } 
            while (nByte & 0x80); // Loop whilst read byte MSB is 1
        }
        // Return final construction (always 32-bit unsigned integer internally)
        return nValue;
    };

    auto ReadString = [this](uint32_t nLength)
    {
        std::string s;
        for (uint32_t i = 0; i < nLength; i++) s += midi.get();
        return s;
    };

    midi.read((char*)&n32, sizeof(uint32_t));
    uint32_t nFileID = Swap32(n32);
    midi.read((char*)&n32, sizeof(uint32_t));
    uint32_t nHeaderLength = Swap32(n32);
    midi.read((char*)&n16, sizeof(uint16_t));
    uint16_t nFormat = Swap16(n16);
    midi.read((char*)&n16, sizeof(uint16_t));
    uint16_t nTrackChunks = Swap16(n16);
    midi.read((char*)&n16, sizeof(uint16_t));
    uint16_t nDivision = Swap16(n16);   

    for (uint16_t nChunk = 0; nChunk < nTrackChunks; nChunk++){			
        std::cout << "===== NEW TRACK" << std::endl;
        // Read Track Header
        midi.read((char*)&n32, sizeof(uint32_t));
        uint32_t nTrackID = Swap32(n32);
        midi.read((char*)&n32, sizeof(uint32_t));
        uint32_t nTrackLength = Swap32(n32);

        bool EndOfTrack = false;

        vecTracks.push_back(MidiTrack());

		uint32_t nWallTime = 0;

		uint8_t nPreviousStatus = 0;

        while (!this->midi.eof() && !EndOfTrack){
            uint32_t nStatusTimeDelta = 0;
            uint8_t nStatus = 0;

            // Read Timecode from MIDI stream. This could be variable in length
            // and is the delta in "ticks" from the previous event. Of course this value
            // could be 0 if two events happen simultaneously.
            nStatusTimeDelta = ReadValue();

            // Read first byte of message, this could be the status byte, or it could not...
            nStatus = this->midi.get();

            if (nStatus < 0x80){
                // MIDI Running Status is happening, so refer to previous valid MIDI Status byte
                nStatus = nPreviousStatus;

                // We had to read the byte to assess if MIDI Running Status is in effect. But!
                // that read removed the byte form the stream, and that will desync all of the 
                // following code because normally we would have read a status byte, but instead
                // we have read the data contained within a MIDI message. The simple solution is 
                // to put the byte back :P
                midi.seekg(-1, std::ios_base::cur);
            }

            if ((nStatus & 0xF0) == EventName::VoiceNoteOff){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nNoteID = midi.get();
                uint8_t nNoteVelocity = midi.get();
                vecTracks[nChunk].vecEvents.push_back({MidiEvent::Type::NoteOff, nNoteID, nNoteVelocity, nStatusTimeDelta});
            }

            else if ((nStatus & 0xF0) == EventName::VoiceNoteOn){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nNoteID = midi.get();
                uint8_t nNoteVelocity = midi.get();
                if(nNoteVelocity == 0)
                    vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nNoteID, nNoteVelocity, nStatusTimeDelta });
                else
                    vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOn, nNoteID, nNoteVelocity, nStatusTimeDelta });
            }

            else if ((nStatus & 0xF0) == EventName::VoiceAftertouch){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nNoteID = midi.get();
                uint8_t nNoteVelocity = midi.get();
                vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
            }

            else if ((nStatus & 0xF0) == EventName::VoiceControlChange){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nControlID = midi.get();
                uint8_t nControlValue = midi.get();
                vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
            }

            else if ((nStatus & 0xF0) == EventName::VoiceProgramChange){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nProgramID = midi.get();					
                vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
            }

            else if ((nStatus & 0xF0) == EventName::VoiceChannelPressure){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nChannelPressure = midi.get();
                vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
            }

            else if ((nStatus & 0xF0) == EventName::VoicePitchBend){
                nPreviousStatus = nStatus;
                uint8_t nChannel = nStatus & 0x0F;
                uint8_t nLS7B = midi.get();
                uint8_t nMS7B = midi.get();
                vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });

            }

            else if ((nStatus & 0xF0) == EventName::SystemExclusive){
                nPreviousStatus = 0;

                if (nStatus == 0xFF){
                    // Meta Message
                    uint8_t nType = midi.get();
                    uint8_t nLength = ReadValue();

                    switch (nType)
                    {
                    case MetaSequence:
                        std::cout << "Sequence Number: " << midi.get() << midi.get() << std::endl;
                        break;
                    case MetaText:
                        std::cout << "Text: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaCopyright:
                        std::cout << "Copyright: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaTrackName:
                        vecTracks[nChunk].sName = ReadString(nLength);
                        std::cout << "Track Name: " << vecTracks[nChunk].sName << std::endl;							
                        break;
                    case MetaInstrumentName:
                        vecTracks[nChunk].sInstrument = ReadString(nLength);
                        std::cout << "Instrument Name: " << vecTracks[nChunk].sInstrument << std::endl;
                        break;
                    case MetaLyrics:
                        std::cout << "Lyrics: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaMarker:
                        std::cout << "Marker: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaCuePoint:
                        std::cout << "Cue: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaChannelPrefix:
                        std::cout << "Prefix: " << midi.get() << std::endl;
                        break;
                    case MetaEndOfTrack:
                        EndOfTrack = true;
                        break;
                    case MetaSetTempo:
                        // Tempo is in microseconds per quarter note	
                        if (m_nTempo == 0)
                        {
                            (m_nTempo |= (midi.get() << 16));
                            (m_nTempo |= (midi.get() << 8));
                            (m_nTempo |= (midi.get() << 0));
                            m_nBPM = (60000000 / m_nTempo);
                            std::cout << "Tempo: " << m_nTempo << " (" << m_nBPM << "bpm)" << std::endl;
                        }
                        break;
                    case MetaSMPTEOffset:
                        std::cout << "SMPTE: H:" << midi.get() << " M:" << midi.get() << " S:" << midi.get() << " FR:" << midi.get() << " FF:" << midi.get() << std::endl;
                        break;
                    case MetaTimeSignature:
                        std::cout << "Time Signature: " << midi.get() << "/" << (2 << midi.get()) << std::endl;
                        std::cout << "ClocksPerTick: " << midi.get() << std::endl;

                        // A MIDI "Beat" is 24 ticks, so specify how many 32nd notes constitute a beat
                        std::cout << "32per24Clocks: " << midi.get() << std::endl;
                        break;
                    case MetaKeySignature:
                        std::cout << "Key Signature: " << midi.get() << std::endl;
                        std::cout << "Minor Key: " << midi.get() << std::endl;
                        break;
                    case MetaSequencerSpecific:
                        std::cout << "Sequencer Specific: " << ReadString(nLength) << std::endl;
                        break;
                    default:
                        std::cout << "Unrecognised MetaEvent: " << nType << std::endl;
                    }
                }

                if (nStatus == 0xF0){
                    // System Exclusive Message Begin
                    std::cout << "System Exclusive Begin: " << ReadString(ReadValue())  << std::endl;
                }

                if (nStatus == 0xF7){
                    // System Exclusive Message Begin
                    std::cout << "System Exclusive End: " << ReadString(ReadValue()) << std::endl;
                }
            }			
            else{
                std::cout << "Unrecognised Status Byte: " << nStatus << std::endl;
            }
        }
    }

}

void MIDI::shift(float percentage, int track, int index){
    std::cout << vecTracks[track].vecEvents[index].nKey << std::endl;
}

const void MIDI::showAllEvent(){
    printf("====================\nShow All Events:\n");
    uint32_t maxtick=0;
    for (auto& track: vecTracks){
        printf("new track\n");
        for (auto& event: track.vecEvents){
            if(event.nDeltaTick > maxtick)
                maxtick = event.nDeltaTick;
            if (event.event==MidiEvent::Type::NoteOn)
                printf("%-10s key%-5d dtick%-10d vel%-5d\n", "NoteOn", event.nKey, event.nDeltaTick, event.nVelocity);
            else if (event.event==MidiEvent::Type::NoteOff)
                printf("%-10s key%-5d dtick%-10d vel%-5d\n", "NoteOff", event.nKey, event.nDeltaTick, event.nVelocity);           
            else if (event.event==MidiEvent::Type::Other)
                printf("%-10s\n", "Other");
        }
    }
    printf("====================\n");
} 

const void MIDI::showAllNote(){
    printf("====================\nShow All Notes:\n");
    for (auto& track: vecTracks){
        //printf("legth of vecNotes %5d\n", track.vecNotes.size());
        printf("new track\n");
        for (auto& note: track.vecNotes){
            printf("key%-5d start%-10d duration%-5d\n", note.nKey, note.nStartTime, note.nDuration);
        }
    }
    printf("====================\n");
} 

void MIDI::ConvertEventNote(){

    //saving key and absolute time 
    std::list<std::pair<MidiEvent, uint32_t>> tmp;
    uint32_t total_tick=0;

    for (auto& track: vecTracks){
        track.vecNotes.clear();
        tmp.clear();
        for (auto& event: track.vecEvents){
            total_tick += event.nDeltaTick;
            if (event.event==MidiEvent::Type::NoteOn)
                tmp.push_back(std::make_pair(event, total_tick));
            else if (event.event==MidiEvent::Type::NoteOff){
                for (auto& note:tmp){
                    //printf("%5d %5d\n", event.nKey, note.first.nKey);
                    if (event.nKey==note.first.nKey){
                        track.vecNotes.push_back({event.nKey, note.first.nVelocity, note.second, total_tick-note.second});
                        tmp.remove(note);
                        //printf("legth of vecNotes %5d\n", track.vecNotes.size());
                        //printf("found\n");
                        break;
                    }
                }
            } 
            sort(track.vecNotes.begin(), track.vecNotes.end(), [](const MidiNote& a, const MidiNote&b){
                return a.nStartTime < b.nStartTime;
            });
        }
    }
    
}

void MIDI::ConvertNoteEvent(){

    std::vector<MidiNote> tmp;
    uint32_t tick=0;

    for (auto& track: vecTracks){
        track.vecEvents.clear();
        tick = 0;
        tmp.clear();
        for (auto& note: track.vecNotes){
            while(!tmp.empty() && ((tmp.back().nStartTime+tmp.back().nDuration) < note.nStartTime)){
                auto &n = tmp.back();
                track.vecEvents.push_back({MidiEvent::Type::NoteOff, n.nKey, 0, n.nStartTime+n.nDuration-tick});
                tick = n.nStartTime+n.nDuration;
                tmp.pop_back();
            }

            track.vecEvents.push_back({MidiEvent::Type::NoteOn, note.nKey, note.nVelocity, note.nStartTime-tick});
            tick = note.nStartTime;

            tmp.push_back(note);
            sort(tmp.begin(), tmp.end(), [](const MidiNote& a, const MidiNote& b) { 
                return int(a.nDuration+a.nStartTime) > int(b.nDuration+b.nStartTime); 
            }); 
        }

        while(!tmp.empty()){
            auto n = tmp.back();
            tmp.pop_back();
            track.vecEvents.push_back({MidiEvent::Type::NoteOff, n.nKey, 0, n.nStartTime+n.nDuration-tick});
            tick = n.nStartTime+n.nDuration;
        }
    }
}

unsigned char hexCharToByte(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    //printf("hex:%d\n", hex);
    throw std::invalid_argument("Invalid hex character");
}

std::vector<unsigned char> hexStringToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    size_t len = hex.length();
    if (len % 2 != 0) {
        throw std::invalid_argument("Hex string length must be even");
    }

    for (size_t i = 0; i < len; i += 2) {
        unsigned char high = hexCharToByte(hex[i]);
        unsigned char low = hexCharToByte(hex[i + 1]);
        bytes.push_back((high << 4) | low);
    }
    return bytes;
}

void MIDI::write(const std::string& file, std::string& hexString1, std::string& hexString2){

    std::string header = "4D 54 68 64 00 00 00 06 00 01 00 02 00 60";
    std::string end = "00 FF 2F 00";
    std::string hexString;
    std::string track = "4D 54 72 6B";

    std::stringstream len1;
    len1 << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>((hexString1.size()/2+4));
    hexString1 = track+len1.str()+hexString1+end;

    std::stringstream len2;
    len2 << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>((hexString2.size()/2+4));
    hexString2 = track+len2.str()+hexString2+end;
 
    hexString = hexString1+hexString2;
    hexString.erase(remove_if(hexString.begin(), hexString.end(), isspace), hexString.end());

    std::vector<unsigned char> binaryData;
    binaryData = hexStringToBytes(hexString);
    
    std::ofstream outFile(file, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(binaryData.data()), binaryData.size());
    outFile.close();
}

std::string MIDI::EventHex(const MidiEvent& event){
    auto int8ToHex = [](const uint8_t value) -> std::string {
        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        return ss.str();
    };
    auto int32ToHex = [](const uint32_t value) -> std::string {
        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << value;
        return ss.str();
    };
    auto timeToHex = [&](uint32_t time) -> std::string {
        std::list<std::string> tmp;

        uint8_t r = (int)(time & (0x7f));
        tmp.push_back(int8ToHex(r));
        time = time >> 7;

        while (time > (uint32_t)127){
            uint8_t remainder = (int)(time & (0x7f));
            tmp.push_back(int8ToHex(remainder+128));
            time = time >> 7;
            //printf("time: %d\n", time);
        }
        
        uint8_t remainder = (int)(time & (0x7f));
        if (remainder != 0)
            tmp.push_back(int8ToHex(remainder+128));
        
        std::stringstream ss;
        while(!tmp.empty()){
            ss << std::hex << std::setw(2) << std::setfill('0') << tmp.back();
            tmp.pop_back();
        }
        return ss.str();
    };

    if(event.event == MidiEvent::Type::NoteOn){
        std::string time = int8ToHex(event.nDeltaTick);
        std::string key = int8ToHex(event.nKey);
        std::string velocity = int8ToHex(event.nVelocity);
        std::string msg = time+"90"+key+velocity;
        return msg;
    }
    else if(event.event == MidiEvent::Type::NoteOff){
        std::string time = timeToHex(event.nDeltaTick);
        std::string key = int8ToHex(event.nKey);
        std::string msg = time+"80"+key+"00";
        return msg;
    }
    return "-1";
}

void MIDI::EventToMeg(){
    std::string s="";
    for (auto &track: vecTracks){
        for (auto &event: track.vecEvents){
            s += EventHex(event);
        }
        msgs.push_back(s);
    }
}

void MIDI::shiftByPercentage(float p, size_t index){
    //printf("track%d\n", vecTracks.size());
    //printf("index%d\n",vecTracks[0].vecEvents[index].nKey);
}


bool operator==(const MidiEvent e1, const MidiEvent e2){
    if (e1.event==e2.event && e1.nKey==e2.nKey && e1.nDeltaTick==e1.nDeltaTick)
        return true;
    else 
        return false;
}

HillClimb::HillClimb(){}

void HillClimb::read(const std::string &file_name){
    file = new MIDI(file_name);
}