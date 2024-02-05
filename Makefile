# List all source files
SOURCES := src/GameEngine.cpp src/Renderer.cpp src/gamedata/Actor.cpp src/gamedata/Scene.cpp src/utils/ConfigUtils.cpp src/utils/StringUtils.cpp

# Derive object file names from source file names
OBJECTS := $(SOURCES:.cpp=.o)

# Compiler flags
CXXFLAGS := -std=c++17 -I./dependencies -O2

# Target executable
TARGET := game_engine_linux

# Main rule to build the executable
$(TARGET): $(OBJECTS)
	clang++ $(CXXFLAGS) -o $@ $^

# Rule to compile each source file into an object file
%.o: %.cpp
	clang++ $(CXXFLAGS) -c -o $@ $<

# Clean rule to remove compiled files
clean:
	rm -f $(OBJECTS) $(TARGET)
