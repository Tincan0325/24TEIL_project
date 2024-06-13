cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MIDI)

add_subdirectory(include)
add_subdirectory(src)
set(CMAKE_BUILD_TYPE Debug)
add_executable(MIDI main.cpp)
target_link_libraries(MIDI readmidi)
target_compile_options(MIDI PUBLIC -g)