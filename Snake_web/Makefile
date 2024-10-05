# Define required environment variables
PLATFORM = PLATFORM_WEB
PROJECT_NAME = snake_web
PROJECT_VERSION = 1.0
PROJECT_BUILD_PATH = ./

RAYLIB_PATH = /home/navid/Applications/raylib
RAYLIB_INCLUDE_PATH = $(RAYLIB_PATH)/src
RAYLIB_LIB_PATH = $(RAYLIB_PATH)/src

# Compiler and flags
CC = emcc
CFLAGS = -Os -Wall -s USE_GLFW=3 -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY -DPLATFORM_WEB

# Include paths
INCLUDE_PATHS = -I. -I$(RAYLIB_INCLUDE_PATH) -I$(RAYLIB_PATH)/external

# Library paths and libs
LDFLAGS = -L. -L$(RAYLIB_LIB_PATH)
LDLIBS = $(RAYLIB_LIB_PATH)/libraylib.a

# Source files
PROJECT_SOURCE_FILES = main.cpp

# Web shell
BUILD_WEB_SHELL = $(RAYLIB_PATH)/src/minshell.html

# Output
EXT = .html

# Resources dir (used for --preload-file)
RESOURCES_DIR = resources

# Default target entry
all: $(PROJECT_NAME)

# Project target
$(PROJECT_NAME): $(PROJECT_SOURCE_FILES)
	$(CC) -o $(PROJECT_NAME)$(EXT) $(PROJECT_SOURCE_FILES) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) \
		-s FORCE_FILESYSTEM=1 \
		--shell-file $(BUILD_WEB_SHELL) \
		--preload-file resources \


# Clean target
clean:
	rm -f $(PROJECT_NAME).html $(PROJECT_NAME).js $(PROJECT_NAME).wasm $(PROJECT_NAME).data

.PHONY: all clean
