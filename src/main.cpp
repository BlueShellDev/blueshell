// oh boy this is the main file. it's gonna be a mess for a while until i get some of the structure in place and can start moving things out to other files. sorry about that!
#include <iostream>
#include <string>
#include <filesystem>

#include "core/util/glfw_util.h"
#include "core/util/imgui_util.h"
#include "imfilebrowser.h"

#include "nsmb/simple_nsmb.h"

#include "blueshell/ui/windows/console_viewer.h"
#include "blueshell/ui/windows/textures_viewer.h"
#include "core/console.h"

bool Core::ByteArray::LogReadsAndWrites = false;

int main(int, char **)
{
    if (!glfwInit())
    {
        Core::Console::Global().Err("Failed to initialize GLFW!");
        return EXIT_FAILURE;
    }
    Core::Console::Global().Log("Initialized GLFW successfully.");

    std::string glsl_version = Core::GetGLSLVersion();
    if (glsl_version.empty())
    {
        Core::Console::Global().Err("Failed to determine GLSL version string!");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    Core::Console::Global().Log("Using GLSL: {}", glsl_version);

    GLFWwindow *window = Core::MakeGLFWWindow("BlueShell", 1920, 1080);
    if (window == nullptr)
    {
        Core::Console::Global().Err("Failed to create main GLFW window!");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    Core::BeginImGui(window, glsl_version, true);
    ImGui::StyleColorsClassic();
    ImGui::GetStyle().WindowRounding = 5.0f;
    ImGui::GetStyle().FrameRounding = 5.0f;
    ImGui::GetStyle().ChildRounding = 5.0f;
    ImGui::GetStyle().CellPadding = ImVec2(6, 6);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Core::BeginImGuiFrame();
        Core::InitializeImGuiDockspace("MainDockspace");

        static bool showConsole = true;
        static bool showTextureViewer = true;
        static bool showTilesetProperties = true;
        static bool showAboutPopup = false;
        if (showConsole)
            BS::UI::ShowConsoleWindow(showConsole);
        if (showTextureViewer)
            BS::UI::ShowTexturesWindow(showTextureViewer);
        if (showAboutPopup)
            ImGui::OpenPopup("About BlueShell");

        if(ImGui::Button("Log warning"))
            Core::Console::Global().Warn("warn");
        if(ImGui::Button("Log err"))
            Core::Console::Global().Err("err");
        if(ImGui::Button("Log normal"))
            Core::Console::Global().Log("test");

        if (ImGui::BeginPopupModal("About BlueShell", &showAboutPopup, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BeginChild("##about_child", ImVec2(400, 300), true);
            ImGui::PushFont(ImGui::GetFont(), 26.0f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "BlueShell (early 2026)\n");
            ImGui::PopFont();
            ImGui::PushFont(ImGui::GetFont(), 18.0f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "A New! 'New! Super Mario Bros.' Editor\n");
            ImGui::PopFont();
            ImGui::SeparatorText("What is this?");
            ImGui::TextWrapped("BlueShell is a level editor for New Super Mario Bros. on the Nintendo DS. It is currently in early development and is not yet feature complete. \n\nThe goal of this project is to create a plugin-centric and powerful level editor that allows users to create and edit levels for NSMB with ease. \n\nIf you have any questions, suggestions, or want to contribute to the project, please visit the GitHub repository linked below.");
            ImGui::SeparatorText("Links");
            ImGui::BulletText("GitHub Repository: ");
            ImGui::SameLine();
            ImGui::TextLinkOpenURL("BlueShell", "https://github.com/BlueShellDev/blueshell");
            ImGui::EndChild();
            ImGui::EndPopup();
        }


        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    glfwSetWindowShouldClose(window, true);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Show Console", nullptr, showConsole))
                    showConsole = !showConsole;
                if (ImGui::MenuItem("Show Texture Viewer", nullptr, showTextureViewer))
                    showTextureViewer = !showTextureViewer;
                if (ImGui::MenuItem("Show Tileset Properties", nullptr, showTilesetProperties))
                    showTilesetProperties = !showTilesetProperties;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About BlueShell..."))
                    showAboutPopup = true;

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        Core::ClearGLFWWindowWithColor(window, 0.2f, 0.2f, 0.2f, 1.0f);
        Core::EndImGuiFrame();
        glfwSwapBuffers(window);
    }
    // end imgui context after loop
    Core::EndImGui();
    // terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}