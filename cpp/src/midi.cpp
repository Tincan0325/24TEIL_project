#include "midi.hpp"

struct MidiEvent;
struct MidiNote;
struct MidiTrack;

MIDI::MIDI(){};
MIDI::MIDI(const std::string& file_name){
    // std::cout << "MIDI init\n";
    midi.open(file_name, std::fstream::in | std::ios::binary);
    if(!midi.is_open())
        std::cerr << "Midi file open failed." << std::endl;

    ParseFile();
    ConvertEventNote();
}

void MIDI::copy(MIDI* c){
    vecTracks = c->vecTracks;
}


MIDI::~MIDI(){
    this->midi.close();
    std::cout << "MIDI object destroyed." << std::endl;
}

void MIDI::ParseFile(){
    // std::cout << "Parsing file." << std::endl;
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
        // std::cout << "===== NEW TRACK" << std::endl;
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
                        // std::cout << "Sequence Number: " << midi.get() << midi.get() << std::endl;
                        break;
                    case MetaText:
                        // std::cout << "Text: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaCopyright:
                        // std::cout << "Copyright: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaTrackName:
                        vecTracks[nChunk].sName = ReadString(nLength);
                        // std::cout << "Track Name: " << vecTracks[nChunk].sName << std::endl;							
                        break;
                    case MetaInstrumentName:
                        vecTracks[nChunk].sInstrument = ReadString(nLength);
                        // std::cout << "Instrument Name: " << vecTracks[nChunk].sInstrument << std::endl;
                        break;
                    case MetaLyrics:
                        // std::cout << "Lyrics: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaMarker:
                        // std::cout << "Marker: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaCuePoint:
                        // std::cout << "Cue: " << ReadString(nLength) << std::endl;
                        break;
                    case MetaChannelPrefix:
                        // std::cout << "Prefix: " << midi.get() << std::endl;
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
                            // std::cout << "Tempo: " << m_nTempo << " (" << m_nBPM << "bpm)" << std::endl;
                        }
                        break;
                    case MetaSMPTEOffset:
                        // std::cout << "SMPTE: H:" << midi.get() << " M:" << midi.get() << " S:" << midi.get() << " FR:" << midi.get() << " FF:" << midi.get() << std::endl;
                        break;
                    case MetaTimeSignature:
                        // std::cout << "Time Signature: " << midi.get() << "/" << (2 << midi.get()) << std::endl;
                        // std::cout << "ClocksPerTick: " << midi.get() << std::endl;

                        // A MIDI "Beat" is 24 ticks, so specify how many 32nd notes constitute a beat
                        // std::cout << "32per24Clocks: " << midi.get() << std::endl;
                        break;
                    case MetaKeySignature:
                        // std::cout << "Key Signature: " << midi.get() << std::endl;
                        // std::cout << "Minor Key: " << midi.get() << std::endl;
                        break;
                    case MetaSequencerSpecific:
                        // std::cout << "Sequencer Specific: " << ReadString(nLength) << std::endl;
                        break;
                    // default:
                        //std::cout << "Unrecognised MetaEvent: " << nType << std::endl;
                    }
                }

                if (nStatus == 0xF0){
                    // System Exclusive Message Begin
                    // std::cout << "System Exclusive Begin: " << ReadString(ReadValue())  << std::endl;
                }

                if (nStatus == 0xF7){
                    // System Exclusive Message Begin
                    // std::cout << "System Exclusive End: " << ReadString(ReadValue()) << std::endl;
                }
            }			
            else{
                // std::cout << "Unrecognised Status Byte: " << nStatus << std::endl;
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
        total_tick = 0;
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

void MIDI::write(const std::string file, std::string hexString1, std::string hexString2){

    std::string header = "4D 54 68 64 00 00 00 06 00 01 00 02 00 60";
    std::string foot = "00";
    std::string end = "00 FF 2F 00";
    std::string hexString;
    std::string track = "4D 54 72 6B";

    std::stringstream len1;
    hexString1.erase(remove_if(hexString1.begin(), hexString1.end(), isspace), hexString1.end());
    len1 << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>((hexString1.size()/2+4));
    hexString1 = track+len1.str()+hexString1+end;

    std::stringstream len2;    
    hexString2.erase(remove_if(hexString2.begin(), hexString2.end(), isspace), hexString2.end());
    len2 << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>((hexString2.size()/2+4));
    hexString2 = track+len2.str()+hexString2+end;
    //hexString = header+hexString1;

    hexString = header+hexString1+hexString2;
    hexString.erase(remove_if(hexString.begin(), hexString.end(), isspace), hexString.end());
    // std::cout << hexString << std::endl;
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
        ss << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>(value);
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
        std::string time = timeToHex(event.nDeltaTick);
        std::string key = int8ToHex(event.nKey);
        std::string velocity = int8ToHex(event.nVelocity);
        std::string msg = time+"90"+key+velocity;
        // printf("msg: %s\n", msg.c_str());
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
        s="";
        for (auto &event: track.vecEvents){
            s += EventHex(event);
        }
        // printf("s %x\n", s);
        msgs.push_back(s);
    }
}

void MIDI::shiftByPercentage(float p, size_t track, size_t index){
    // ConvertEventNote();
    std::vector<MidiNote>& m = vecTracks[track].vecNotes;
    int shift = int(m[index].nStartTime) + int(m[index].nDuration * p);
    m[index].nStartTime = (shift>=0)?(uint32_t)((m[index].nStartTime) + int((m[index].nDuration) * p)):0;
    
    sort(m.begin(), m.end(), [](const MidiNote& a, const MidiNote& b) { 
        return int(a.nStartTime) < int(b.nStartTime); 
    }); 
    ConvertNoteEvent();
}

bool operator==(const MidiEvent e1, const MidiEvent e2){
    if (e1.event==e2.event && e1.nKey==e2.nKey && e1.nDeltaTick==e1.nDeltaTick)
        return true;
    else 
        return false;
}

HillClimb::HillClimb(): file(nullptr){}
HillClimb::~HillClimb(){
    delete file;
}

void HillClimb::read(std::string file_name){
    // if(file!=nullptr){    
    //     printf("here");
    //     delete file;
    //     printf("here");

    //     file = nullptr;
    // }
    file = new MIDI(file_name);
    //file->showAllEvent();
}

void HillClimb::shift(float p, size_t track , size_t note){
    file->shiftByPercentage(p, track, note);
}

void HillClimb::evaluate(const std::string file_name){
    // transfer mid to ref
    std::string cmd0 = "python3 /home/tincan/code/midi_csv/midi_to_csv.py -u ";
    cmd0 += HOME+file_name+".mid";
    cmd0 += " -o "+HOME;
    system(cmd0.c_str());

    std::string cmd = "python3 ";
    cmd += "/home/tincan/code/midi2ref.py ";
    cmd += HOME+file_name+".mid";
    //cmd += HOME+"midi/"+file_name+".mid";
    system(cmd.c_str());
    
    // evaluate
    cmd = "python3 ";
    cmd += "/home/tincan/code/evaluate.py ";
    cmd += HOME+file_name+".csv";
    // printf("cmd %s\n", cmd.c_str());

    system(cmd.c_str());

}
const float EPLISON = 0.01;

void HillClimb::runPercentage(const std::string file_name){
    // srand(time(0));

    read(file_name);
    file->showAllEvent();

    MIDI best;
    float bestF1 = 0.0;
    file->ConvertEventNote();
    file->EventToMeg();
    file->showAllNote();

    int ForwardOrBackward;
    std::string _file_name;
    std::string shift_file;
    std::string tmp = HOME+"tmp.mid";
    int track=0;
    int note=0;
    file->write(tmp, file->msgs[0], file->msgs[1]);

    for (int i=0; i<HillClimb::LOOP_MAX; i++) {
        std::fstream score(HOME+"score.txt");
        std::ofstream Accscore(HOME+"Accscores.txt");

        if (i%10 == 0) 
            std::cout << "This is " << i << " loop\n";

        ForwardOrBackward = std::rand()%2;

        track = std::rand()%2;
        // track = 0;

        if(file->vecTracks[track].vecNotes.size()!=0)
            note = std::rand()%(file->vecTracks[track].vecNotes.size());
        else 
            continue;
        float p =HillClimb::PERCENTAGE * rand()/RAND_MAX;
        // if (ForwardOrBackward==0){
        //     _file_name = "shift_forward_"+std::to_string(int(HillClimb::PERCENTAGE*100))+"_"+std::to_string(track)+"_"+std::to_string(note);
        // }
        // else {
        //     _file_name = "shift_backward_"+std::to_string(int(HillClimb::PERCENTAGE*100))+"_"+std::to_string(track)+"_"+std::to_string(note);
        // }

        // shift_file = HOME+"midi/percentage/" + _file_name+".mid";
        // printf("shift_forward_file: %s\n", shift_forward_file.c_str());
        //read(file_name);

        file->ConvertEventNote();
        //file->showAllNote();
        shift(p*((ForwardOrBackward==0)?-1.0:1.0), track, note);
        // file->showAllEvent();
        file->EventToMeg();
        //printf("msg1 %s, msg2 %s\n",  file->msgs[0].c_str(), file->msgs[1].c_str());

        file->write(tmp, file->msgs[0], file->msgs[1]);
        evaluate("tmp");

        std::string fs;

        std::getline(score, fs);
        float F1_measure = std::stof(fs);
        std::cout << "F1 score: " << F1_measure << std::endl;
        std::cout << note;
        file->showAllNote();

        if(F1_measure>bestF1+EPLISON){
            std::cout << "GetBetter, Climb UP! F1: " << fs << std::endl;
            file->write("./best.mid", file->msgs[0], file->msgs[1]);
            Accscore << "F1 score: "  << F1_measure << "at loop:" << i <<"\n";
            bestF1=F1_measure;
            best.copy(file);
        }
        else{
            shift(p*((ForwardOrBackward==0)?1.0:-1.0), track, note);
        }
        score.close();
        Accscore.close();
    }
    // file->showAllEvent();
    std::cout << "Best F1 score: " << bestF1 << std::endl;

    best.showAllNote();
    
}

const float HillClimb::PERCENTAGE=0.5;
const float HillClimb::F1_BOUND=0.85;
const uint32_t HillClimb::LOOP_MAX=2;

