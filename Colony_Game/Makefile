# Makefile for Colony Game

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external \
           -I$(SRC_DIR) -I$(SRC_DIR)/Colony -I$(SRC_DIR)/Engine -I$(SRC_DIR)/Planet -I$(SRC_DIR)/Sect -I$(SRC_DIR)/Unit

# Raylib path (adjust this to match your Raylib installation)
RAYLIB_PATH = /home/navid/Applications/raylib

# Linker flags
LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Project name and directories
PROJECT_NAME = colony_game
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/Colony/colony.cpp \
          $(SRC_DIR)/Engine/Engine.cpp \
          $(SRC_DIR)/Planet/planet.cpp \
          $(SRC_DIR)/Sect/sect.cpp \
          $(SRC_DIR)/Unit/unit.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Header files
HEADERS = $(SRC_DIR)/Colony/colony.h \
          $(SRC_DIR)/Engine/Engine.h \
          $(SRC_DIR)/Planet/planet.h \
          $(SRC_DIR)/Sect/sect.h \
          $(SRC_DIR)/Unit/unit.h

# Main target
$(BIN_DIR)/$(PROJECT_NAME): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create bin directory
$(BIN_DIR):
	mkdir -p $@

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean

# Default target
all: $(BIN_DIR)/$(PROJECT_NAME)
