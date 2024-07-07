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
#include "util/lz.hpp"

// ui
#include "ui/widgets/level_render_widget.h"
#include "ui/widgets/object_picker_widget.h"

// nitro
#include "nitro/rom.hpp"

namespace Editor
{
    // enviroment variables
    struct Environment
    {
        std::filesystem::path settings_dir = "settings/";
        std::filesystem::path resources_dir = "res/";
        std::filesystem::path env_file = settings_dir / "environment.json";

        /* jsons, should these live here? kinda confusing */
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

        void copy_paths_from_json();
        bool validate_paths();
        void copy_paths_to_json();
    };

    struct Application
    {
        /* program paths, lives here */
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
        void load_names_from_json();
        void load_overlays();
        void load_filename_lookup(const u16& root_dir_id);
        void change_current_area(const sz_t& new_area_id);
    };

    struct ApplicationUI
    {
        GLFWwindow* main_window = nullptr;

        /* imgui addons */
        ImGui::FileBrowser dir_browser = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
        ImGui::FileBrowser file_browser = ImGui::FileBrowser();

        /* editor-specific ui elements */
        UI::AreaRenderWidget area_editor;
        std::array<UI::ObjectPickerWidget, 3> object_picker_widgets;

        /* ui states */
        bool 
        open_setup_popup = false,
        open_fix_env_popup = false,
        open_level_select_popup = false;

        void init_main_window
        (
            const std::string&  title, 
            const int&          width = 1920, 
            const int&          height = 1080,
            const bool&         maximize = true
        );
        void new_frame();

        void init_dock_space();
        void relink_widgets_to_area(NSMB::Area* area);

        // main ui elements
        void draw_main_menu_bar();
        void draw_selection_window();

        // popup logic
        void draw_fix_env_popup(Environment& env);
        void draw_level_select_popup(Application& app);
    };

    void init_render_mgr
    (
        Render::RenderManager& manager,
        const sz_t&
        num_textures
    );
    void set_editor_style();
}