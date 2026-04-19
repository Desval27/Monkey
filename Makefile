

####################################################################################################
# Directory Definitions
####################################################################################################
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
BIN_DIR := bin
BUILD_DIR := build
TEST_DIR := test

####################################################################################################
# Targets
####################################################################################################
LIB_NAME := libMonkey.a
LIB_OUT := $(LIB_DIR)/$(LIB_NAME)

####################################################################################################
# Build Options
####################################################################################################
USE_DEBUG = -DUSE_DEBUG
DEBUGFLAGS = -g
OPTIMIZEFLAGS = -O2
CXXSTANDARD = -std=c++17
#DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
DEPFLAGS = -MMD -MP

DEFINES = $(DEBUGFLAGS) $(USE_DEBUG)
INCLUDES = -I$(INC_DIR)
WARNINGS = -Wall -Wextra
EXTRA_WARNINGS = -Wno-unused-parameter -Wno-unused-variable

####################################################################################################
# Compilers and Flags
####################################################################################################
CXX := g++
CXXFLAGS = $(CXXSTANDARD) $(DEPFLAGS) $(INCLUDES) $(DEFINES) $(WARNINGS) $(EXTRA_WARNINGS)
LDFLAGS = -L$(LIB_DIR)
LDLIBS = -lMonkey

AR := ar
ARFLAGS = rcs

####################################################################################################
# Files and Targets
####################################################################################################
LIB_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)

# Collect all potential .d files for inclusion
ALL_DEPS := $(LIB_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

####################################################################################################
# Main Build Rules
####################################################################################################
.PHONY: all clean tests directories

all: directories $(LIB_OUT) tests

directories: 
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)

$(LIB_OUT): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests: $(TEST_BINS)

$(BIN_DIR)/%: $(TEST_DIR)/%.cpp $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)

echo: 
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(LIB_OUT)
	@echo $(TEST_SRCS)
	@echo $(TEST_BINS)
	@echo $(ALL_DEPS)

# Automatic Dependency Inclusion
-include $(ALL_DEPS)
