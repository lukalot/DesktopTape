# Compiler and flags
CC = g++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -DGL_SILENCE_DEPRECATION
OBJCXX = clang++
OBJCXXFLAGS = -std=c++17 -O2 -Wall -Wextra -x objective-c++ -DGL_SILENCE_DEPRECATION

# GLFW paths
GLFW_PATH = $(shell pkg-config --variable=prefix glfw3)
GLFW_INCLUDE_PATH = $(GLFW_PATH)/include
GLFW_LIBRARY_PATH = $(GLFW_PATH)/lib

# ImGui paths
IMGUI_PATH = ./imgui
IMGUI_SOURCES = $(IMGUI_PATH)/imgui.cpp $(IMGUI_PATH)/imgui_demo.cpp $(IMGUI_PATH)/imgui_draw.cpp $(IMGUI_PATH)/imgui_tables.cpp $(IMGUI_PATH)/imgui_widgets.cpp
IMGUI_SOURCES += $(IMGUI_PATH)/backends/imgui_impl_glfw.cpp $(IMGUI_PATH)/backends/imgui_impl_opengl3.cpp

# Includes and libraries
INCLUDES = -I/opt/homebrew/Cellar/glfw/3.4/include -I./imgui -I./imgui/backends -I/opt/homebrew/include -I./glad/include -I.

LIBS = -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw

# macOS specific flags
MACOS_FLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
LDFLAGS += -framework Cocoa

# Source files
SOURCES = main.cpp window_utils.mm menu_bar_controller.mm cocoa_bridge.mm AppLauncher.mm glad/src/glad.c $(IMGUI_SOURCES)

# Output
OUTPUT = DesktopTape.app/Contents/MacOS/DesktopTape

# Build directory
BUILD_DIR = build

# Object files
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

# Targets
all: $(OUTPUT)

$(OUTPUT): $(BUILD_DIR) $(OBJS)
	@mkdir -p $(dir $@)
	$(OBJCXX) -o $@ $(OBJS) $(OBJCXXFLAGS) $(LIBS) $(MACOS_FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Add a new target to create the .app bundle
bundle: $(OUTPUT)
	@mkdir -p DesktopTape.app/Contents/Resources
	@cp Info.plist DesktopTape.app/Contents/
	@echo "APPL????" > DesktopTape.app/Contents/PkgInfo
	@if [ -d resources ] && [ "$(shell ls -A resources)" ]; then \
		cp -R resources/* DesktopTape.app/Contents/Resources/; \
	fi

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: $(IMGUI_PATH)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: $(IMGUI_PATH)/backends/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: %.mm | $(BUILD_DIR)
	$(OBJCXX) $(OBJCXXFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/glad.o: glad/src/glad.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) DesktopTape.app

.PHONY: all clean bundle