# Build Options
USE_DEBUG = 1
DEBUG_FLAG = -g

TARGETS = music_test.exe weighted_map_example.exe

DEFINES = -DUSE_DEBUG=$(USE_DEBUG)

CPP_SOURCES = \
	Monkey.cpp\
	Music.cpp \
	Euclid.cpp
CPP_OBJS=$(CPP_SOURCES:%.cpp=%.o)

INCLUDES = -I./

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
DEPFILES := $(TARGETS:%.exe=$(DEPDIR)/%.d) $(CPP_SOURCES:%.cpp=$(DEPDIR)/%.d)

# Define Variables
CXX = g++
CXXFLAGS = $(DEBUG_FLAG) $(DEFINES) $(DEPFLAGS) -pedantic -pedantic-errors -Wall -O2 $(INCLUDES)

all: $(TARGETS)

music_test.exe: music_test.o $(CPP_OBJS)
	$(CXX) $^ -o $@

weighted_map_example.exe: weighted_map_example.o $(CPP_OBJS)
	$(CXX) $^ -o $@

clean:
	rm -f $(TARGETS) $(TARGETS:.exe=.o) $(CPP_OBJS) $(DEPFILES)

# test.exe: test.o $(CPP_OBJS)
# 	$(CXX) test.o -o $@

%.o: %.cpp $(DEPDIR)/%.d
	$(CXX) $(CXXFLAGS) -c $< -o $@


-include $(DEPFILES)
