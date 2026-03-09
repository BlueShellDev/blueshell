#pragma once
#include "imgui.h"
#include "blueshell/ui/util/glfw_util.h"
#include "blueshell/ui/windows/visibility_state.h"

namespace BS::UI
{
    void DrawMainMenuBar(GLFWwindow* main_win_ctx, BS::UI::VisibilityState &vis_state)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    glfwSetWindowShouldClose(main_win_ctx, true);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Show Console", nullptr, vis_state.ShowConsole))
                    vis_state.ShowConsole= !vis_state.ShowConsole;

                if (ImGui::MenuItem("Show Texture Viewer", nullptr, vis_state.ShowTextureViewer))
                    vis_state.ShowTextureViewer = !vis_state.ShowTextureViewer;

                ImGui::Separator();
                if (ImGui::MenuItem("Show Plugin Manager", nullptr, vis_state.ShowPluginMgr))
                    vis_state.ShowPluginMgr = !vis_state.ShowPluginMgr;

                ImGui::Separator();
                if (ImGui::MenuItem("Show Debug Window", nullptr, vis_state.ShowDebug))
                    vis_state.ShowDebug = !vis_state.ShowDebug;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About BlueShell..."))
                    vis_state.ShowAboutPopup = true;

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}