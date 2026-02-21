# Makefile for DJ Track Session Manager Assignment
# C++ Memory Management Assignment - BGU SPL Course

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g -Weffc++
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Include path
INCLUDES = -I$(INC_DIR)

DEBUG_FLAGS = -DDEBUG
RELEASE_FLAGS = -DNDEBUG

# Source files (from src directory)
SOURCES = \
	$(SRC_DIR)/AudioTrack.cpp \
	$(SRC_DIR)/CacheSlot.cpp \
	$(SRC_DIR)/ConfigurationManager.cpp \
	$(SRC_DIR)/DJSession.cpp \
	$(SRC_DIR)/DJLibraryService.cpp \
	$(SRC_DIR)/DJControllerService.cpp \
	$(SRC_DIR)/MixingEngineService.cpp \
	$(SRC_DIR)/LRUCache.cpp \
	$(SRC_DIR)/MP3Track.cpp \
	$(SRC_DIR)/Playlist.cpp \
	$(SRC_DIR)/SessionFileParser.cpp \
	$(SRC_DIR)/WAVTrack.cpp \
	$(SRC_DIR)/main.cpp

# Object files (placed in bin directory)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SOURCES))

# Phase 4 specific objects
PHASE4_OBJECTS = $(BIN_DIR)/DJSession.o $(BIN_DIR)/SessionFileParser.o

# Target executable (placed in bin)
TARGET = $(BIN_DIR)/dj_manager

# Default target
all: dirs $(TARGET)

# Ensure bin directory exists
dirs:
	mkdir -p $(BIN_DIR)

# Build the main executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Build with debug flags
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all
	@echo "Debug build complete!"

# Build for release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: all
	@echo "Release build complete!"

# Compile source files to bin/*.o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Memory leak testing with valgrind
test-leaks: debug
	@echo "Running memory leak test with valgrind..."
	@echo "Note: Install valgrind first: sudo apt-get install valgrind"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# test run
test: $(TARGET)
	@echo "Running quick test..."
	./$(TARGET)

# Clean up build files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing development dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential g++ valgrind gdb

# Help target
help:
	@echo "DJ Track Library Manager - Build Targets:"
	@echo ""
	@echo "  all          - Build the program (default)"
	@echo "  debug        - Build with debug information"
	@echo "  release      - Build optimized version"
	@echo "  test         - Run the program"
	@echo "  test-leaks   - Run with valgrind memory leak detection"
	@echo "  clean        - Remove build files"
	@echo "  install-deps - Install required development tools"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "STUDENT WORKFLOW:"
	@echo "  1. make debug        # Build with debug information"
	@echo "  2. make test         # Run the program"
	@echo "  3. Fix the TODOs in the code"
	@echo "  4. make test-leaks   # Check with valgrind"
	@echo "  5. Repeat until no leaks found!"

examination:
	@echo "This is a placeholder for examination-specific targets."
	./test.sh
# Phony targets
.PHONY: all debug sanitize release test test-leaks clean install-deps help examination
