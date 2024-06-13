#pragma once
#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <utility> 
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdint>

class HillClimb;

struct MidiEvent{
	enum class Type
	{
		NoteOff,
		NoteOn,
		Other
	} event;

	uint8_t nKey;
	uint8_t nVelocity;
	uint32_t nDeltaTick;

	MidiEvent(Type type, uint8_t key=0, uint8_t v=0, uint32_t t=0): event(type), nKey(key), nVelocity(v), nDeltaTick(t){};
};

struct MidiNote
{
	uint8_t nKey = 0;
	uint8_t nVelocity = 0;
	uint32_t nStartTime = 0;
	uint32_t nDuration = 0;

	MidiNote(uint8_t key, uint8_t v=0, uint32_t start=0, uint32_t d=0): nKey(key), nVelocity(v), nStartTime(start), nDuration(d){};
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
	friend class HillClimb;
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
    
	void shift(float percentage, int track, int index);
	const void showAllEvent(); 
	const void showAllNote(); 
	void ConvertEventNote();
	void ConvertNoteEvent();
	void write(const std::string& file, std::string& hexString, std::string& hexString2);
	std::string EventHex(const MidiEvent& event);
	void EventToMeg();
	void shiftByPercentage(float p, size_t index);
	std::vector<std::string> msgs;

protected:
    void ParseFile();
    std::ifstream midi;
};

bool operator==(const MidiEvent e1, const MidiEvent e2);

class HillClimb{
public:
    HillClimb();
    void read(const std::string &file_name);
protected:
    MIDI* file;
};

