// oh boy this is the main file. it's gonna be a mess for a while until i get some of the structure in place and can start moving things out to other files. sorry about that!
#include <iostream>
#include <string>
#include <filesystem>

#include "core/util/glfw_util.h"
#include "core/util/imgui_util.h"
#include "imfilebrowser.h"

#include "ui/windows/console_viewer.h"
#include "ui/windows/textures_viewer.h"
#include "ui/windows/tileset_definitions_editor.h"

bool BS::IO::ByteArray::LogReadsAndWrites = false;

int main(int, char **)
{
    if (!glfwInit())
    {
        BS::Console::Global().Err("Failed to initialize GLFW!");
        return EXIT_FAILURE;
    }
    BS::Console::Global().Log("Initialized GLFW successfully.");

    std::string glsl_version = BS::Util::GetShaderVersion();
    if (glsl_version.empty())
    {
        BS::Console::Global().Err("Failed to determine GLSL version string!");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    BS::Console::Global().Log("Using GLSL: {}", glsl_version);

    GLFWwindow *window = BS::Util::MakeGLFWWindow("BlueShell", 1920, 1080);
    if (window == nullptr)
    {
        BS::Console::Global().Err("Failed to create main GLFW window!");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    BS::Util::BeginImGui(window, glsl_version, true);
    ImGui::StyleColorsClassic();
    ImGui::GetStyle().WindowRounding = 5.0f;
    ImGui::GetStyle().FrameRounding = 5.0f;
    ImGui::GetStyle().ChildRounding = 5.0f;
    ImGui::GetStyle().CellPadding = ImVec2(6, 6);

    BS::BSContext mainContext{};
    mainContext.basePath = std::filesystem::current_path();
    BS::Console::Global().Log("Set base path to: {}", mainContext.basePath.string());

    mainContext.tilesetTableOffsets = {.entryCount = 76, // eventually load these from a config file or something instead of hardcoding them, but for now this is fine since they shouldn't ever change and if they do change then the editor user probably knows what they're doing and can change these values themselves
                                       .NCGBaseOffset = 0x00030EA4,
                                       .NCLBaseOffset = 0x00031494,
                                       .PNLBaseOffset = 0x000316F4,
                                       .UNTBaseOffset = 0x0002FA14,
                                       .CHKBaseOffset = 0x0002FB44};
    mainContext.tilesetDefinitions = BS::NSMB::ReadTilesetIndexFromOverlay0(mainContext.overlay0, mainContext.tilesetTableOffsets);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        BS::Util::BeginImGuiFrame();
        BS::Util::InitializeImGuiDockspace("MainDockspace");

        static bool showConsole = true;
        static bool showTextureViewer = true;
        static bool showTilesetProperties = true;
        static bool showAboutPopup = false;
        if (showConsole)
            BS::UI::ShowConsoleWindow(showConsole);
        if (showTextureViewer)
            BS::UI::ShowTexturesWindow(showTextureViewer);
        if (showTilesetProperties)
            BS::UI::ShowTilesetDefinitionsEditor(showTilesetProperties, mainContext);
        if (showAboutPopup)
            ImGui::OpenPopup("About BlueShell");

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

        BS::Util::ClearGLFWWindowWithColor(window, 0.2f, 0.2f, 0.2f, 1.0f);
        BS::Util::EndImGuiFrame();
        glfwSwapBuffers(window);
    }
    // end imgui context after loop
    BS::Util::EndImGui();
    // terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}