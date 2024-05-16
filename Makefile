# List all source files
SOURCES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/utils/config/*.cpp)

# Derive object file names from source file names for release and debug
OBJECTS := $(SOURCES:.cpp=.o)
DEBUG_OBJECTS := $(SOURCES:.cpp=.debug.o)

# Compiler flags
CXXFLAGS := -std=c++17 -I./dependencies -O3
DEBUG_CXXFLAGS := -std=c++17 -I./dependencies -O0 -g -Wall -Wextra

# If we're in WSL, make sure the program knows that
ifdef WSL_DISTRO_NAME
	CXXFLAGS += -DWSL=1
	DEBUG_CXXFLAGS += -DWSL=1
endif

# Linker flags
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4

# Target executables
TARGET := PartyEngineLinux
DEBUG_TARGET := PartyEngineLinuxDebug

# Main rule to build the release executable
$(TARGET): $(OBJECTS)
	clang++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile each source file into an object file
%.o: %.cpp
	clang++ $(CXXFLAGS) -c -o $@ $<

# Rule to build the debug executable
debug: $(DEBUG_OBJECTS)
	clang++ $(DEBUG_CXXFLAGS) -o $(DEBUG_TARGET) $^ $(LDFLAGS)

# Rule to compile each source file into a debug object file
%.debug.o: %.cpp
	clang++ $(DEBUG_CXXFLAGS) -c -o $@ $<

# Clean rule to remove compiled files
clean:
	rm -f $(OBJECTS) $(DEBUG_OBJECTS) $(TARGET) $(DEBUG_TARGET)