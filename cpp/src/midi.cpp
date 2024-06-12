#include "midi.hpp"

MIDI::MIDI(const std::string& file_name){
    this->midi.open(file_name, std::fstream::in | std::ios::binary);
    if(!this->midi.is_open())
        std::cerr << "Midi file open failed." << std::endl;

    this->ParseFile();
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
                struct MidiEvent e = {.event=MidiEvent::Type::NoteOff, .nKey=nNoteVelocity, .nDeltaTick=nStatusTimeDelta, .nVelocity=nNoteVelocity};
                vecTracks[nChunk].vecEvents.push_back(e);
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
