# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tincan/code/cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tincan/code/cpp/build

# Include any dependencies generated for this target.
include CMakeFiles/MIDI.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MIDI.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MIDI.dir/flags.make

CMakeFiles/MIDI.dir/main.cpp.o: CMakeFiles/MIDI.dir/flags.make
CMakeFiles/MIDI.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tincan/code/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MIDI.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MIDI.dir/main.cpp.o -c /home/tincan/code/cpp/main.cpp

CMakeFiles/MIDI.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MIDI.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tincan/code/cpp/main.cpp > CMakeFiles/MIDI.dir/main.cpp.i

CMakeFiles/MIDI.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MIDI.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tincan/code/cpp/main.cpp -o CMakeFiles/MIDI.dir/main.cpp.s

# Object files for target MIDI
MIDI_OBJECTS = \
"CMakeFiles/MIDI.dir/main.cpp.o"

# External object files for target MIDI
MIDI_EXTERNAL_OBJECTS =

MIDI: CMakeFiles/MIDI.dir/main.cpp.o
MIDI: CMakeFiles/MIDI.dir/build.make
MIDI: src/libreadmidi.a
MIDI: CMakeFiles/MIDI.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tincan/code/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MIDI"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MIDI.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MIDI.dir/build: MIDI

.PHONY : CMakeFiles/MIDI.dir/build

CMakeFiles/MIDI.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MIDI.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MIDI.dir/clean

CMakeFiles/MIDI.dir/depend:
	cd /home/tincan/code/cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tincan/code/cpp /home/tincan/code/cpp /home/tincan/code/cpp/build /home/tincan/code/cpp/build /home/tincan/code/cpp/build/CMakeFiles/MIDI.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MIDI.dir/depend

