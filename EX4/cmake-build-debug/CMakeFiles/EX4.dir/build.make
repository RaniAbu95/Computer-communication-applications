# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/EX4.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/EX4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/EX4.dir/flags.make

CMakeFiles/EX4.dir/gameServer.c.o: CMakeFiles/EX4.dir/flags.make
CMakeFiles/EX4.dir/gameServer.c.o: ../gameServer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/EX4.dir/gameServer.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/EX4.dir/gameServer.c.o -c "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/gameServer.c"

CMakeFiles/EX4.dir/gameServer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/EX4.dir/gameServer.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/gameServer.c" > CMakeFiles/EX4.dir/gameServer.c.i

CMakeFiles/EX4.dir/gameServer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/EX4.dir/gameServer.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/gameServer.c" -o CMakeFiles/EX4.dir/gameServer.c.s

# Object files for target EX4
EX4_OBJECTS = \
"CMakeFiles/EX4.dir/gameServer.c.o"

# External object files for target EX4
EX4_EXTERNAL_OBJECTS =

EX4: CMakeFiles/EX4.dir/gameServer.c.o
EX4: CMakeFiles/EX4.dir/build.make
EX4: CMakeFiles/EX4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable EX4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/EX4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/EX4.dir/build: EX4
.PHONY : CMakeFiles/EX4.dir/build

CMakeFiles/EX4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/EX4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/EX4.dir/clean

CMakeFiles/EX4.dir/depend:
	cd "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4" "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4" "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug" "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug" "/Users/raniaburaia/Desktop/Azrieli/Cources/2025/Semester A/יישומי תקשורת/EX4/cmake-build-debug/CMakeFiles/EX4.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/EX4.dir/depend

