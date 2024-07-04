//std
#include <thread>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// app
#include "editor.h"
#include "sources.h"
#include "util/util.hpp"
#include "util/lz.hpp"

#include "nitro/nsbtx.h"
#include "nitro/rom.hpp"
#include "nsmb/area/area.h"

// ui
#include "ui/object_image.hpp"
#include "ui/widgets/level_render_widget.h"
#include "ui/widgets/object_picker_widget.h"

#include "imfilebrowser.h"

Editor::Application      app;
Editor::LevelSource      level_source;

ImGui::FileBrowser dir_browser = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
ImGui::FileBrowser file_browser = ImGui::FileBrowser();

UI::AreaRenderWidget     area_view;
UI::ObjectPickerWidget   general_picker;
UI::ObjectPickerWidget   stage_picker;
UI::ObjectPickerWidget   goal_picker;

NSMB::Area               loaded_area;

IO::ByteArray            overlay_0_raw;
IO::ByteArray            overlay_10_raw;
IO::ByteArray            nsbtx_raw;
IO::ByteArray            fnt_raw;

Nitro::Directory         nitro_root;

std::vector<Editor::TilesetEntry> tileset_entries;
std::vector<Editor::WorldEntry> worlds;

bool is_env_set = true;
bool open_level_listing = false;

// FILESYSTEM HELPER FUNCS //
std::filesystem::path sys_path_from_file_id
(
    const std::filesystem::path& sys_root,
    const Nitro::Directory&      rom_root,
    const u16&                   file_id
)
{
    const std::filesystem::recursive_directory_iterator sys_root_it(sys_root);
    
    std::filesystem::path found_sys_path;
    std::string rom_filename = "";

    if (Nitro::search_dir(rom_root, file_id, rom_filename))
    {
        for (auto& p : sys_root_it)
        {
            if (!p.is_directory())
            {
                const std::filesystem::path& sys_path = p.path();
                if (rom_filename == sys_path.filename().string())
                {
                    found_sys_path = sys_path;
                }
            }
        }
    }

    return found_sys_path;
}

void draw_fs_tree(const Nitro::Directory& dir)
{
    if (ImGui::TreeNodeEx(dir.name.c_str()))
    {
        for (auto& dir : dir.dirs)
            draw_fs_tree(dir);

        for (auto& file : dir.files)
            if (ImGui::TreeNodeEx(file.name.c_str(), ImGuiTreeNodeFlags_Leaf))
                ImGui::TreePop();

        ImGui::TreePop();
    }
}

void insert_global_tilesets(NSMB::Area& area, Nitro::Directory root)
{
    NSMB::Tileset& tileset_0 = area.tilesets.at(0);
    NSMB::Tileset& tileset_2 = area.tilesets.at(2);
    
    sz_t jyotyu_ncl_offset = 0x30CD8 + area.bgnscid;
    u8 jyotyu_ncl_id = IO::read<u8>(overlay_0_raw, jyotyu_ncl_offset);

    std::string jyotyu_ncl_path;
    if (jyotyu_ncl_id == 1)
        jyotyu_ncl_path = "d_2d_A_J_jyotyu_B_ncl.bin";
    else if (jyotyu_ncl_id == 2)
        jyotyu_ncl_path = "d_2d_A_J_jyotyu_R_ncl.bin";
    else if (jyotyu_ncl_id == 3)
        jyotyu_ncl_path = "d_2d_A_J_jyotyu_W_ncl.bin";
    else
        jyotyu_ncl_path = "d_2d_A_J_jyotyu_ncl.bin";

    // load tileset 0
    tileset_0 = NSMB::load_tileset
    (
        NSMB::TilesetType_Jyotyu,
        app.env.fs / app.env.root / app.env.characters / "d_2d_A_J_jyotyu_ncg.bin",
        app.env.fs / app.env.root / app.env.palettes / jyotyu_ncl_path,
        app.env.fs / app.env.root / app.env.metatiles / "d_2d_PA_A_J_jyotyu.bin",
        app.env.fs / app.env.root / app.env.objects / "A_J_jyotyu.bin"
    );

    // init tileset0 overrides vector
    tileset_0.overrides.assign(tileset_0.metatiles.size(), -1);

    // load overrides
    for (auto& item : app.render_manager.overrides_json.items())
    {
        tileset_0.overrides.at(std::stoi(item.key())) = item.value();
    }
    tileset_0.use_overrides = true;

    // load tileset 2
    tileset_2 = NSMB::load_tileset
    (
        NSMB::TilesetType_Sub,
        app.env.fs / app.env.root / app.env.characters / "d_2d_I_S_tikei_nohara_ncg.bin",
        app.env.fs / app.env.root / app.env.palettes / "d_2d_I_S_tikei_nohara_ncl.bin",
        app.env.fs / app.env.root / app.env.metatiles / "d_2d_PA_I_S_nohara.bin",
        app.env.fs / app.env.root / app.env.objects / "I_S_nohara.bin"
    );
}

void insert_area_tileset(NSMB::Area& area, Nitro::Directory root)
{
    NSMB::Tileset& tileset_1 = area.tilesets.at(1);

    // get table offsets for overlays 0
    sz_t addr_ncl = 0x31494 + (area.main_tileset_table_offset * 4);
    sz_t addr_ncg = 0x30ea4 + (area.main_tileset_table_offset * 4);
    sz_t addr_pnl = 0x316f4 + (area.main_tileset_table_offset * 4);
    sz_t addr_unt = 0x2fa14 + (area.main_tileset_table_offset * 4);

    // use offsets to read file ids
    u32 ncl_id = (IO::read<u32>(overlay_0_raw, addr_ncl) & 0xffff) + 131; // this offset is different for different regions!
    u32 ncg_id = (IO::read<u32>(overlay_0_raw, addr_ncg) & 0xffff) + 131;
    u32 pnl_id = (IO::read<u32>(overlay_0_raw, addr_pnl) & 0xffff) + 131;
    u32 unt_id = (IO::read<u32>(overlay_0_raw, addr_unt) & 0xffff) + 131;

    // load sys paths from file ids
    std::filesystem::path ncl_path = sys_path_from_file_id(app.env.fs / app.env.root, root, ncl_id);
    std::filesystem::path ncg_path = sys_path_from_file_id(app.env.fs / app.env.root, root, ncg_id);
    std::filesystem::path pnl_path = sys_path_from_file_id(app.env.fs / app.env.root, root, pnl_id);
    std::filesystem::path unt_path = sys_path_from_file_id(app.env.fs / app.env.root, root, unt_id);

    // load each tileset 1 file
    std::ifstream in = Util::begin_ifstream(ncl_path.string());
    IO::ByteArray ncl_raw = IO::bytes_from_file(in);

    in = Util::begin_ifstream(ncg_path.string());
    IO::ByteArray ncg_raw = IO::bytes_from_file(in);

    in = Util::begin_ifstream(pnl_path.string());
    IO::ByteArray pnl_raw = IO::bytes_from_file(in);

    in = Util::begin_ifstream(unt_path.string());
    IO::ByteArray unt_raw = IO::bytes_from_file(in);

    // finally can load tileset 1
    tileset_1 = NSMB::load_tileset
    (
        NSMB::TilesetType_Main,
        ncl_raw,
        ncg_raw,
        pnl_raw,
        unt_raw
    );
}

void populate_object_pickers(NSMB::Area& area)
{
    general_picker.populate();
    stage_picker.populate();
    goal_picker.populate();
}

void render_area_tilesets(NSMB::Area& area)
{
    // load null character
    IO::ByteArray null_pix_dat;
    null_pix_dat.assign(64, 0);
    sz_t addr = 0;
    Graphics::Character c;
    c.unpack(null_pix_dat, addr);
    app.render_manager.null_character;

    // render tilesets to render manager
    Render::render_tileset(&app.render_manager, area.tilesets.at(0));
    Render::render_tileset(&app.render_manager, area.tilesets.at(1));
    Render::render_tileset(&app.render_manager, area.tilesets.at(2));
}

// these are not final methods, just a grouping rn
void load_reqs()
{
    std::ifstream fstream;
    Debug::log("reading overlay 0 from disk...", Debug::LogType_Verbose);
    fstream = Util::begin_ifstream(app.env.fs / app.env.overlays / "overlay9_0.bin");
    overlay_0_raw = IO::bytes_from_file(fstream);

    Debug::log("reading overlay 0 from disk...", Debug::LogType_Verbose);
    fstream = Util::begin_ifstream(app.env.fs / app.env.overlays / "overlay9_10.bin");
    overlay_10_raw = IO::bytes_from_file(fstream);

    Debug::log("reading filename table from disk...", Debug::LogType_Verbose);
    fstream = Util::begin_ifstream(app.env.fs / "fnt.bin");
    fnt_raw = IO::bytes_from_file(fstream);

    Debug::log("reading nsbtx file from disk...", Debug::LogType_Verbose);
    fstream = Util::begin_ifstream(app.env.fs / app.env.root / app.env.textures / "I_dokan.nsbtx");
    nsbtx_raw = IO::bytes_from_file(fstream);
    fstream.close();
}

//
void load_fs()
{
    // load fs
    Debug::log("parsing fnt...", Debug::LogType_Verbose);
    nitro_root = Nitro::parse_directory_filename_table(fnt_raw, 0xF000);
    nitro_root.name = "root";
    fnt_raw.clear();
}

// 
void load_level(const Editor::LevelSource& lvl_src, const sz_t& area_id)
{
    level_source = lvl_src;
    Editor::AreaSource source = level_source.areas.at(area_id);
    loaded_area = NSMB::load_area(app.env, source);

    Debug::log("loading tilesets 0 and 2 from disk...", Debug::LogType_Verbose);
    insert_global_tilesets(loaded_area, nitro_root);

    Debug::log("loading tileset 1 from disk...", Debug::LogType_Verbose);
    insert_area_tileset(loaded_area, nitro_root);
}

void init_ui()
{
    // init ui elements
    area_view.area = &loaded_area;
    general_picker.tileset = &loaded_area.tilesets.at(0);
    stage_picker.tileset = &loaded_area.tilesets.at(1);
    goal_picker.tileset = &loaded_area.tilesets.at(2);
    populate_object_pickers(loaded_area);
}

int main()
{
    try
    {
        Debug::log("initializing main window...", Debug::LogType_Verbose);
        Editor::initialize_main_window(&app);
        Debug::log("initializing render manager... ", Debug::LogType_Verbose);
        Editor::initialize_renderer(&app);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }

    Editor::initialize_imgui_style();

    // load editor files
    std::ifstream fstream;
    Debug::log("reading jsons from disk...", Debug::LogType_Verbose);
    try
    {
        fstream = Util::begin_ifstream("res/enviroment.json");
        app.env.env_json = nlohmann::json::parse(fstream);

        fstream = Util::begin_ifstream("res/overrides.json");
        app.render_manager.overrides_json = nlohmann::json::parse(fstream);

        fstream = Util::begin_ifstream("res/tilesets.json");
        app.env.tilesets_json = nlohmann::json::parse(fstream);

        fstream = Util::begin_ifstream("res/worlds.json");
        app.env.worlds_json = nlohmann::json::parse(fstream);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }
    fstream.close();

    // populate entries from jsons
    for (const auto& tileset : app.env.tilesets_json["tilesets"])
    {
        Editor::TilesetEntry ent;
        ent.id = tileset["id"].get<int>();
        ent.name = tileset["name"].get<std::string>();
        ent.unused = tileset["unused"].get<bool>();
        tileset_entries.push_back(ent);
    }
    for (const auto& world : app.env.worlds_json["worlds"])
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
        worlds.push_back(w_entry);
    }

    // load paths from json file to env variables
    Debug::log("loading eviroment variables from 'enviroment.json'...", Debug::LogType_Verbose);
    app.env.load_paths_from_json();

    // if base path is empty or invalid we can't continue
    if (!std::filesystem::exists(app.env.fs))
    {
        Debug::log
        (
            "base path was not specified or was invalid \nplease check 'rom-fs' field in res/enviroment.json:\n'" +
            app.env.fs.string() + "' is not a valid file path"
        );

        is_env_set = false;
        //return EXIT_FAILURE;
    }

    if (is_env_set)
    {
        // read non-level essential files from
        try
        {
            load_reqs();
            load_fs();
        }
        catch (const std::runtime_error& e)
        {
            Debug::log(e.what());
            return EXIT_FAILURE;
        }

        // set level source for load
        level_source = worlds.at(4).levels.at(0);

        // load W1-4, area 1
        Debug::log("reading '" + level_source.name + "' area files from disk... ", Debug::LogType_Verbose);
        try
        {
            load_level(level_source, 0);
            init_ui();
            render_area_tilesets(loaded_area);
        }
        catch (const std::runtime_error& e)
        {
            Debug::log(e.what());
            return EXIT_FAILURE;
        }
    }

    Debug::log("entering main loop", Debug::LogType_Verbose);

    // render loop
    while (!glfwWindowShouldClose(app.main_window))
    {
        glClearColor(0.4f, 0.5f, 0.6f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        // begin imgui context
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        Render::bind_buffers_to_gpu(&app.render_manager);

        if (!is_env_set)
            ImGui::OpenPopup("###setup_env");

        if (ImGui::BeginPopupModal("Setup Enviroment Variables###setup_env"))
        {
            ImGui::TextWrapped("Please provide an extracted 'New! Super Mario Bros' ROM at the top-most level\n(e.g <extracted_folder>/FILESYSTEM)");
            if (ImGui::Button("Browse"))
            {
                dir_browser.Open();
            }
            ImGui::SameLine();;
            if (dir_browser.HasSelected())
            {
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
                ImGui::SameLine();
                if (ImGui::Button("OK"))
                {
                    app.env.fs = dir_browser.GetSelected();
                    // check more than if just root exists ;p
                    if (!std::filesystem::exists(app.env.fs / app.env.root))
                    {
                        ImGui::OpenPopup("###invalid_path");
                    }
                    else
                    {
                        load_reqs();
                        load_fs();
                        is_env_set = true;
                        app.env.save_paths_to_json();
                        Debug::log("writing enviroment variables to json...");
                    
                        std::ofstream fstream = Util::begin_ofstream("res/enviroment.json");
                        fstream << std::setw(4) << app.env.env_json;
                        fstream.close();

                        open_level_listing = true;
                        ImGui::CloseCurrentPopup();
                    }
                }

                if (ImGui::BeginPopupModal("Invalid Path###invalid_path"))
                {
                    ImGui::Text("The selected directory does not contain valid files.");
                    if (ImGui::Button("Re-select directory..."))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::Separator();
                ImGui::TextWrapped(("selected: " + dir_browser.GetSelected().string()).c_str());
            }
            dir_browser.Display();
            ImGui::EndPopup();
        }

        if (open_level_listing)
            ImGui::OpenPopup("###level_listing");

        if (ImGui::BeginPopupModal("Select a Level to Open###level_listing"))
        {
            ImGui::BeginChild("##listing-child", { 0, 0 }, ImGuiChildFlags_Border);
            for (auto& world : worlds)
            {
                if (ImGui::TreeNodeEx(world.name.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    for (auto& level : world.levels)
                    {
                        if (ImGui::Selectable(("> " + level.name).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                        {
                            load_level(level, 0);
                            init_ui();
                            area_view._clear_selected_items();
                            render_area_tilesets(loaded_area);

                            open_level_listing = false;
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

        // menu bar setup
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Level by Name...", "Ctrl+O"))
            {
                open_level_listing = true;
            }

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
            ImGui::MenuItem("Show Objects", "Ctrl+Alt+O", &area_view.show_objects);
            ImGui::MenuItem("Show Sprites", "Ctrl+Alt+S", &area_view.show_sprites);
            ImGui::MenuItem("Show Entrances", "Ctrl+Alt+E", &area_view.show_entrances);
            ImGui::MenuItem("Show Views", "Ctrl+Alt+V", &area_view.show_views);
            ImGui::MenuItem("Show Zones", "Ctrl+Alt+Z", &area_view.show_zones);
            ImGui::MenuItem("Show Paths", "Ctrl+Alt+P", &area_view.show_paths);

            ImGui::Separator();

            if (ImGui::MenuItem("Zoom In", "Ctrl+="))
            {
            }

            if (ImGui::MenuItem("Zoom 1x", "Ctrl+0"))
            {
            }

            if (ImGui::MenuItem("Zoom Out", "Ctrl+-"))
            {
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::ColorEdit4("Background color: ", &area_view.background_color.x);
            ImGui::PushItemWidth(300.f);
            ImGui::ShowFontSelector("Font");
            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        // toolbox setup
        ImGui::Begin("Toolbox");
        if (ImGui::BeginTabBar("Toolbox Tabs"))
        {      
            if (ImGui::BeginTabItem("Objects"))
            {
                if (ImGui::BeginTabBar("Tileset Tabs"))
                {
                    if (ImGui::BeginTabItem("General"))
                    {
                        area_view.active_picker = &general_picker;
                        general_picker.draw(&app.render_manager, { 8, 48 });
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Stage"))
                    {
                        area_view.active_picker = &stage_picker;
                        stage_picker.draw(&app.render_manager, { 8, 48 });
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Goal"))
                    {
                        area_view.active_picker = &goal_picker;
                        goal_picker.draw(&app.render_manager, { 8, 48 });
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Sprites"))
            {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Entrances"))
            {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Views/Zones"))
            {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Paths"))
            {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        // areas window setup
        if (area_view.area)
        {
            ImGui::Begin(std::format("{}###level_editor", level_source.name).c_str());
            area_view.draw(&app.render_manager, { 512, 256 });
            ImGui::End();
        }

        // loaded_area settigns table
        ImGui::Begin("Level Settings");
        ImGui::BeginChild("##settings_child", { 0, 0 }, ImGuiChildFlags_Border);
        if (ImGui::BeginTable("##settings_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Parameter");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Time Limit");
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(ImGui::GetColumnWidth());
            ImGui::InputScalar("", ImGuiDataType_U16, &loaded_area.time_limit);
            ImGui::PopItemWidth();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Start Entrance ID");
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(ImGui::GetColumnWidth());
            ImGui::InputScalar("", ImGuiDataType_U8, &loaded_area.start_entrance_id);
            ImGui::PopItemWidth();
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Checkpoint Entrance ID");
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(ImGui::GetColumnWidth());
            ImGui::InputScalar("", ImGuiDataType_U8, &loaded_area.midway_entrance_id);
            ImGui::PopItemWidth();
            
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::End();

        //// nsbtx table
        //ImGui::Begin("3D Tile -> NSBTX Table");
        //for (auto& e : textured_tile_table)
        //{
        //    std::string fn;
        //    Nitro::search_dir(nitro_root, ((e.second.i_file_id & 0xFFFF) + 131), fn);
        //    ImGui::Text(std::format("Tile {}: | Texture File: {} | Texture ID: {} | Palette ID: {}", e.first, fn.c_str(), e.second.tex_id, e.second.pal_id).c_str());
        //}
        //ImGui::End();

        // loaded_area view info 
        ImGui::Begin("Status", 0, ImGuiWindowFlags_NoTitleBar);
        ImGui::Text(std::format("Zoom: {}x", area_view.draw_env.scale).c_str());
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::Text(std::format("Area {} of {} (PgUp/PgDown)", loaded_area.source.area_id + 1, level_source.areas.size()).c_str());
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::Text(std::format("{} FPS", (int)ImGui::GetIO().Framerate).c_str());
        ImGui::End();
        
        // selection table
        if (!area_view.selection.empty())
        {
            u16 step = 1;
            UI::AreaRenderItem* e = area_view.selection.at(0);
            ImGui::Begin("Selection", 0, ImGuiWindowFlags_Modal);
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
                    ImGui::Text(std::format("{}", e->item_id).c_str());
                    ImGui::PopItemWidth();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Item Type");
                    ImGui::TableNextColumn();
                    ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                    switch (e->item_type)
                    {
                    case UI::AreaItemType::AreaItemType_BgDat:
                        ImGui::Text("Object");
                        break;
                    case UI::AreaItemType::AreaItemType_StageObject:
                        ImGui::Text("Sprite");
                        break;
                    case UI::AreaItemType::AreaItemType_Entrance:
                        ImGui::Text("Entrance");
                        break;
                    case UI::AreaItemType::AreaItemType_View:
                        ImGui::Text("View");
                        break;
                    case UI::AreaItemType::AreaItemType_Zone:
                        ImGui::Text("Zone");
                        break;
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndTable();
                }
            }
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
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
                    ImGui::DragScalar("##x", ImGuiDataType_U16, &e->transform.x, 0.25f);
                    ImGui::SameLine();
                    ImGui::DragScalar("##y", ImGuiDataType_U16, &e->transform.y, 0.25f);
                    ImGui::PopItemWidth();

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Size");
                    ImGui::TableNextColumn();
                    ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                    ImGui::DragScalar("##w", ImGuiDataType_U16, &e->transform.w, 0.25f);
                    ImGui::SameLine();
                    ImGui::DragScalar("##h", ImGuiDataType_U16, &e->transform.h, 0.25f);
                    ImGui::PopItemWidth();

                    ImGui::EndTable();
                }
            }
            if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginTable("##parameters_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
                {
                    ImGui::TableSetupColumn("Parameter");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableHeadersRow();

                    for (auto& property : e->properties)
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
            }
            ImGui::EndChild();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // end imgui context

        glfwSwapBuffers(app.main_window);
        glfwPollEvents();
    }

    glfwTerminate();
    Debug::log("program ended sucessfully");

    return 0;
}