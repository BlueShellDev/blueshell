#pragma once
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Core
{
    void BeginImGui(GLFWwindow *mainWindow, std::string glslVersion, bool supportDocking);
    void BeginImGuiFrame();
    void InitializeImGuiDockspace(std::string dockspaceID);
    void EndImGuiFrame();
    void EndImGui();
}