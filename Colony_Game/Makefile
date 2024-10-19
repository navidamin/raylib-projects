# Makefile for Colony game

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -I$(SRC_DIR)

# Raylib path (adjust this to match your Raylib installation)
RAYLIB_PATH = /home/navid/Applications/raylib

# Linker flags
LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Project name and directories
PROJECT_NAME = colony
SRC_DIR = src
ENGINE_DIR = $(SRC_DIR)/Engine
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(ENGINE_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Main target
$(BIN_DIR)/$(PROJECT_NAME): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Engine source files
$(BUILD_DIR)/Engine/%.o: $(ENGINE_DIR)/%.cpp | $(BUILD_DIR)/Engine
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

$(BUILD_DIR)/Engine:
	mkdir -p $@

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean

# Default target
all: $(BIN_DIR)/$(PROJECT_NAME)
