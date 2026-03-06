#pragma once

#include "imgui_internal.h"
#include "ui/util/glfw_util.h"
#include "ui/util/imgui_util.h"

namespace BS
{
    struct MainContext
    {
        GLFWwindow* window;
        ImGuiContext* imgui_ctx;

        MainContext() {window = nullptr; imgui_ctx = nullptr;};
    };

    void RunUI(MainContext* ctx);

    MainContext* CreateEmptyContext();
    bool InitContext(MainContext* ctx, const std::string &win_tile, const int &win_w, const int &win_h);
    void DestroyContext(MainContext* ctx);
};

