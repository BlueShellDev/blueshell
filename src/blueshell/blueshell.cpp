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
    ImGuiIO& io                             = ctx->guiCtx->IO;
    io.ConfigFlags                         |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly    = true;
    ImGui::GetStyle().WindowRounding        = 5.0f;
    ImGui::GetStyle().FrameRounding         = 5.0f;
    ImGui::GetStyle().ChildRounding         = 5.0f;
    ImGui::GetStyle().CellPadding           = ImVec2(6, 6);
    ImGui::StyleColorsClassic();

    // so this gives us a bare name table of what files are recognized in the rom
    // if we also have a file browser that shows the extracted rom on your local FS, we could highlight
    // which files are NOT recognized by the nitrorom, letting the user know they need to be inserted
    Nitro::FS::FileNameTable rom_fns = Nitro::FS::ReadFileNameTableFromBytes(Core::ReadByteArrayFromFile("data/fnt.bin"), 0xF000);
    Nitro::ExPalette pal = Nitro::ReadExPaletteFromBytes(Core::ReadByteArrayFromFile("data/jyotyu_ncl.bin"));

    double last_frame_time = glfwGetTime();
    while (!glfwWindowShouldClose(ctx->glfwCtx))
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
        BS::UI::DrawMainMenuBar(ctx->glfwCtx, window_visibility);

        if (window_visibility.ShowConsole)
            BS::UI::ShowConsoleWindow("Console",                        &window_visibility.ShowConsole);
        if (window_visibility.ShowTextureViewer)
            BS::UI::ShowTexturesWindow("Textures",                      &window_visibility.ShowTextureViewer);
        if (window_visibility.ShowDebug)
            BS::UI::ShowDebugWindow("Debug",                            &window_visibility.ShowDebug, ctx);
        if (window_visibility.ShowPluginMgr)
            BS::Plugins::UI::ShowPluginManagerWindow("Plugin Manager",  &window_visibility.ShowPluginMgr,
                                                                        &ctx->pluginManager);
        if (window_visibility.ShowFileNameTableViewer)
            BS::UI::ShowFileNameTableViewer("Filenames in ROM",         &window_visibility.ShowFileNameTableViewer,
                                                                        rom_fns);
        if (window_visibility.ShowAboutPopup)
            BS::UI::ShowAboutPopup("About BlueShell",                   &window_visibility.ShowAboutPopup);
    

        ctx->pluginManager.TickAll(delta_seconds);
        BS::UI::ClearGLFWWindowWithColor(ctx->glfwCtx, 0.2f, 0.2f, 0.2f, 1.0f);
        BS::UI::EndImGuiFrame();
        glfwSwapBuffers(ctx->glfwCtx);
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

    ctx->glfwCtx = BS::UI::MakeGLFWWindow(win_title.c_str(), win_w, win_h);
    if (ctx->glfwCtx == nullptr)
    {
        Core::Console::Global().Err("Failed to create a GLFW window!");
        glfwTerminate();
        return false;
    }

    ctx->guiCtx = BS::UI::BeginImGui(ctx->glfwCtx, BS::UI::GetGLSLVersion());
    if (ctx->guiCtx == nullptr)
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
    BS::UI::EndImGui(ctx->guiCtx);
    glfwDestroyWindow(ctx->glfwCtx);
    glfwTerminate();
    delete ctx;
}
