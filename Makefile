# SPDX-License-Identifier: CC0-1.0 

###############################################################################
#* @file Makefile
#* @brief General Dev Utilities.
#* @author pfburdette <paul.f.burdette@gmail.com>
#*
#* @copyright This work is dedicated to the public domain under CC0 1.0.
#* To the extent possible under law, the author(s) have waived all copyright
#* and related or neighboring rights to this software.
#* See <http://creativecommons.org>
###############################################################################

###############################################################################
# Directory Definitions
###############################################################################
SRC_DIR := ./src
INC_DIR := ./include
LIB_DIR := ./lib
BIN_DIR := ./bin
BUILD_DIR := ./build
TEST_DIR := ./test
EXAMPLES_DIR := ./examples
# Sometimes put in by AI using PowerShell
BAD_DIR := ./-p

###############################################################################
# Targets
###############################################################################
LIB_NAME := libMonkey.a
LIB_OUT := $(LIB_DIR)/$(LIB_NAME)

###############################################################################
# Build Options
###############################################################################
USE_DEBUG = -DUSE_DEBUG 
NO_USE_DEBUG = -UUSE_DEBUG 
DEBUGFLAGS = -g
OPTIMIZEFLAGS = -O2
CXXSTANDARD = -std=c++20
#CXXSTANDARD = -std=c++14
#DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
DEPFLAGS = -MMD -MP

DEFINES = $(DEBUGFLAGS) $(USE_DEBUG)
INCLUDES = -I$(INC_DIR)
TEST_INCLUDES = $(INCLUDES) -I$(TEST_DIR)
WARNINGS = -Wall -Wextra
EXTRA_WARNINGS = -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable

###############################################################################
# Compilers and Flags
###############################################################################
#CXX := g++
CXXFLAGS = $(CXXSTANDARD) $(DEPFLAGS) $(INCLUDES) $(DEFINES) $(WARNINGS) $(EXTRA_WARNINGS)
TEST_CXXFLAGS = $(CXXSTANDARD) $(DEPFLAGS) $(TEST_INCLUDES) $(DEFINES) $(WARNINGS) $(EXTRA_WARNINGS) $(NO_USE_DEBUG)
LDFLAGS = -L$(LIB_DIR)
LDLIBS = -lMonkey

#AR := ar
ARFLAGS = rcs

NM := gcc-nm

###############################################################################
# Files and Targets
###############################################################################
LIB_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TEST_SRCS := $(wildcard $(TEST_DIR)/*_test.cpp)
TEST_BINS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)
EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_BINS := $(EXAMPLE_SRCS:$(EXAMPLES_DIR)/%.cpp=$(BIN_DIR)/%)

# Collect all potential .d files for inclusion
ALL_DEPS := $(LIB_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

###############################################################################
# Main Build Rules
###############################################################################
.PHONY: all clean tests examples directories

all: directories $(LIB_OUT) examples 

directories: 
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR) $(EXAMPLES_DIR)

$(LIB_OUT): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests: directories $(TEST_BINS)
	@set -e; for test_bin in $(TEST_BINS); do ./$$test_bin; done

examples: directories $(EXAMPLE_BINS)

$(BIN_DIR)/%: $(TEST_DIR)/%.cpp $(LIB_OUT)
	$(CXX) $(TEST_CXXFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.cpp $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR) $(BAD_DIR)

size: $(LIB_OUT)
	@echo "Size of $(LIB_OUT)"
	$(NM) --print-size --size-sort --reverse-sort $^
	

echo: 
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(LIB_OUT)
	@echo $(TEST_SRCS)
	@echo $(TEST_BINS)
	@echo $(EXAMPLE_SRCS)
	@echo $(EXAMPLE_BINS)
	@echo $(ALL_DEPS)

# Automatic Dependency Inclusion
-include $(ALL_DEPS)
