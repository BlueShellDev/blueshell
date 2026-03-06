#pragma once

#include "imgui_internal.h"
#include "plugins/plugin_host_api.h"
#include "plugins/plugin_manager.h"
#include "ui/util/glfw_util.h"
#include "ui/util/imgui_util.h"

namespace BS
{
    struct MainContext
    {
        GLFWwindow* window;
        ImGuiContext* imgui_ctx;
        Plugins::PluginManager pluginManager;
        Plugins::EditorStateProvider *editorStateProvider;
        Plugins::LevelEditSink *levelEditSink;

        MainContext()
        {
            window = nullptr;
            imgui_ctx = nullptr;
            editorStateProvider = nullptr;
            levelEditSink = nullptr;
        };
    };

    void RunUI(MainContext* ctx);

    MainContext* CreateEmptyContext();
    bool InitContext(MainContext* ctx, const std::string &win_tile, const int &win_w, const int &win_h);
    void DestroyContext(MainContext* ctx);
};
