#include "blueshell.h"
#include "core/io/byte_array.h"
#include "plugins/plugin_manager_ui.hpp"
#include "ui/windows/popups/all_popups.h"
#include "ui/windows/all_windows.h"

bool Core::ByteArray::LogReadsAndWrites = false;

void BS::RunUI(MainContext *ctx)
{
    ImGuiIO& io                             = ctx->imgui_ctx->IO;
    io.ConfigFlags                         |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly    = true;
    ImGui::StyleColorsClassic();
    ImGui::GetStyle().WindowRounding        = 5.0f;
    ImGui::GetStyle().FrameRounding         = 5.0f;
    ImGui::GetStyle().ChildRounding         = 5.0f;
    ImGui::GetStyle().CellPadding           = ImVec2(6, 6);
    double last_frame_time = glfwGetTime();

    while (!glfwWindowShouldClose(ctx->window))
    {
        glfwPollEvents();
        BS::UI::BeginImGuiFrame();
        BS::UI::InitializeImGuiDockspace("bs_dockspace");
        const double frame_time = glfwGetTime();
        const double delta_seconds = frame_time - last_frame_time;
        last_frame_time = frame_time;

        static bool showConsole             = true;
        static bool showTextureViewer       = true;
        static bool showAboutPopup          = false;
        static bool showDebugWindow         = false;
        static bool showPluginManager       = false;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    glfwSetWindowShouldClose(ctx->window, true);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Show Console", nullptr, showConsole))
                    showConsole = !showConsole;

                if (ImGui::MenuItem("Show Texture Viewer", nullptr, showTextureViewer))
                    showTextureViewer = !showTextureViewer;

                ImGui::Separator();
                if (ImGui::MenuItem("Show Debug Window", nullptr, showDebugWindow))
                    showDebugWindow = !showDebugWindow;
                if (ImGui::MenuItem("Show Plugin Manager", nullptr, showPluginManager))
                    showPluginManager = !showPluginManager;

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

        if (showConsole)
            BS::UI::ShowConsoleWindow("Console", &showConsole);
            
        if (showTextureViewer)
            BS::UI::ShowTexturesWindow("Textures", &showTextureViewer);

        if (showAboutPopup)
            BS::UI::ShowAboutPopup("About BlueShell", &showAboutPopup);

        if (showDebugWindow)
            BS::UI::ShowDebugWindow("Debug", &showDebugWindow, ctx);
        if (showPluginManager)
            BS::Plugins::UI::ShowPluginManagerWindow("Plugin Manager", &showPluginManager, &ctx->pluginManager);

        ctx->pluginManager.TickAll(delta_seconds);

        BS::UI::ClearGLFWWindowWithColor(ctx->window, 0.2f, 0.2f, 0.2f, 1.0f);
        BS::UI::EndImGuiFrame();
        glfwSwapBuffers(ctx->window);
    }
}

BS::MainContext *BS::CreateEmptyContext()
{
    return new BS::MainContext();
}

bool BS::InitContext(MainContext *ctx, const std::string &win_title, const int &win_w, const int &win_h)
{
    if (!glfwInit())
    {
        Core::Console::Global().Err("Failed to initialize GLFW!");
        return false;
    }
    Core::Console::Global().Log("Initialized GLFW successfully");
    Core::Console::Global().Log("Using GLSL: {}", BS::UI::GetGLSLVersion());

    ctx->window = BS::UI::MakeGLFWWindow(win_title.c_str(), win_w, win_h);
    if (ctx->window == nullptr)
    {
        Core::Console::Global().Err("Failed to create a GLFW window!");
        glfwTerminate();
        return false;
    }

    ctx->imgui_ctx = BS::UI::BeginImGui(ctx->window, BS::UI::GetGLSLVersion());
    if (ctx->imgui_ctx == nullptr)
    {
        Core::Console::Global().Err("Failed to initialize ImGui!");
        glfwTerminate();
        return false;
    }

    if (!ctx->pluginManager.Initialize("./plugins", ctx->editorStateProvider, ctx->levelEditSink))
        Core::Console::Global().Err("Plugin manager initialization failed; continuing with empty plugin registry");

    return true;
}

void BS::DestroyContext(MainContext *ctx)
{
    ctx->pluginManager.Shutdown();
    BS::UI::EndImGui(ctx->imgui_ctx);
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    delete ctx;
}
