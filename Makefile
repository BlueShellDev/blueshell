#CXX = g++
#CXX = clang++

IMGUI_DIR 			:= imgui
IMGUI_EXTRAS_DIR 	:= imgui-extras
STB_DIR 			:= stb
SRC_DIR 			:= src
BUILD_DIR 			:= build
BIN_DIR 			:= bin
EXE 				:= $(BIN_DIR)/BlueShell

SRCS := $(SRC_DIR)/main.cpp

# imgui 
SRCS += $(IMGUI_DIR)/imgui.cpp 
SRCS += $(IMGUI_DIR)/imgui_demo.cpp 
SRCS += $(IMGUI_DIR)/imgui_draw.cpp 
SRCS += $(IMGUI_DIR)/imgui_tables.cpp 
SRCS += $(IMGUI_DIR)/imgui_widgets.cpp
SRCS += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp 
SRCS += $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# core utils
SRCS += $(SRC_DIR)/core/util/glfw_util.cpp
SRCS += $(SRC_DIR)/core/util/imgui_util.cpp

# core systems
SRCS += $(SRC_DIR)/core/systems/command/command_manager.cpp
SRCS += $(SRC_DIR)/core/systems/io/binary_file.cpp
SRCS += $(SRC_DIR)/core/systems/render/render.cpp
SRCS += $(SRC_DIR)/core/systems/render/texture.cpp

# nitro
SRCS += $(SRC_DIR)/nitro/graphics/ncg.cpp
SRCS += $(SRC_DIR)/nitro/graphics/ncl.cpp
SRCS += $(SRC_DIR)/nitro/graphics/nsc.cpp

# nsmb
SRCS += $(SRC_DIR)/nsmb/tileset/tileset_definition.cpp
SRCS += $(SRC_DIR)/nsmb/tileset/tileset_object.cpp
SRCS += $(SRC_DIR)/nsmb/tileset/map16/map16.cpp

# program ui
SRCS += $(SRC_DIR)/ui/windows/console_viewer.cpp
SRCS += $(SRC_DIR)/ui/windows/textures_viewer.cpp
SRCS += $(SRC_DIR)/ui/windows/tileset_definitions_editor.cpp

OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

CXXFLAGS := -std=c++20 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(STB_DIR) -I$(IMGUI_EXTRAS_DIR) -I./$(SRC_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS :=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    ECHO_MESSAGE := Linux
    LINUX_GL_LIBS := -lGL
    LIBS += $(LINUX_GL_LIBS) -lX11 `pkg-config --static --libs glfw3`
    CXXFLAGS += `pkg-config --cflags glfw3`
endif

ifeq ($(UNAME_S), Darwin)
    ECHO_MESSAGE := Mac OS X
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib -lglfw
    CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
endif

ifeq ($(OS), Windows_NT)
    ECHO_MESSAGE := MinGW
    LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32
    CXXFLAGS += `pkg-config --cflags glfw3`
endif

.PHONY: all clean run dirs

all: dirs $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

dirs:
	mkdir -p $(BUILD_DIR) $(BIN_DIR) $(ASSET_DIR)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(IMGUI_DIR)/backends/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(IMGUI_EXTRAS_DIR)/backends/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/commands/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/ui/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/ui/windows/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/core/util/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/systems/command/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/systems/console/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/systems/render/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/systems/io/%.cpp | dirs 

	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nitro/filesystem/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nitro/graphics/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<


$(BUILD_DIR)/%.o: $(SRC_DIR)/nsmb/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nsmb/tileset/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nsmb/tileset/map16/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: all
	./$(EXE)

clean:
	rm -rf $(BUILD_DIR)
