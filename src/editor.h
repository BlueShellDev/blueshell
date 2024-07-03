#pragma once

// opengl
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// imgui
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// json
#include "nlohmann/json.hpp"

// internal
#include "graphics/render.h"
#include "util/util.hpp"

namespace Editor
{
    struct Enviroment
    {
        std::string font_path = "res/super-mario-ds-original.ttf",
                    pipe_override_path = "res/pipe_override_2.png",
                    jyotyu_override_path = "res/block_override.png",
                    override_json = "res/overrides.json",
                    tileset_json = "res/names.json",
                    worlds_json = "res/worlds.json",

                    ov0_path = "D:/Documents/NDS Games/FILESYSTEM/overlay9/overlay9_0.bin",
                    ov10_path = "D:/Documents/NDS Games/FILESYSTEM/overlay9/overlay9_10.bin",
                    fnt_path = "D:/Documents/NDS Games/FILESYSTEM/fnt.bin",

                    base_path = "D:/Documents/NDS Games/FILESYSTEM/root/",

                    course_path = "course/",
                    ncg_dir = "BG_ncg/",
                    ncl_dir = "BG_ncl/",
                    pnl_dir = "BG_pnl/",
                    unt_dir = "BG_unt/",
                    nsbtx_dir = "polygon_unit/",

                    t0_ncg_path = "d_2d_A_J_jyotyu_ncg.bin",
                    t0_pnl_path = "d_2d_PA_A_J_jyotyu.bin",
                    t0_unt_path = "A_J_jyotyu.bin",

                    t2_ncg_path = "d_2d_I_S_tikei_nohara_ncg.bin",
                    t2_ncl_path = "d_2d_I_S_tikei_nohara_ncl.bin",
                    t2_pnl_path = "d_2d_PA_I_S_nohara.bin",
                    t2_unt_path = "I_S_nohara.bin",

                    nsbtx_path = "I_dokan.nsbtx";

        nlohmann::json json;
    };

    struct Application
    {
        /* program constants */
        const std::string main_window_title = "BlueShell [NSMB Level Editor] 0.2.2a";
        const int default_main_window_width = 1920; // px
        const int default_main_window_height = 1080; // px

        /* program paths */ 
        Enviroment enviroment;

        /* links to main app services */
        GLFWwindow* main_window = nullptr;

        /* handles pixbufs, lives here */
        Render::RenderManager render_manager;
    };

    void initialize_main_window(Application* app);
    void initialize_renderer(Application* app);
    void initialize_imgui_style();
}