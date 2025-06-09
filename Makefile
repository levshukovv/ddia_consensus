# Simple Makefile for DDIA Consensus Project
# No CMake required - just traditional make

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
LDFLAGS = -lcurl 

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source files (automatically find all .cpp files)
SOURCES = $(wildcard *.cpp $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
TARGET = consensus

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)

# Build the main target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

# Compile source files
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug with gdb
debug: $(TARGET)
	gdb ./$(TARGET)

# Memory check with valgrind
memcheck: $(TARGET)
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Create necessary directories
setup:
	mkdir -p $(SRC_DIR) $(INCLUDE_DIR) tests

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build files"
	@echo "  rebuild   - Clean and build"
	@echo "  run       - Build and run the program"
	@echo "  debug     - Run with gdb debugger"
	@echo "  memcheck  - Run with valgrind memory checker"
	@echo "  setup     - Create project directory structure"
	@echo "  help      - Show this help"

# Declare phony targets
.PHONY: all clean rebuild run debug memcheck setup help
