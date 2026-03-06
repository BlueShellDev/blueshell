#pragma once
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace BS::UI
{
    ImGuiContext* BeginImGui(GLFWwindow *mainWindow, std::string glslVersion);
    void BeginImGuiFrame();
    void InitializeImGuiDockspace(std::string dockspaceID);
    void EndImGuiFrame();
    void EndImGui(ImGuiContext* imgui_ctx);
}