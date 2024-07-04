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
    struct Environment
    {
        /* jsons */
        nlohmann::json overrides_json,
            tilesets_json,
            worlds_json,
            env_json;

        /* paths to-be-loaded from json */
        std::filesystem::path fs,
            root,
            overlays,
            stages,
            characters,
            palettes,
            metatiles,
            objects,
            textures;

        void load_paths_from_json();
        void save_paths_to_json();
    };

    struct Application
    {
        /* program constants */
        const std::string main_window_title = "BlueShell [NSMB Level Editor] 0.2.2a";
        const int default_main_window_width = 1920; // px
        const int default_main_window_height = 1080; // px

        /* program paths */ 
        Environment env;

        /* links to main app services */
        GLFWwindow* main_window = nullptr;

        /* handles pixbufs, lives here */
        Render::RenderManager render_manager;
    };

    void initialize_main_window(Application* app);
    void initialize_renderer(Application* app);
    void initialize_imgui_style();
}