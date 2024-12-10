# Compiler settings #
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./include -I./pkg/nlohmann/json/include

# SFML settings #
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

-DCMAKE_EXPORT_COMPILE_COMMANDS=1
# Target settings
TARGET = program
SRC = $(wildcard src/*.cpp) 
OBJ_DIR = build
OBJ = $(OBJ_DIR)/main.o

# Default target to build the app
all: $(OBJ_DIR)/$(TARGET)

# Rule to create the executable #
$(OBJ_DIR)/$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(OBJ_DIR)/$(TARGET) $(SFML_FLAGS)

# Rule to compile .cpp to .o #
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)  # Create the build directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build files #
clean:
	rm -rf $(OBJ_DIR)

# Run the program (rebuilds if needed) #
run: all
	./$(OBJ_DIR)/$(TARGET)


rerun: clean all
	@echo "Rebuilding and Rerunning..."
	./$(OBJ_DIR)/$(TARGET)