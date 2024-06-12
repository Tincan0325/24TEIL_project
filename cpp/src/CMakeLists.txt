project(MIDI)
add_library(readmidi midi.cpp)
target_include_directories(readmidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)