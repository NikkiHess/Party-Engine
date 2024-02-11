# List all source files
SOURCES := $(wildcard src/*.cpp) $(wildcard src/gamedata/*.cpp) $(wildcard src/utils/*.cpp) $(wildcard src/visuals/*.cpp)

# Derive object file names from source file names
OBJECTS := $(SOURCES:.cpp=.o)

# Compiler flags
CXXFLAGS := -std=c++17 -I./dependencies -O2

# Linker flags
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Target executable
TARGET := game_engine_linux

# Main rule to build the executable
$(TARGET): $(OBJECTS)
	clang++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile each source file into an object file
%.o: %.cpp
	clang++ $(CXXFLAGS) -c -o $@ $<

# Clean rule to remove compiled files
clean:
	rm -f $(OBJECTS) $(TARGET)
