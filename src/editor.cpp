#include "editor.h"

namespace Editor
{
    void Application::load_names_from_env()
    {
        // load level name tables
        const nlohmann::json& json = env.jsons[ApplicationJSON_Names].second;

        for (const auto& world : json["worlds"])
        {
            Editor::WorldEntry w_entry;
            const std::string w_letter = world["letter"].get<std::string>();
            const std::string w_name = world["name"].get<std::string>();

            w_entry.name = w_name;

            for (const auto& level : world["levels"])
            {
                Editor::LevelSource l_entry;
                const std::string l_id = level["id"].get<std::string>();
                const std::string l_name = level["name"].get<std::string>();
                const int l_area_count = level["areas"].get<int>();

                l_entry.name = w_name + l_name;

                for (int i = 0; i < l_area_count; i++)
                {
                    Editor::AreaSource a_entry;
                    const std::string a_base_fn = w_letter + l_id + "_" + std::to_string(i + 1);

                    a_entry.area_id = i;
                    a_entry.header_fn = a_base_fn + ".bin";
                    a_entry.bgdat_fn = a_base_fn + "_bgdat.bin";
                    l_entry.areas.push_back(a_entry);
                }
                w_entry.levels.push_back(l_entry);
            }
            level_names.push_back(w_entry);
        }

        // and tileset name tables
        for (const auto& tileset : json["tilesets"])
        {
            Editor::TilesetEntry t_entry;

            t_entry.id = tileset["id"].get<int>();
            t_entry.name = tileset["name"].get<std::string>();
            t_entry.unused = tileset["unused"].get<bool>();
            tileset_names.push_back(t_entry);
        }
    }

    void Application::load_overlays()
    {
        std::ifstream in;
        in = Util::begin_ifstream
        (
            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Overlays] /
            "overlay9_0.bin"
        );
        overlay_0_raw = IO::bytes_from_file(in);
        in = Util::begin_ifstream
        (
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Overlays] /
            "overlay9_10.bin"
        );
        overlay_10_raw = IO::bytes_from_file(in);
        in.close();

        if (overlay_0_raw.size() < 203680 || overlay_10_raw.size() < 380736)
            throw (std::runtime_error("i'm gonna go ahead and guess you forgot to decompress overlay 0 and 10...")); 
    }

    void Application::load_filenames(const u16& root_dir_id)
    {
        std::ifstream in;
        IO::ByteArray raw;
        in = Util::begin_ifstream
        (
            env.paths[EnvironmentPaths_Dump] /
            "fnt.bin"
        );
        raw = IO::bytes_from_file(in);
        in.close();

        // parse the raw fnt into a clean structure
        nitro_root = Nitro::parse_directory_filename_table(raw, root_dir_id);
        nitro_root.name = "root";
        raw.clear();
    }
    
    void Application::change_current_area(const sz_t& new_area_index)
    {
        if (new_area_index >= curr_level_source.areas.size())
            throw (std::out_of_range(std::format
            (
                "refusing to load area {}/{}",
                new_area_index + 1,
                curr_level_source.areas.size()
            )));
        
        // parse the area files
        const Editor::AreaSource& source = curr_level_source.areas.at(new_area_index);
        curr_area = NSMB::load_area_from_disk
        (
            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Stages] / 
            source.header_fn,

            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Stages] / 
            source.bgdat_fn,
            
            new_area_index
        );

        // load tilesets
        NSMB::Tileset& tileset_0 = curr_area.tilesets.at(0);
        NSMB::Tileset& tileset_1 = curr_area.tilesets.at(1);
        NSMB::Tileset& tileset_2 = curr_area.tilesets.at(2);

        NSMB::AreaJyotyuType_ jytotyu_type = NSMB::get_jyotyu_type
        (
            curr_area, 
            overlay_0_raw, 
            (sz_t)Nitro::OV0TableOffsets_JY_NCL // region specific value!
        );

        std::string palette_file;
        switch (jytotyu_type)
        {
        case NSMB::AreaJyotyuType_Normal:
            palette_file = "d_2d_A_J_jyotyu_ncl.bin";
            break;
        case NSMB::AreaJyotyuType_B:
            palette_file = "d_2d_A_J_jyotyu_B_ncl.bin";
            break;
        case NSMB::AreaJyotyuType_R:
            palette_file = "d_2d_A_J_jyotyu_R_ncl.bin";
            break;
        case NSMB::AreaJyotyuType_W:
            palette_file = "d_2d_A_J_jyotyu_W_ncl.bin";
            break;
        }

        // load tileset 1
        // get table offsets for overlays 0
        sz_t addr_ncl = (sz_t)Nitro::OV0TableOffsets_TS_NCL + (curr_area.main_tileset_table_offset * 4);
        sz_t addr_ncg = (sz_t)Nitro::OV0TableOffsets_TS_NCG + (curr_area.main_tileset_table_offset * 4);
        sz_t addr_pnl = (sz_t)Nitro::OV0TableOffsets_TS_PNL + (curr_area.main_tileset_table_offset * 4);
        sz_t addr_unt = (sz_t)Nitro::OV0TableOffsets_TS_UNT + (curr_area.main_tileset_table_offset * 4);

        // use offsets to read file ids
        u32 ncl_id = (IO::read<u32>(overlay_0_raw, addr_ncl) & 0xffff) + Nitro::OV0TableOffsets_OverlayCount;
        u32 ncg_id = (IO::read<u32>(overlay_0_raw, addr_ncg) & 0xffff) + Nitro::OV0TableOffsets_OverlayCount;
        u32 pnl_id = (IO::read<u32>(overlay_0_raw, addr_pnl) & 0xffff) + Nitro::OV0TableOffsets_OverlayCount;
        u32 unt_id = (IO::read<u32>(overlay_0_raw, addr_unt) & 0xffff) + Nitro::OV0TableOffsets_OverlayCount;

        // load sys paths from file ids
        std::filesystem::path ncl_path = Nitro::sys_path_from_file_id
        (
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Root],
            nitro_root, 
            ncl_id
        );
        std::filesystem::path ncg_path = Nitro::sys_path_from_file_id
        (
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Root],
            nitro_root,
            ncg_id
        );
        std::filesystem::path pnl_path = Nitro::sys_path_from_file_id
        (
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Root],
            nitro_root,
            pnl_id
        );
        std::filesystem::path unt_path = Nitro::sys_path_from_file_id
        (
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Root],
            nitro_root, 
            unt_id
        );

        // load tileset 0
        tileset_0 = NSMB::load_tileset_from_disk
        (
            NSMB::TilesetType_Jyotyu,

            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Characters] / 
            "d_2d_A_J_jyotyu_ncg.bin",

            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Palettes] / 
            palette_file,

            env.paths[EnvironmentPaths_Dump] / 
            env.paths[EnvironmentPaths_Map16] / 
            "d_2d_PA_A_J_jyotyu.bin",
            
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Objects] 
            / "A_J_jyotyu.bin"
        );

        // init tileset0 overrides vector
        for (auto& json_entry : env.jsons[ApplicationJSON_Overrides].second["override-lookup"])
        {
            OverrideEntry entry;
            entry.overrides = json_entry["overrides"].get<sz_t>();
            entry.x_offset = json_entry["x-offset"].get<sz_t>();
            if (!json_entry["y-offset"].is_null())
                entry.y_offset = json_entry["y-offset"].get<sz_t>();
            render_manager.overrides_table.push_back(entry);
        }
        tileset_0.use_overrides = true;

        // load_tileset 1
        tileset_1 = NSMB::load_tileset_from_disk
        (
            NSMB::TilesetType_Main,
            ncg_path,
            ncl_path,
            pnl_path,
            unt_path
        );

        // load tileset 2
        tileset_2 = NSMB::load_tileset_from_disk
        (
            NSMB::TilesetType_Sub,

            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Characters] /
            "d_2d_I_S_tikei_nohara_ncg.bin",

            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Palettes] / 
            "d_2d_I_S_tikei_nohara_ncl.bin",

            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Map16] / 
            "d_2d_PA_I_S_nohara.bin",
            
            env.paths[EnvironmentPaths_Dump] /
            env.paths[EnvironmentPaths_Objects] / 
            "I_S_nohara.bin"
        );

        // re-render tilesets
        for (sz_t i = 0; i < 3; i++)
        {
            Render::render_tileset
            (
                &render_manager, 
                curr_area.tilesets.at(i)
            );
        }
    }

    void init_render_mgr
    (
        Render::RenderManager& manager, 
        const sz_t& num_textures
    )
    {
        // why is this variable name so long
        manager.number_of_textures_to_gen = num_textures;
        Render::initialize_opengl_textures(&manager);
        Render::initialize_pixmap_table(&manager);

        // load override images as textures
        Render::load_image_to_texture
        (
            &manager,
            &manager.overrides_texture,
            "res/images/overrides.png"
        );

        // load null character
        IO::ByteArray null_data;
        null_data.assign(64, 0);
        sz_t addr = 0;
        Graphics::Character null_c;
        null_c.unpack(null_data, addr);
        manager.null_character = null_c;
    }

    void set_editor_style()
    {
        // docking
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

        // shaping
        auto& style = ImGui::GetStyle();
        style.TabRounding = 4;
        style.ScrollbarRounding = 4;
        style.WindowRounding = 4;
        style.GrabRounding = 4;
        style.FrameRounding = 4;
        style.PopupRounding = 4;
        style.ChildRounding = 4;
        style.WindowBorderSize = 0;
        style.FrameBorderSize = 0;
        style.TabBarBorderSize = 1;
        style.FramePadding = { 6, 4 };

        // colors
        ImGui::StyleColorsDark();
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.13f, 0.93f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.10f, 0.10f, 0.13f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(0.44f, 0.37f, 0.61f, 0.64f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.33f, 0.28f, 0.47f, 0.64f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.50f, 0.42f, 0.69f, 0.64f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.32f, 0.27f, 0.44f, 0.64f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.19f, 0.20f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.24f, 0.24f, 0.32f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.44f, 0.37f, 0.61f, 0.54f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.74f, 0.58f, 0.98f, 0.54f);
        colors[ImGuiCol_Button]                 = ImVec4(0.30f, 0.26f, 0.43f, 0.94f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.60f, 0.50f, 0.83f, 0.94f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.60f, 0.45f, 0.99f, 0.94f);
        colors[ImGuiCol_Header]                 = ImVec4(0.44f, 0.37f, 0.61f, 0.29f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.50f, 0.42f, 0.69f, 0.29f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.45f, 0.75f, 0.29f);
        colors[ImGuiCol_Separator]              = ImVec4(0.44f, 0.37f, 0.61f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.84f, 0.58f, 1.00f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.44f, 0.37f, 0.61f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.74f, 0.58f, 0.98f, 0.29f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.84f, 0.58f, 1.00f, 0.29f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.30f, 0.26f, 0.43f, 0.64f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.60f, 0.50f, 0.83f, 0.64f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.60f, 0.45f, 0.99f, 0.64f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.19f, 0.16f, 0.26f, 0.64f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.60f, 0.45f, 0.99f, 0.64f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.44f, 0.37f, 0.61f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}
