# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/dekruptos/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/183.5429.37/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/dekruptos/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/183.5429.37/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dekruptos/MyData/MorseCodeRS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dekruptos/MyData/MorseCodeRS/cmake-build-debug

# Utility rule file for MorseCodeRS-burn.

# Include the progress variables for this target.
include CMakeFiles/MorseCodeRS-burn.dir/progress.make

CMakeFiles/MorseCodeRS-burn: MorseCodeRS.elf
	/usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -cstk500v2 -Pusb -patmega328p -v -Uflash:w:/home/dekruptos/MyData/MorseCodeRS/cmake-build-debug/MorseCodeRS.hex

MorseCodeRS-burn: CMakeFiles/MorseCodeRS-burn
MorseCodeRS-burn: CMakeFiles/MorseCodeRS-burn.dir/build.make

.PHONY : MorseCodeRS-burn

# Rule to build all files generated by this target.
CMakeFiles/MorseCodeRS-burn.dir/build: MorseCodeRS-burn

.PHONY : CMakeFiles/MorseCodeRS-burn.dir/build

CMakeFiles/MorseCodeRS-burn.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MorseCodeRS-burn.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MorseCodeRS-burn.dir/clean

CMakeFiles/MorseCodeRS-burn.dir/depend:
	cd /home/dekruptos/MyData/MorseCodeRS/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dekruptos/MyData/MorseCodeRS /home/dekruptos/MyData/MorseCodeRS /home/dekruptos/MyData/MorseCodeRS/cmake-build-debug /home/dekruptos/MyData/MorseCodeRS/cmake-build-debug /home/dekruptos/MyData/MorseCodeRS/cmake-build-debug/CMakeFiles/MorseCodeRS-burn.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MorseCodeRS-burn.dir/depend

