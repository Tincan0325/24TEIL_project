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
include src/CMakeFiles/readmidi.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/readmidi.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/readmidi.dir/flags.make

src/CMakeFiles/readmidi.dir/midi.cpp.o: src/CMakeFiles/readmidi.dir/flags.make
src/CMakeFiles/readmidi.dir/midi.cpp.o: ../src/midi.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tincan/code/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/readmidi.dir/midi.cpp.o"
	cd /home/tincan/code/cpp/build/src && /home/tincan/miniconda3/envs/AMT/bin/x86_64-conda-linux-gnu-c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/readmidi.dir/midi.cpp.o -c /home/tincan/code/cpp/src/midi.cpp

src/CMakeFiles/readmidi.dir/midi.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/readmidi.dir/midi.cpp.i"
	cd /home/tincan/code/cpp/build/src && /home/tincan/miniconda3/envs/AMT/bin/x86_64-conda-linux-gnu-c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tincan/code/cpp/src/midi.cpp > CMakeFiles/readmidi.dir/midi.cpp.i

src/CMakeFiles/readmidi.dir/midi.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/readmidi.dir/midi.cpp.s"
	cd /home/tincan/code/cpp/build/src && /home/tincan/miniconda3/envs/AMT/bin/x86_64-conda-linux-gnu-c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tincan/code/cpp/src/midi.cpp -o CMakeFiles/readmidi.dir/midi.cpp.s

# Object files for target readmidi
readmidi_OBJECTS = \
"CMakeFiles/readmidi.dir/midi.cpp.o"

# External object files for target readmidi
readmidi_EXTERNAL_OBJECTS =

src/libreadmidi.a: src/CMakeFiles/readmidi.dir/midi.cpp.o
src/libreadmidi.a: src/CMakeFiles/readmidi.dir/build.make
src/libreadmidi.a: src/CMakeFiles/readmidi.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tincan/code/cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libreadmidi.a"
	cd /home/tincan/code/cpp/build/src && $(CMAKE_COMMAND) -P CMakeFiles/readmidi.dir/cmake_clean_target.cmake
	cd /home/tincan/code/cpp/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/readmidi.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/readmidi.dir/build: src/libreadmidi.a

.PHONY : src/CMakeFiles/readmidi.dir/build

src/CMakeFiles/readmidi.dir/clean:
	cd /home/tincan/code/cpp/build/src && $(CMAKE_COMMAND) -P CMakeFiles/readmidi.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/readmidi.dir/clean

src/CMakeFiles/readmidi.dir/depend:
	cd /home/tincan/code/cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tincan/code/cpp /home/tincan/code/cpp/src /home/tincan/code/cpp/build /home/tincan/code/cpp/build/src /home/tincan/code/cpp/build/src/CMakeFiles/readmidi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/readmidi.dir/depend
