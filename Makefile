#CXX = g++
#CXX = clang++

IMGUI_DIR			:= imgui
INC_DIR				:= include
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

# core systems
SRCS += $(SRC_DIR)/core/command/command_manager.cpp
SRCS += $(SRC_DIR)/core/io/binary_file.cpp
SRCS += $(SRC_DIR)/core/render/render.cpp
SRCS += $(SRC_DIR)/core/render/texture.cpp

# program ui
SRCS += $(SRC_DIR)/blueshell/ui/util/glfw_util.cpp
SRCS += $(SRC_DIR)/blueshell/ui/util/imgui_util.cpp
SRCS += $(SRC_DIR)/blueshell/blueshell.cpp

OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

CXXFLAGS := -std=c++20 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(INC_DIR) -I./$(SRC_DIR)
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

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/commands/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/util/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/windows/%.hpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/popups/%.hpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/core/command/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/render/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/io/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/nitro/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nsmb/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: all
	./$(EXE)

clean:
	rm -rf $(BUILD_DIR)
