#include "include/midi.hpp"

int main(){
    std::cout << "init" << std::endl;
    std::string file_path("/home/tincan/code/midi/piano.mid");
    MIDI midi(file_path);
    //midi.showAllEvent();
    midi.ConvertEventNote();
    //midi.showAllNote();
    midi.ConvertNoteEvent();
    //midi.showAllEvent();
    MidiEvent e({MidiEvent::Type::NoteOn, 60, 127, 0});
    std::string s =midi.EventHex(e);
    MidiEvent e1({MidiEvent::Type::NoteOn, 70, 127, 10});
    std::string s1 =midi.EventHex(e1);
    MidiEvent e2({MidiEvent::Type::NoteOff, 60, 0, 10});
    std::string s2 =midi.EventHex(e2);
    MidiEvent e3({MidiEvent::Type::NoteOff, 70, 0, 20});
    std::string s3 =midi.EventHex(e3);
    MidiEvent e4({MidiEvent::Type::NoteOn, 80, 127, 10});
    std::string s4 =midi.EventHex(e4);
    MidiEvent e5({MidiEvent::Type::NoteOff, 80, 0, 50});
    std::string s5 =midi.EventHex(e5);

    std::string msg = s+s1+s2+s3+s4+s5;
    printf("event: %s\n", msg.c_str());
    midi.write("test.mid", msg);
    
    MIDI midi2("/home/tincan/code/cpp/test.mid");
    midi2.showAllEvent();
    midi2.ConvertEventNote();
    midi2.showAllNote();
    midi2.ConvertNoteEvent();
    midi2.showAllEvent();

}