# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dest/sweeterthancandy/gpack

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dest/sweeterthancandy/gpack

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/dest/sweeterthancandy/gpack/CMakeFiles /home/dest/sweeterthancandy/gpack/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/dest/sweeterthancandy/gpack/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named gpack_test

# Build rule for target.
gpack_test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 gpack_test
.PHONY : gpack_test

# fast build rule for target.
gpack_test/fast:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/build
.PHONY : gpack_test/fast

test/byte.o: test/byte.cpp.o

.PHONY : test/byte.o

# target to build an object file
test/byte.cpp.o:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/byte.cpp.o
.PHONY : test/byte.cpp.o

test/byte.i: test/byte.cpp.i

.PHONY : test/byte.i

# target to preprocess a source file
test/byte.cpp.i:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/byte.cpp.i
.PHONY : test/byte.cpp.i

test/byte.s: test/byte.cpp.s

.PHONY : test/byte.s

# target to generate assembly for a file
test/byte.cpp.s:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/byte.cpp.s
.PHONY : test/byte.cpp.s

test/formats.o: test/formats.cpp.o

.PHONY : test/formats.o

# target to build an object file
test/formats.cpp.o:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/formats.cpp.o
.PHONY : test/formats.cpp.o

test/formats.i: test/formats.cpp.i

.PHONY : test/formats.i

# target to preprocess a source file
test/formats.cpp.i:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/formats.cpp.i
.PHONY : test/formats.cpp.i

test/formats.s: test/formats.cpp.s

.PHONY : test/formats.s

# target to generate assembly for a file
test/formats.cpp.s:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/formats.cpp.s
.PHONY : test/formats.cpp.s

test/serialization.o: test/serialization.cpp.o

.PHONY : test/serialization.o

# target to build an object file
test/serialization.cpp.o:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/serialization.cpp.o
.PHONY : test/serialization.cpp.o

test/serialization.i: test/serialization.cpp.i

.PHONY : test/serialization.i

# target to preprocess a source file
test/serialization.cpp.i:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/serialization.cpp.i
.PHONY : test/serialization.cpp.i

test/serialization.s: test/serialization.cpp.s

.PHONY : test/serialization.s

# target to generate assembly for a file
test/serialization.cpp.s:
	$(MAKE) -f CMakeFiles/gpack_test.dir/build.make CMakeFiles/gpack_test.dir/test/serialization.cpp.s
.PHONY : test/serialization.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... gpack_test"
	@echo "... test/byte.o"
	@echo "... test/byte.i"
	@echo "... test/byte.s"
	@echo "... test/formats.o"
	@echo "... test/formats.i"
	@echo "... test/formats.s"
	@echo "... test/serialization.o"
	@echo "... test/serialization.i"
	@echo "... test/serialization.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

