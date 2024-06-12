#include <fstream>
#include <iostream>
#include <array>
#include <vector>

struct MidiEvent{
	enum class Type
	{
		NoteOff,
		NoteOn,
		Other
	} event;

	uint8_t nKey = 0;
	uint8_t nVelocity = 0;
	uint32_t nDeltaTick = 0;
};

struct MidiNote
{
	uint8_t nKey = 0;
	uint8_t nVelocity = 0;
	uint32_t nStartTime = 0;
	uint32_t nDuration = 0;
};

struct MidiTrack
{
	std::string sName;
	std::string sInstrument;
	std::vector<MidiEvent> vecEvents;
	std::vector<MidiNote> vecNotes;
	uint8_t nMaxNote = 64;
	uint8_t nMinNote = 64;
};

class MIDI {
public:
	enum EventName : uint8_t
	{					
		VoiceNoteOff = 0x80,
		VoiceNoteOn = 0x90,
		VoiceAftertouch = 0xA0,
		VoiceControlChange = 0xB0,
		VoiceProgramChange = 0xC0,
		VoiceChannelPressure = 0xD0,
		VoicePitchBend = 0xE0,
		SystemExclusive = 0xF0,		
	};

	enum MetaEventName : uint8_t
	{
		MetaSequence = 0x00,
		MetaText = 0x01,
		MetaCopyright = 0x02,
		MetaTrackName = 0x03,
		MetaInstrumentName = 0x04,
		MetaLyrics = 0x05,
		MetaMarker = 0x06,
		MetaCuePoint = 0x07,
		MetaChannelPrefix = 0x20,
		MetaEndOfTrack = 0x2F,
		MetaSetTempo = 0x51,
		MetaSMPTEOffset = 0x54,
		MetaTimeSignature = 0x58,
		MetaKeySignature = 0x59,
		MetaSequencerSpecific = 0x7F,
	};

    MIDI(const std::string& file_name);
    ~MIDI();
	std::vector<MidiTrack> vecTracks;
	uint32_t m_nTempo = 0;
	uint32_t m_nBPM = 0;
    
protected:
    void ParseFile();
    std::ifstream midi;
};