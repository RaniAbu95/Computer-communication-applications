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
CMAKE_SOURCE_DIR = /Users/raniaburaia/Desktop/send_request

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/raniaburaia/Desktop/send_request/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/send_request.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/send_request.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/send_request.dir/flags.make

CMakeFiles/send_request.dir/client.c.o: CMakeFiles/send_request.dir/flags.make
CMakeFiles/send_request.dir/client.c.o: ../client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/raniaburaia/Desktop/send_request/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/send_request.dir/client.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/send_request.dir/client.c.o -c /Users/raniaburaia/Desktop/send_request/client.c

CMakeFiles/send_request.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/send_request.dir/client.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/raniaburaia/Desktop/send_request/client.c > CMakeFiles/send_request.dir/client.c.i

CMakeFiles/send_request.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/send_request.dir/client.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/raniaburaia/Desktop/send_request/client.c -o CMakeFiles/send_request.dir/client.c.s

# Object files for target send_request
send_request_OBJECTS = \
"CMakeFiles/send_request.dir/client.c.o"

# External object files for target send_request
send_request_EXTERNAL_OBJECTS =

send_request: CMakeFiles/send_request.dir/client.c.o
send_request: CMakeFiles/send_request.dir/build.make
send_request: CMakeFiles/send_request.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/raniaburaia/Desktop/send_request/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable send_request"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/send_request.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/send_request.dir/build: send_request
.PHONY : CMakeFiles/send_request.dir/build

CMakeFiles/send_request.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/send_request.dir/cmake_clean.cmake
.PHONY : CMakeFiles/send_request.dir/clean

CMakeFiles/send_request.dir/depend:
	cd /Users/raniaburaia/Desktop/send_request/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/raniaburaia/Desktop/send_request /Users/raniaburaia/Desktop/send_request /Users/raniaburaia/Desktop/send_request/cmake-build-debug /Users/raniaburaia/Desktop/send_request/cmake-build-debug /Users/raniaburaia/Desktop/send_request/cmake-build-debug/CMakeFiles/send_request.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/send_request.dir/depend

