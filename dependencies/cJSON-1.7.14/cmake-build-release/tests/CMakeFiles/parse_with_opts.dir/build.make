# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release

# Include any dependencies generated for this target.
include tests/CMakeFiles/parse_with_opts.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/parse_with_opts.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/parse_with_opts.dir/flags.make

tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.obj: tests/CMakeFiles/parse_with_opts.dir/flags.make
tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.obj: ../tests/parse_with_opts.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.obj"
	cd /d F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests && D:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\parse_with_opts.dir\parse_with_opts.c.obj   -c F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\tests\parse_with_opts.c

tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/parse_with_opts.dir/parse_with_opts.c.i"
	cd /d F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests && D:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\tests\parse_with_opts.c > CMakeFiles\parse_with_opts.dir\parse_with_opts.c.i

tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/parse_with_opts.dir/parse_with_opts.c.s"
	cd /d F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests && D:\msys64\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\tests\parse_with_opts.c -o CMakeFiles\parse_with_opts.dir\parse_with_opts.c.s

# Object files for target parse_with_opts
parse_with_opts_OBJECTS = \
"CMakeFiles/parse_with_opts.dir/parse_with_opts.c.obj"

# External object files for target parse_with_opts
parse_with_opts_EXTERNAL_OBJECTS =

tests/parse_with_opts.exe: tests/CMakeFiles/parse_with_opts.dir/parse_with_opts.c.obj
tests/parse_with_opts.exe: tests/CMakeFiles/parse_with_opts.dir/build.make
tests/parse_with_opts.exe: libcjson.dll.a
tests/parse_with_opts.exe: tests/libunity.a
tests/parse_with_opts.exe: tests/CMakeFiles/parse_with_opts.dir/linklibs.rsp
tests/parse_with_opts.exe: tests/CMakeFiles/parse_with_opts.dir/objects1.rsp
tests/parse_with_opts.exe: tests/CMakeFiles/parse_with_opts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable parse_with_opts.exe"
	cd /d F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\parse_with_opts.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/parse_with_opts.dir/build: tests/parse_with_opts.exe

.PHONY : tests/CMakeFiles/parse_with_opts.dir/build

tests/CMakeFiles/parse_with_opts.dir/clean:
	cd /d F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests && $(CMAKE_COMMAND) -P CMakeFiles\parse_with_opts.dir\cmake_clean.cmake
.PHONY : tests/CMakeFiles/parse_with_opts.dir/clean

tests/CMakeFiles/parse_with_opts.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14 F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\tests F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests F:\Developement\active\seadoggo\dependencies\cJSON-1.7.14\cmake-build-release\tests\CMakeFiles\parse_with_opts.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/parse_with_opts.dir/depend

