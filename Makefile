#CXX = g++
#CXX = clang++

BUILD_DIR 			:= build
BIN_DIR 			:= bin
DEPS_DIR			:= deps
INC_DIR				:= include
SRC_DIR 			:= src

IMGUI_DIR			:= ./$(DEPS_DIR)/imgui
YAML_CPP_DIR  		:= ./$(DEPS_DIR)/yaml-cpp

EXE 				:= $(BIN_DIR)/BlueShell

# blueshell
SRC_CPP := $(SRC_DIR)/blueshell/main.cpp

# imgui 
SRC_CPP += $(wildcard $(IMGUI_DIR)/*.cpp)
SRC_CPP += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp 
SRC_CPP += $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

SRC_CPP += $(SRC_DIR)/core/io/binary_file.cpp
SRC_CPP += $(SRC_DIR)/core/render/render.cpp
SRC_CPP += $(SRC_DIR)/core/render/texture.cpp
SRC_CPP += $(SRC_DIR)/core/command/command_manager.cpp
SRC_CPP += $(wildcard $(SRC_DIR)/nitro/impl/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/nsmb/impl/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/commands/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/plugins/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/ui/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/ui/util/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/ui/windows/*.cpp)
SRC_CPP += $(wildcard $(SRC_DIR)/blueshell/ui/widgets/*.cpp)

# yamlcpp
SRC_CPP += $(wildcard $(YAML_CPP_DIR)/src/*.cpp)

OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRC_CPP)))

CXXFLAGS := -std=c++20
# add includes for imgui
CXXFLAGS += -I$(IMGUI_DIR) 
CXXFLAGS += -I$(IMGUI_DIR)/backends
# blueshell/imgui extension shit
CXXFLAGS += -I$(INC_DIR) 
CXXFLAGS += -I$(INC_DIR)/imgui-3rd-party
CXXFLAGS += -I./$(SRC_DIR)
# add includes for yamlcpp
CXXFLAGS += -I./$(YAML_CPP_DIR)/include

CXXFLAGS += -g -Wall -Wformat

LIBS :=
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    ECHO_MESSAGE := Linux
    LINUX_GL_LIBS := -lGL
    LIBS += $(LINUX_GL_LIBS) -lX11 -ldl `pkg-config --static --libs glfw3`
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
$(BUILD_DIR)/%.o: $(YAML_CPP_DIR)/src/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/core/command/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/render/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/core/io/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/nitro/impl/%.cpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/nsmb/impl/%.cpp | dirs 
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
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/windows/popups/%.hpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/ui/widgets/%.hpp | dirs 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/plugins/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/blueshell/render/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: all
	./$(EXE)

clean:
	rm -rf $(BUILD_DIR)
