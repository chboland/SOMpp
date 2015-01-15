#!/usr/bin/env make -f

#
#  Based on Makefile by Tobias Pape
#
#
# Copyright (c) 2007 Michael Haupt, Tobias Pape
# Software Architecture Group, Hasso Plattner Institute, Potsdam, Germany
# http://www.hpi.uni-potsdam.de/swa/
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

CC			?=clang++
CFLAGS		=-std=c++11 -Wno-endif-labels -O3 -DNDEBUG $(DBG_FLAGS) $(FEATURE_FLAGS) $(INCLUDES)
LDFLAGS		=$(DBG_FLAGS) $(LIBRARIES)

INSTALL		=install

CSOM_LIBS	=-lm

CSOM_NAME	=SOM++

############ global stuff -- overridden by ../Makefile

ROOT_DIR	?= $(PWD)/..
SRC_DIR		?= $(ROOT_DIR)/src
BUILD_DIR   ?= $(ROOT_DIR)/build
DEST_DIR	?= $(ROOT_DIR)/build.out

ST_DIR		?= $(ROOT_DIR)/core-lib/Smalltalk
EX_DIR		?= $(ROOT_DIR)/core-lib/Examples
TEST_DIR	?= $(ROOT_DIR)/core-lib/TestSuite

############# "component" directories


COMPILER_DIR 	= $(SRC_DIR)/compiler
THREAD_DIR    	= $(SRC_DIR)/natives
INTERPRETER_DIR = $(SRC_DIR)/interpreter
MEMORY_DIR 		= $(SRC_DIR)/memory
STOPWORLD_DIR	= $(MEMORY_DIR)/stopTheWorld
PAUSELESS_DIR	= $(MEMORY_DIR)/pauseless
MISC_DIR 		= $(SRC_DIR)/misc
VM_DIR 			= $(SRC_DIR)/vm
VMOBJECTS_DIR 	= $(SRC_DIR)/vmobjects

COMPILER_SRC	= $(wildcard $(COMPILER_DIR)/*.cpp)
COMPILER_OBJ	= $(COMPILER_SRC:.cpp=.o)
THREAD_SRC	    = $(wildcard $(THREAD_DIR)/*.cpp)
THREAD_OBJ	    = $(THREAD_SRC:.cpp=.o)
INTERPRETER_SRC	= $(wildcard $(INTERPRETER_DIR)/*.cpp)
INTERPRETER_OBJ	= $(INTERPRETER_SRC:.cpp=.o)
MEMORY_SRC		= $(wildcard $(MEMORY_DIR)/*.cpp)
MEMORY_OBJ		= $(MEMORY_SRC:.cpp=.o)
STOPWORLD_SRC	= $(wildcard $(STOPWORLD_DIR)/*.cpp)
STOPWORLD_OBJ	= $(STOPWORLD_SRC:.cpp=.o)
PAUSELESS_SRC	= $(wildcard $(PAUSELESS_DIR)/*.cpp)
PAUSELESS_OBJ	= $(PAUSELESS_SRC:.cpp=.o)
MISC_SRC		= $(wildcard $(MISC_DIR)/*.cpp)
MISC_OBJ		= $(MISC_SRC:.cpp=.o)
VM_SRC			= $(wildcard $(VM_DIR)/*.cpp)
VM_OBJ			= $(VM_SRC:.cpp=.o)
VMOBJECTS_SRC	= $(wildcard $(VMOBJECTS_DIR)/*.cpp)
VMOBJECTS_OBJ	= $(VMOBJECTS_SRC:.cpp=.o)

MAIN_SRC		= $(wildcard $(SRC_DIR)/*.cpp)
#$(SRC_DIR)/Main.cpp
MAIN_OBJ		= $(MAIN_SRC:.cpp=.o)
#$(SRC_DIR)/main.o

############# primitives loading

PRIMITIVESCORE_DIR = $(SRC_DIR)/primitivesCore
PRIMITIVESCORE_SRC = $(wildcard $(PRIMITIVESCORE_DIR)/*.cpp)
PRIMITIVESCORE_OBJ = $(PRIMITIVESCORE_SRC:.cpp=.o)

############# primitives location etc.

PRIMITIVES_DIR	= $(SRC_DIR)/primitives
PRIMITIVES_SRC	= $(wildcard $(PRIMITIVES_DIR)/*.cpp)
PRIMITIVES_OBJ	= $(PRIMITIVES_SRC:.cpp=.o)

############# include path

INCLUDES		=-I$(SRC_DIR)
LIBRARIES		=-L$(ROOT_DIR)

##############
############## Collections.

CSOM_OBJ		=  $(MEMORY_OBJ) $(MISC_OBJ) $(VMOBJECTS_OBJ) \
				$(COMPILER_OBJ) $(INTERPRETER_OBJ) $(VM_OBJ) $(THREAD_OBJ) \
				$(STOPWORLD_OBJ) $(PAUSELESS_OBJ) 

OBJECTS			= $(CSOM_OBJ) $(PRIMITIVESCORE_OBJ) $(PRIMITIVES_OBJ) $(MAIN_OBJ)

SOURCES			=  $(COMPILER_SRC) $(INTERPRETER_SRC) $(MEMORY_SRC) \
				$(MISC_SRC) $(VM_SRC) $(VMOBJECTS_SRC)  \
				$(PRIMITIVES_SRC) $(PRIMITIVESCORE_SRC) $(MAIN_SRC) \
				$(THREAD_SRC) $(STOPWORLD_SRC) $(PAUSELESS_SRC)

############# Things to clean

CLEAN			= $(OBJECTS) \
				$(DIST_DIR) $(DEST_DIR) CORE $(CSOM_NAME)

############# Tools

#OSTOOL			= $(BUILD_DIR)/ostool

#
#
#
#  metarules
#

.PHONY: clean clobber test
# some defaults
TAGGING=false
#GC_TYPE=generational
GC_TYPE?=pauseless
CACHE_INTEGER=false
INT_CACHE_MIN_VALUE=-5
INT_CACHE_MAX_VALUE=100
GENERATE_INTEGER_HISTOGRAM=false
GENERATE_ALLOCATION_STATISTICS=false
LOG_RECEIVER_TYPES=false
UNSAFE_FRAME_OPTIMIZATION=false
ADDITIONAL_ALLOCATION=false

#
# set feature flags 
#
ifeq ($(USE_TAGGING),true)
  FEATURE_FLAGS+=-DUSE_TAGGING
endif
ifeq ($(CACHE_INTEGER),true)
  FEATURE_FLAGS+=-DCACHE_INTEGER
  FEATURE_FLAGS+=-DINT_CACHE_MIN_VALUE=$(INT_CACHE_MIN_VALUE)
  FEATURE_FLAGS+=-DINT_CACHE_MAX_VALUE=$(INT_CACHE_MAX_VALUE)
endif
ifeq ($(GC_TYPE),copying)
  FEATURE_FLAGS+=-DGC_TYPE=COPYING
endif
ifeq ($(GC_TYPE),mark_sweep)
  FEATURE_FLAGS+=-DGC_TYPE=MARK_SWEEP
endif
ifeq ($(GC_TYPE),generational)
  FEATURE_FLAGS+=-DGC_TYPE=GENERATIONAL
endif
ifeq ($(GC_TYPE),pauseless)
  FEATURE_FLAGS+=-DGC_TYPE=PAUSELESS
endif
ifeq ($(GENERATE_INTEGER_HISTOGRAM),true)
  FEATURE_FLAGS+=-DGENERATE_INTEGER_HISTOGRAM
endif
ifeq ($(GENERATE_ALLOCATION_STATISTICS),true)
  FEATURE_FLAGS+=-DGENERATE_ALLOCATION_STATISTICS
endif
ifeq ($(UNSAFE_FRAME_OPTIMIZATION),true)
  FEATURE_FLAGS+=-DUNSAFE_FRAME_OPTIMIZATION
endif
ifeq ($(LOG_RECEIVER_TYPES),true)
  FEATURE_FLAGS+=-DLOG_RECEIVER_TYPES
endif
ifeq ($(ADDITIONAL_ALLOCATION),true)
  FEATURE_FLAGS+=-DADDITIONAL_ALLOCATION
endif

FEATURE_FLAGS+=-DHEAP_SIZE=2013265920
FEATURE_FLAGS+=-DPAGE_SIZE=131072


all: $(CSOM_NAME)


debug : DBG_FLAGS=-DDEBUG -O0 -g
debug: all

profiling : DBG_FLAGS=-g -pg
profiling : LDFLAGS+=-pg
profiling: all


.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $*.o

clean:
	rm -Rf $(CLEAN)
	#just to be sure delete again
	find . -name "*.o" -delete
	-rm -Rf *.so



#
#
#
# product rules
#

$(CSOM_NAME): $(CSOM_OBJ) $(MAIN_OBJ) $(PRIMITIVESCORE_OBJ) $(PRIMITIVES_OBJ)
	@echo Linking $(CSOM_NAME)
	$(CXX) $(LDFLAGS) \
		-o $(CSOM_NAME) $(MAIN_OBJ) $(CSOM_OBJ) $(PRIMITIVESCORE_OBJ) $(PRIMITIVES_OBJ) $(CSOM_LIBS)
	@echo loader done.

install: all
	@echo installing CSOM into build
	$(INSTALL) -d $(DEST_DIR)
	$(INSTALL) $(CSOM_NAME) $(DEST_DIR)
	@echo CSOM.
	cp -Rpf $(ST_DIR) $(EX_DIR) $(TEST_DIR)  $(DEST_DIR)
	@echo Library.
	@echo done.

#
# console: start the console
#
console: all
	./$(CSOM_NAME) -cp ./Smalltalk

richards: all
	./$(CSOM_NAME) -cp ./Smalltalk ./Examples/Benchmarks/Richards/RichardsBenchmarks.som

#
# test: run the standard test suite
#
test: all
	./$(CSOM_NAME) -cp ./Smalltalk ./TestSuite/TestHarness.som

#
# bench: run the benchmarks
#
bench: all
	./$(CSOM_NAME) -cp ./Smalltalk ./Examples/Benchmarks/All.som
