#pragma once

// std
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// opengl
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// imgui
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// third-party imgui lib
#include "imfilebrowser.h"

// stb
#include "stb_image.h"

// internal
#include "sources.h"
#include "graphics/render.h"
#include "environment.h"
#include "util/lz.hpp"

// ui
#include "ui/widgets/level_render_widget.h"
#include "ui/widgets/object_picker_widget.h"

// nitro
#include "nitro/rom.hpp"

namespace Editor
{
    struct Application
    {
        /* application environment */
        Environment env;

        /* handles pixbufs, lives here */
        Render::RenderManager render_manager;

        /* program meta-data, such as name tables */
        std::vector<WorldEntry> level_names;
        std::vector<TilesetEntry> tileset_names;
        LevelSource curr_level_source;

        /* live program data */
        IO::ByteArray overlay_0_raw;  // overlays have no data structure yet, I need to do this for clean multi-
        IO::ByteArray overlay_10_raw; // region support!

        Nitro::Directory nitro_root;
        NSMB::Area curr_area; // the 3 loaded tilesets live in this structure

        /* methods */
        void load_names_from_env();
        void load_overlays();
        void load_filenames(const u16& root_dir_id);
        void change_current_area(const sz_t& new_area_id);
    };

    // ui layer of application
    struct ApplicationUI
    {
        /* link to main application */
        Application* app = nullptr;

        /* main app window */
        GLFWwindow* main_window = nullptr;

        /* imgui addons */
        ImGui::FileBrowser 
        dir_browser = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory),
        file_browser = ImGui::FileBrowser();

        /* editor-specific ui elements */
        UI::AreaRenderWidget area_editor;
        std::array<UI::ObjectPickerWidget, 3> object_picker_widgets;

        /* ui states */
        bool 
        open_setup_popup = false,
        open_fix_env_popup = false,
        open_level_select_popup = false;

        // boilerplate shit
        void init_main_window
        (
            const std::string&  title, 
            const int&          width = 1920, 
            const int&          height = 1080,
            const bool&         maximize = true
        );
        void new_frame();
        void init_dock_space();

        // main ui elements
        void draw_main_menu_bar();
        void draw_area_editor_window(const ImVec2& content_size_units);
        void draw_toolbox_window();
        void draw_status_window();
        void draw_selection_window();

        // popups
        void draw_fix_env_popup();
        void draw_level_select_popup();

        // interesting function
        void relink_widgets_to_area(NSMB::Area* area);
    };

    void init_render_mgr
    (
        Render::RenderManager& manager,
        const sz_t&
        num_textures
    );

    void set_editor_style();
}