#include "editor.h"

namespace Editor
{
    void ApplicationUI::init_main_window
    (
        const std::string& title, 
        const int& width,
        const int& height,
        const bool& maximize
    )
    {
        // basic boilerplate GLFW shit, just create a window and make sure it's there
        if (!glfwInit())
            throw std::runtime_error("failed to initialize GLFW!");

        // oh yeah, we need to check if we're running on mac and configure shaders accordingly for imgui
        // (copied right from the imgui demo thing)
#if defined(__APPLE__)
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

        // get window ptr and make sure it points to something
        main_window = glfwCreateWindow
        (
            width,
            height,
            title.c_str(),
            nullptr,
            nullptr
        );
        if (!main_window)
        {
            glfwTerminate();
            throw std::runtime_error("failed to create the main window!");
        }

        // maximize it if set
        if (maximize)
            glfwMaximizeWindow(main_window);

        // set window icon
        GLFWimage image;
        image.pixels = stbi_load("res/icon16.png", &image.width, &image.height, 0, 4);
        glfwSetWindowIcon(main_window, 1, &image);
        stbi_image_free(image.pixels);

        // create opengl and imgui contexts on window
        glfwMakeContextCurrent(main_window);
        ImGui::CreateContext();

        // init imgui opengl things
        ImGui_ImplGlfw_InitForOpenGL(main_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void ApplicationUI::new_frame()
    {
        glClearColor(0.4f, 0.5f, 0.6f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ApplicationUI::init_dock_space()
    {
        // configure dock space
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, { 0, 0 });
        static bool startup = true;
        if (startup)
        {
            startup = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.0375f, nullptr, &dockspace_id);
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.1375f, nullptr, &dockspace_id);
            auto dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.33f, nullptr, &dock_id_left);
            //auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 1.0f, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("AreaEditorWindow", dockspace_id);
            ImGui::DockBuilderDockWindow("ToolboxWindow", dock_id_left);
            ImGui::DockBuilderDockWindow("SelectionWindow", dock_id_left_down);
            ImGui::DockBuilderDockWindow("StatusWindow", dock_id_down);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        ImGui::End();
    }

    void ApplicationUI::relink_widgets_to_area(NSMB::Area* area)
    {
        for (sz_t i = 0; i < 3; i++)
        {
            object_picker_widgets.at(i).tileset = &area->tilesets.at(i);
            object_picker_widgets.at(i).populate();
        }
        area_editor.area = area;
    }

    void ApplicationUI::draw_fix_env_popup(Environment& env)
    {
        // shown when the user needs to fix their environment variables due to a invalid result at boot
        if (ImGui::BeginPopupModal("Fix Environment Variables###fix_env_popup", 0, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (!open_fix_env_popup)
                ImGui::CloseCurrentPopup();

            ImGui::Text("BlueShell has detected one or more invalid system paths stored in its settings.\nPlease choose a valid 'New! Super Mario Bros.' dump folder.");
            ImGui::NewLine();
            ImGui::TextColored
            (
                { 0.85f, 0.85f, 1.0f, 1.0f }, 
                "Current dump folder:"
            );
            ImGui::SameLine();
            ImGui::TextColored
            (
                { 0.85f, 0.85f, 1.0f, 1.0f }, 
                "'%s'", (env.fs).string().c_str()
            );
            if (dir_browser.HasSelected())
            {
                ImGui::TextColored
                (
                    { 0.85f, 85.0f, 1.0f, 1.0f }, 
                    "Selected dump folder:"
                );
                ImGui::SameLine();
                ImGui::TextColored
                (
                    { 0.85f, 85.0f, 1.0f, 1.0f }, 
                    "'%s'", 
                    (dir_browser.GetSelected()).string().c_str()
                );
            }
            ImGui::Separator();
            if (ImGui::Button("Browse for a dump folder..."))
                dir_browser.Open();
            ImGui::SameLine();
            if (dir_browser.HasSelected())
            {
                if (ImGui::ButtonEx("Next..."))
                {
                    env.fs = dir_browser.GetSelected();
                    if (!env.validate_paths())
                    {
                        env.copy_paths_from_json();
                        ImGui::OpenPopup("###invalid-selection");
                    }
                    else
                    {
                        env.copy_paths_to_json();
                        ImGui::OpenPopup("###valid-selection");
                    }
                }
            }
            if (ImGui::BeginPopupModal("Invalid Selection###invalid-selection", 0, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("'%s' does not appear to be a valid directory. Please select a valid game dump.", dir_browser.GetSelected().string().c_str());
                ImGui::Separator();
                if (ImGui::Button("OK"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopupModal("Selection Saved###valid-selection", 0, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("'%s' will be used as the default dump folder.", dir_browser.GetSelected().string().c_str());
                ImGui::Separator();
                if (ImGui::Button("OK"))
                {
                    open_fix_env_popup = false;
                    open_level_select_popup = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            dir_browser.Display();
            ImGui::EndPopup();
        }
    }

    void ApplicationUI::draw_level_select_popup(Application& app)
    {
        // displays a list of all levels in the game organized by name
        // in a tree view. items are selectable and occur a change in area if "area_editor.area" is not null and is not unsaved.
        if (ImGui::BeginPopupModal("Select a Level to Open###level_select_popup", 0, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BeginChild("level_listing_child", {275, 500}, ImGuiChildFlags_Border);
            for (auto& world : app.level_names)
            {
                if (ImGui::TreeNodeEx(world.name.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    for (auto& level : world.levels)
                    {
                        if (ImGui::Selectable(("> " + level.name).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                        {
                            app.curr_level_source = level;
                            app.change_current_area(0);
                            relink_widgets_to_area(&app.curr_area);
                            open_level_select_popup = false;
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::Separator();
                    ImGui::NewLine();
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
            ImGui::EndPopup();
        }
    }

    void ApplicationUI::draw_main_menu_bar()
    {
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Level by Name...", "Ctrl+O"))
                open_level_select_popup = true;
            ImGui::Separator();
            if (ImGui::MenuItem("Save Level", "Ctrl+S"))
            {

            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Copy", "Ctrl+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V"))
            {
            }
            if (ImGui::MenuItem("Cut", "Ctrl+X/Del"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Show Objects", "Ctrl+Alt+O", &area_editor.show_objects);
            ImGui::MenuItem("Show Sprites", "Ctrl+Alt+A", &area_editor.show_actors);
            ImGui::MenuItem("Show Entrances", "Ctrl+Alt+E", &area_editor.show_entrances);
            ImGui::MenuItem("Show Views/Zones", "Ctrl+Alt+V", &area_editor.show_views);
            ImGui::MenuItem("Show Paths", "Ctrl+Alt+P", &area_editor.show_paths);
            ImGui::Separator();
            if (ImGui::MenuItem("Scale +25%", "Ctrl+="))
                area_editor.scale(area_editor.draw_env.scale += 0.25);
            if (ImGui::MenuItem("Reset Scale", "Ctrl+0"))
                area_editor.scale(1.0f);
            if (ImGui::MenuItem("Scale -25%", "Ctrl+-"))
                area_editor.scale(area_editor.draw_env.scale -= 0.25);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::ColorEdit4("Background color: ", &area_editor.background_color.x);
            ImGui::PushItemWidth(300.f);
            ImGui::ShowFontSelector("Font");
            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    void ApplicationUI::draw_selection_window()
    {
        if (area_editor.selection.empty())
            return;

        u16 step = 1;
        // only display properties for the first item in a multi-selection
        UI::AreaRenderItem* selection = area_editor.selection.at(0);

        ImGui::Begin("SelectionWindow");
        ImGui::BeginChild("##properties_child", { 0, 0 }, ImGuiChildFlags_Border);
        if (ImGui::CollapsingHeader("Item", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("##item_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Item ID");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::Text(std::format("{}", selection->item_id).c_str());
                ImGui::PopItemWidth();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Item Type");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                switch (selection->item_type)
                {
                case UI::AreaItemType_BgDat:
                    ImGui::Text("Object");
                    break;
                case UI::AreaItemType_StageObject:
                    ImGui::Text("Sprite");
                    break;
                case UI::AreaItemType_Entrance:
                    ImGui::Text("Entrance");
                    break;
                case UI::AreaItemType_View:
                    ImGui::Text("View");
                    break;
                case UI::AreaItemType_Zone:
                    ImGui::Text("Zone");
                    break;
                }
                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            if (ImGui::BeginTable("##transform_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Position");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::DragScalar("##x", ImGuiDataType_U16, &selection->transform.x, 0.25f);
                ImGui::SameLine();
                ImGui::DragScalar("##y", ImGuiDataType_U16, &selection->transform.y, 0.25f);
                ImGui::PopItemWidth();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Size");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::DragScalar("##w", ImGuiDataType_U16, &selection->transform.w, 0.25f);
                ImGui::SameLine();
                ImGui::DragScalar("##h", ImGuiDataType_U16, &selection->transform.h, 0.25f);
                ImGui::PopItemWidth();

                ImGui::EndTable();
            }
        if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
            if (ImGui::BeginTable("##parameters_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                for (auto& property : selection->properties)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(property.display_name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    ImGui::InputScalar(("##" + property.display_name).c_str(), property.type, &property.value, &step);
                }
                ImGui::EndTable();
            }

        ImGui::EndChild();
        ImGui::End();
    }

    // // // // 

    void Application::load_names_from_json()
    {
        // load level name tables
        for (const auto& world : env.worlds_json["worlds"])
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
        for (const auto& tileset : env.tilesets_json["tilesets"])
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
        in = Util::begin_ifstream(env.fs / env.overlays / "overlay9_0.bin");
        overlay_0_raw = IO::bytes_from_file(in);
        in = Util::begin_ifstream(env.fs / env.overlays / "overlay9_10.bin");
        overlay_10_raw = IO::bytes_from_file(in);
        if (overlay_0_raw.size() < 203680 || overlay_10_raw.size() < 380736)
            throw (std::runtime_error("i'm gonna go ahead and guess you forgot to decompress 0 and 10..."));
 
        in.close();
    }

    void Application::load_filename_lookup(const u16& root_dir_id)
    {
        std::ifstream in;
        IO::ByteArray raw;
        in = Util::begin_ifstream(env.fs / "fnt.bin");
        raw = IO::bytes_from_file(in);
        in.close();

        // parse the raw fnt into a clean structure
        nitro_root = Nitro::parse_directory_filename_table(raw, root_dir_id);
        nitro_root.name = "root";
        raw.clear();
    }

    void Application::change_current_area(const sz_t& new_area_index)
    {
        if (new_area_index > curr_level_source.areas.size() - 1)
            return;
        
        // parse the area files
        const Editor::AreaSource& source = curr_level_source.areas.at(new_area_index);
        curr_area = NSMB::load_area_from_disk
        (
            env.fs / env.root / env.stages / source.header_fn,
            env.fs / env.root / env.stages / source.bgdat_fn,
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
        std::filesystem::path ncl_path = Nitro::sys_path_from_file_id(env.fs / env.root, nitro_root, ncl_id);
        std::filesystem::path ncg_path = Nitro::sys_path_from_file_id(env.fs / env.root, nitro_root, ncg_id);
        std::filesystem::path pnl_path = Nitro::sys_path_from_file_id(env.fs / env.root, nitro_root, pnl_id);
        std::filesystem::path unt_path = Nitro::sys_path_from_file_id(env.fs / env.root, nitro_root, unt_id);

        // load tileset 0
        tileset_0 = NSMB::load_tileset_from_disk
        (
            NSMB::TilesetType_Jyotyu,
            env.fs / env.root / env.characters / "d_2d_A_J_jyotyu_ncg.bin",
            env.fs / env.root / env.palettes / palette_file,
            env.fs / env.root / env.metatiles / "d_2d_PA_A_J_jyotyu.bin",
            env.fs / env.root / env.objects / "A_J_jyotyu.bin"
        );

        // init tileset0 overrides vector
        tileset_0.overrides.assign(tileset_0.metatiles.size(), -1);
        for (auto& item : render_manager.overrides_json.items())
        {
            tileset_0.overrides.at(std::stoi(item.key())) = item.value();
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
            env.fs / env.root / env.characters / "d_2d_I_S_tikei_nohara_ncg.bin",
            env.fs / env.root / env.palettes / "d_2d_I_S_tikei_nohara_ncl.bin",
            env.fs / env.root / env.metatiles / "d_2d_PA_I_S_nohara.bin",
            env.fs / env.root / env.objects / "I_S_nohara.bin"
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
            "res/block_override.png"
        );
        Render::load_image_to_texture
        (
            &manager,
            &manager.pipes_texture,
            "res/pipe_override.png"
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

    void Environment::copy_paths_from_json()
    {
        fs          = env_json["sys-paths"]["rom_fs"].get<std::string>();
        root        = env_json["dirs"]["root"].get<std::string>(),
        overlays    = env_json["dirs"]["overlay"].get<std::string>(),
        stages      = env_json["subdirs"]["stages"].get<std::string>(),
        characters  = env_json["subdirs"]["graphics"]["characters"].get<std::string>(),
        palettes    = env_json["subdirs"]["graphics"]["palettes"].get<std::string>(),
        metatiles   = env_json["subdirs"]["graphics"]["metatiles"].get<std::string>(),
        objects     = env_json["subdirs"]["graphics"]["objects"].get<std::string>(),
        textures    = env_json["subdirs"]["graphics"]["textures"].get<std::string>();
    }

    bool Environment::validate_paths()
    {
        // check validity as well and throw an exception if anything doesn't exist
        return
        !(
            !std::filesystem::exists(fs) ||
            !std::filesystem::exists(fs / root) ||
            !std::filesystem::exists(fs / overlays) ||
            !std::filesystem::exists(fs / root / stages) ||
            !std::filesystem::exists(fs / root / characters) ||
            !std::filesystem::exists(fs / root / palettes) ||
            !std::filesystem::exists(fs / root / metatiles) ||
            !std::filesystem::exists(fs / root / objects) ||
            !std::filesystem::exists(fs / root / textures)
        );
    }

    void Environment::copy_paths_to_json()
    {
        env_json["sys-paths"]["rom_fs"]                 = fs;
        env_json["dirs"]["root"]                        = root;
        env_json["dirs"]["overlay"]                     = overlays;
        env_json["subdirs"]["stages"]                   = stages;
        env_json["subdirs"]["graphics"]["characters"]   = characters;
        env_json["subdirs"]["graphics"]["palettes"]     = palettes;
        env_json["subdirs"]["graphics"]["metatiles"]    = metatiles;
        env_json["subdirs"]["graphics"]["objects"]      = objects;
        env_json["subdirs"]["graphics"]["textures"]     = textures;
    }
}
