#pragma once

#include "imgui_internal.h"
#include "editor/editor_manager.h"
#include "plugins/plugin_host_api.h"
#include "plugins/plugin_manager.h"
#include "ui/util/glfw_util.h"
#include "ui/util/imgui_util.h"
#include "game/game.h"

namespace BS
{
    struct MainContext
    {
        GLFWwindow* glfwCtx;
        ImGuiContext* guiCtx;

        EditorManager editorMgr;

        // plugin stuffs
        Plugins::PluginManager pluginManager;
        Plugins::EditorStateProvider *editorStateProvider;
        Plugins::LevelEditSink *levelEditSink;

        inline MainContext()
        {
            glfwCtx             = nullptr;
            guiCtx              = nullptr;

            editorStateProvider = nullptr;
            levelEditSink       = nullptr;
        };
    };

    MainContext*                CreateEmptyContext();
    bool                        InitContext(MainContext* ctx, const std::string &win_tile, const int &win_w, const int &win_h);
    void                        DestroyContext(MainContext* ctx);
    void                        RunUI(MainContext* ctx);
};
