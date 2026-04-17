# Build Options
USE_DEBUG = 1
DEBUG_FLAG = -g

TARGETS = music_test weighted_map_example

DEFINES = -DUSE_DEBUG=$(USE_DEBUG) -Wno-unused-variable

CPP_SOURCES = \
	Monkey.cpp\
	Music.cpp \
	EventSet.cpp \
	Euclid.cpp
CPP_OBJS=$(CPP_SOURCES:%.cpp=%.o)

INCLUDES = -I./

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
DEPFILES := $(TARGETS:%=$(DEPDIR)/%.d) $(CPP_SOURCES:%.cpp=$(DEPDIR)/%.d)

# Define Variables
CXX = g++
CXXFLAGS = $(DEBUG_FLAG) $(DEFINES) $(DEPFLAGS) -pedantic -pedantic-errors -Wall -O2 $(INCLUDES)

all: $(TARGETS)

music_test: music_test.o $(CPP_OBJS)
	$(CXX) $^ -o $@

weighted_map_example: weighted_map_example.o $(CPP_OBJS)
	$(CXX) $^ -o $@

clean:
	rm -f $(TARGETS) $(TARGETS:.exe=.o) $(CPP_OBJS) $(DEPFILES)

# test.exe: test.o $(CPP_OBJS)
# 	$(CXX) test.o -o $@

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPFILES)
