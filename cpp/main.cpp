#include "include/midi.hpp"

int main(){
    std::cout << "init" << std::endl;
    std::string file_path("/home/tincan/Course/third_grade/second_semester/project/TEIL_project/code/transcribe/Track00001/piano/only_piano_sounds_basic_pitch.mid");
    MIDI midi(file_path);
    
}