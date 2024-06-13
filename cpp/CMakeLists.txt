cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MIDI)
find_package(Python3 COMPONENTS Development REQUIRED)

add_subdirectory(include)
add_subdirectory(src)

set(CMAKE_BUILD_TYPE Debug)
add_executable(MIDI main.cpp)

target_include_directories(MIDI PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(MIDI readmidi ${Python3_LIBRARIES})
target_compile_options(MIDI PUBLIC -g -I/usr/include/python3.8/cpython -I/usr/include/python3.8/ )

