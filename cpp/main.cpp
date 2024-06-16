#include "include/midi.hpp"

int main(int argc, char* argv[]){
    // std::cout << "init" << std::endl;
    std::string file_path("/home/tincan/code/cpp/build/test1.mid");
    // MIDI midi(file_path);
    // midi.showAllNote();
    // MidiEvent e({MidiEvent::Type::NoteOn, 60, 127, 0});
    // std::string s =midi.EventHex(e);
    // MidiEvent e1({MidiEvent::Type::NoteOn, 70, 127, 100});
    // std::string s1 =midi.EventHex(e1);
    // MidiEvent e2({MidiEvent::Type::NoteOff, 60, 0, 100});
    // std::string s2 =midi.EventHex(e2);
    // MidiEvent e3({MidiEvent::Type::NoteOff, 70, 0, 200});
    // std::string s3 =midi.EventHex(e3);
    // MidiEvent e4({MidiEvent::Type::NoteOn, 80, 127, 100});
    // std::string s4 =midi.EventHex(e4);
    // MidiEvent e5({MidiEvent::Type::NoteOff, 80, 0, 300});
    // std::string s5 =midi.EventHex(e5);
    // std::string msg = s+s1+s2+s3+s4+s5;
    // midi.write("test.mid", msg,"");
    // MidiEvent e({MidiEvent::Type::NoteOn, 60, 127, 15});
    // std::string s =midi.EventHex(e);
    // MidiEvent e1({MidiEvent::Type::NoteOn, 70, 127, 0});
    // std::string s1 =midi.EventHex(e1);
    // MidiEvent e2({MidiEvent::Type::NoteOff, 60, 0, 200});
    // std::string s2 =midi.EventHex(e2);
    // MidiEvent e3({MidiEvent::Type::NoteOff, 70, 0, 100});
    // std::string s3 =midi.EventHex(e3);
    // MidiEvent e4({MidiEvent::Type::NoteOn, 80, 127, 200});
    // std::string s4 =midi.EventHex(e4);
    // MidiEvent e5({MidiEvent::Type::NoteOff, 80, 0, 300});
    // std::string s5 =midi.EventHex(e5);
    // std::string msg = s+s1+s2+s3+s4+s5;
    // midi.write("test1.mid", msg, "");

    // midi.showAllEvent();
    // midi.showAllNote();
    // midi.showAllEvent();
    
    HillClimb HC;
    HC.runPercentage(file_path);
    //HC.evaluate("test1");
    //MIDI midi2("/home/tincan/code/cpp/test.mid");
    
    // midi2.ConvertEventNote();
    //midi.showAllNote();
    // midi2.ConvertNoteEvent();
    //midi2.showAllEvent();
    // std::string file_name("/home/tincan/code/midi/piano.mid");

}