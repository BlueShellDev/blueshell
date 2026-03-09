#include "blueshell.h"

#include "core/io/binary_file.h"
#include "core/io/byte_array.h"

#include "plugins/plugin_manager_ui.hpp"

#include "nitro/simple_nitro.h"

#include "ui/windows/visibility_state.h"
#include "ui/windows/popups/all_popups.h"
#include "ui/windows/all_windows.h"
#include "ui/widgets/main_menu_bar.hpp"

bool Core::ByteArray::LogReadsAndWrites = false;

void BS::RunUI(MainContext *ctx)
{
    // lets configure the ui
    ImGuiIO& io                             = ctx->imgui_ctx->IO;
    io.ConfigFlags                         |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly    = true;
    ImGui::GetStyle().WindowRounding        = 5.0f;
    ImGui::GetStyle().FrameRounding         = 5.0f;
    ImGui::GetStyle().ChildRounding         = 5.0f;
    ImGui::GetStyle().CellPadding           = ImVec2(6, 6);
    ImGui::StyleColorsClassic();

    Nitro::ExPalette pal    = Nitro::ReadExPaletteFromBytes(Core::ReadByteArrayFromFile("bad_filename"));
    pal                     = Nitro::ReadExPaletteFromBytes(Core::ReadByteArrayFromFile("data/jyotyu_ncl.bin"));

    double last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(ctx->window))
    {
        // 
        const double frame_time = glfwGetTime();
        const double delta_seconds = frame_time - last_frame_time;
        last_frame_time = frame_time;

        // 
        glfwPollEvents();
        BS::UI::BeginImGuiFrame();
        BS::UI::InitializeImGuiDockspace("bs_dockspace");

        static BS::UI::VisibilityState window_visibility;
        BS::UI::DrawMainMenuBar(ctx->window, window_visibility);

        if (window_visibility.ShowConsole)
            BS::UI::ShowConsoleWindow("Console",                        &window_visibility.ShowConsole);
        if (window_visibility.ShowTextureViewer)
            BS::UI::ShowTexturesWindow("Textures",                      &window_visibility.ShowTextureViewer);
        if (window_visibility.ShowDebug)
            BS::UI::ShowDebugWindow("Debug",                            &window_visibility.ShowDebug, ctx);
        if (window_visibility.ShowPluginMgr)
            BS::Plugins::UI::ShowPluginManagerWindow("Plugin Manager",  &window_visibility.ShowPluginMgr,
                                                                        &ctx->pluginManager);
        if (window_visibility.ShowAboutPopup)
            BS::UI::ShowAboutPopup("About BlueShell",                   &window_visibility.ShowAboutPopup);

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
