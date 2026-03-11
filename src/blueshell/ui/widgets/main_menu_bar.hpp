#pragma once
#include "imgui.h"
#include "blueshell/editor/editor_manager.h"
#include "blueshell/ui/util/glfw_util.h"
#include "blueshell/ui/windows/visibility_state.h"

namespace BS::UI
{
    void DrawMainMenuBar(GLFWwindow* main_win_ctx, BS::UI::VisibilityState &vis_state, BS::EditorManager& editor_mgr)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {    
                if(ImGui::MenuItem("Open Binary File..."))
                    editor_mgr.LoadFileNextAvail("data/jyotyu_ncl.bin");

                if(ImGui::MenuItem("Save All Binary Files"))
                    editor_mgr.SaveAllFiles();
                
                ImGui::Separator();
                // boilerplate to get me motivated to make a proper program
                ImGui::MenuItem("Open Area from File Pair...");
                ImGui::Separator();
                ImGui::MenuItem("Settings...");
                ImGui::Separator();
                if (ImGui::MenuItem("Exit"))
                    // this shouldn't be how we exit. there needs to be some sort of "TryExit" func that checks if we can, then does
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