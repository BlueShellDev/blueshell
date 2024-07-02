//std
#include <thread>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// app
#include "editor.hpp"
#include "util/util.hpp"
#include "util/lz.hpp"

#include "nitro/rom.hpp"
#include "game/level.h"

// ui
#include "ui/object_image.hpp"
#include "ui/widgets/level_render_widget.h"
#include "ui/widgets/object_picker_widget.h"

static Editor::Application      app;

static UI::AreaRenderWidget     area_view;
static UI::ObjectPickerWidget   general_picker;
static UI::ObjectPickerWidget   stage_picker;
static UI::ObjectPickerWidget   goal_picker;

static NSMB::LevelSource        level_source;

static NSMB::Area               loaded_area;

static IO::ByteArray            overlay_0_raw;
static IO::ByteArray            overlay_10_raw;
static IO::ByteArray            nsbtx_raw;
static IO::ByteArray            fnt_raw;

static Nitro::Directory         nitro_root;

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

struct WorldEntry
{
    std::string name;
    std::vector<NSMB::LevelSource> levels;
};

struct TilesetEntry
{
    u32 id;
    std::string name;
    bool unused;
};

// NSBTX HELPER FUNCS // 
struct NitroHeader
{
    char identifier[4];
    u32 magic;
    u32 file_size;
    u16 header_size;
    u16 num_blocks;
};

struct TEX0Block
{
    char identifier[4];
    u32 size;
    u32 padding_a;
    u16 tex_data_size;
    u16 tex_info_offset;
    u32 padding_b;
    u32 tex_data_offset;
    u32 padding_c;
    u16 cmp_tex_data_size;
    u16 cmp_tex_info_offset;
    u32 padding_d;
    u32 cmp_tex_data_offset;
    u32 cmp_text_info_data_offset;
    u32 padding_e;
    u32 pal_data_size;
    u32 pal_info_offset;
    u32 pal_data_offset;
};

struct PaletteHeader
{
    u16 pal_offset;
    u16 unk;
};

struct TextureHeader
{
    u16     tex_offset;
    u8      format;
    u8      width;
    u8      height;
    bool    color_0;
    u32     unused;

    void unpack(IO::ByteArray& from, sz_t& at)
    {
        tex_offset = IO::read<u16>(from, at) * 8;
        u16 packed_param = IO::read<u16>(from, at);
        format = (packed_param >> 10) & 0b111;
        width = 0b1000 << ((packed_param >> 4) & 0b111);
        height = 0b1000 << ((packed_param >> 7) & 0b111);
        color_0 = !((packed_param >> 13) & 0b001);
        unused = IO::read<u32>(from, at);
    }
};

struct TextureData
{
    IO::ByteArray pixel_data;
};

struct NitroSubHeader
{
    u8  dummy;
    u8  num_objects;
    u16 header_size;

    u16 unk_section_header_size;
    u16 unk_section_size;
    u32 unk_constant;
    std::vector<u32> unk_entries;

    u16 data_info_entry_size;
    u16 data_info_section_size;
    std::vector<TextureHeader> data_info;

    std::vector<std::string> object_names;

    void unpack(IO::ByteArray& from, sz_t& at)
    {
        dummy = IO::read<u8>(from, at);
        num_objects = IO::read<u8>(from, at);
        header_size = IO::read<u16>(from, at);

        unk_section_header_size = IO::read<u16>(from, at);
        unk_section_size = IO::read<u16>(from, at);
        unk_constant = IO::read<u32>(from, at);

        for (sz_t i = 0; i < num_objects; i++)
        {
            u32 unk_data = IO::read<u32>(from, at);
            unk_entries.push_back(unk_data);
        }

        data_info_entry_size = IO::read<u16>(from, at);
        data_info_section_size = IO::read<u16>(from, at);

        for (sz_t i = 0; i < num_objects; i++)
        {
            TextureHeader data;
            data.unpack(from, at);
            data_info.push_back(data);
        }

        for (sz_t i = 0; i < num_objects; i++)
        {
            std::string name;
            for (sz_t j = 0; j < 0x10; j++)
            {
                name += IO::read<char>(from, at);
            }
            object_names.push_back(name);
        }
    }
};

struct TileNSBTXTableEntry
{
    u32 i_file_id;
    u32 tex_id;
    u16 pal_id;
    //two bytes padding!
};

static std::vector<TilesetEntry> tileset_entries;
static nlohmann::json names;

static std::vector<WorldEntry> worlds;

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

// loader functions

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
        app.base_path + app.ncg_dir + app.t0_ncg_path,
        app.base_path + app.ncl_dir + jyotyu_ncl_path,
        app.base_path + app.pnl_dir + app.t0_pnl_path,
        app.base_path + app.unt_dir + app.t0_unt_path
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
        app.base_path + app.ncg_dir + app.t2_ncg_path,
        app.base_path + app.ncl_dir + app.t2_ncl_path,
        app.base_path + app.pnl_dir + app.t2_pnl_path,
        app.base_path + app.unt_dir + app.t2_unt_path
    );
}

void insert_area_tileset(NSMB::Area& area, Nitro::Directory root)
{
    NSMB::Tileset& tileset_1 = area.tilesets.at(1);

    // get table offsets for overlay 0
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
    std::filesystem::path ncl_path = sys_path_from_file_id(app.base_path, root, ncl_id);
    std::filesystem::path ncg_path = sys_path_from_file_id(app.base_path, root, ncg_id);
    std::filesystem::path pnl_path = sys_path_from_file_id(app.base_path, root, pnl_id);
    std::filesystem::path unt_path = sys_path_from_file_id(app.base_path, root, unt_id);

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

int main()
{
    Editor::initialize_main_window(&app);
    Editor::initialize_renderer(&app);

    // setup root directory
    std::filesystem::recursive_directory_iterator root(app.base_path);
    std::ifstream fstream;

    // parse override map
    try
    {
        fstream = Util::begin_ifstream(app.override_json);
        app.render_manager.overrides_json = nlohmann::json::parse(fstream);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
    }

    // parse tileset name map
    try
    {
        fstream = Util::begin_ifstream(app.tileset_json);
        names = nlohmann::json::parse(fstream);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
    }

    for (const auto& e : names["tilesets"])
    {
        TilesetEntry ent;
        ent.id = e["id"].get<int>();
        ent.name = e["name"].get<std::string>();
        ent.unused = e["unused"].get<bool>();
        tileset_entries.push_back(ent);
    }

    // parse worlds name map
    fstream = Util::begin_ifstream(app.worlds_json);
    nlohmann::json worlds_json = nlohmann::json::parse(fstream);

    for (const auto& world : worlds_json["worlds"])
    {
        WorldEntry w_entry;
        const std::string w_letter = world["letter"].get<std::string>();
        const std::string w_name = world["name"].get<std::string>();
        w_entry.name = w_name;
        for (const auto& level : world["levels"])
        {
            NSMB::LevelSource l_entry;
            const std::string l_id = level["id"].get<std::string>();
            const std::string l_name = level["name"].get<std::string>();
            const int l_area_count = level["areas"].get<int>();
            l_entry.name = w_name + l_name;
            for (int i = 0; i < l_area_count; i++)
            {
                NSMB::AreaSource a_entry;
                const std::string a_base_fn = w_letter + l_id + "_" + std::to_string(i + 1);
                a_entry.area_id = i;
                a_entry.header_fn = app.base_path + app.course_path + a_base_fn + ".bin";
                a_entry.bgdat_fn = app.base_path + app.course_path + a_base_fn + "_bgdat.bin";

                l_entry.areas.push_back(a_entry);
            }
            w_entry.levels.push_back(l_entry);
        }
        worlds.push_back(w_entry);
    }

    // load certain files for testing
    try
    {
        fstream = Util::begin_ifstream(app.ov0_path);
        overlay_0_raw = IO::bytes_from_file(fstream);

        fstream = Util::begin_ifstream(app.ov10_path);
        overlay_10_raw = IO::bytes_from_file(fstream);

        fstream = Util::begin_ifstream(app.fnt_path);
        fnt_raw = IO::bytes_from_file(fstream);

        fstream = Util::begin_ifstream(app.base_path + app.nsbtx_dir + app.nsbtx_path);
        nsbtx_raw = IO::bytes_from_file(fstream);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
    }

    fstream.close();

    // load filesystem
    nitro_root = Nitro::parse_directory_filename_table(fnt_raw, 0xF000);
    nitro_root.name = "root";
    fnt_raw.clear();

    // tex block testing
    NitroHeader head;
    TEX0Block tex_block;

    NitroSubHeader s;

    sz_t at = 0x50;
    s.unpack(nsbtx_raw, at);

    std::memcpy(&tex_block, &nsbtx_raw[20], sizeof(TEX0Block));

    std::vector<std::pair<u8, TileNSBTXTableEntry>> textured_tile_table;

    try
    {
        // load nsbtx table
        sz_t ov0_table_offset = 0x2F60C;
        sz_t ov10_table_offset = 0x55BE0;
        sz_t ov10_at = 0;
        for (sz_t i = 0; i < 144; i++)
        {
            u8 offset = IO::read<u8>(overlay_0_raw, ov0_table_offset);
            ov10_at = ov10_table_offset + (offset * sizeof(TileNSBTXTableEntry));
            TileNSBTXTableEntry e;
            e.i_file_id = IO::read<u32>(overlay_10_raw, ov10_at);
            e.tex_id = IO::read<u32>(overlay_10_raw, ov10_at);
            e.pal_id = IO::read<u16>(overlay_10_raw, ov10_at);
            textured_tile_table.push_back({ i + 112, e });
        }
    }
    catch (const std::exception &e)
    {
        Debug::log(e.what(), Debug::LogType_Trace);
        return EXIT_FAILURE;
    }
  
    general_picker.tileset = &loaded_area.tilesets.at(0);
    stage_picker.tileset = &loaded_area.tilesets.at(1);
    goal_picker.tileset = &loaded_area.tilesets.at(2);

    // load W1-4, area 1
    level_source = worlds.at(4).levels.at(0);
    NSMB::AreaSource source = level_source.areas.at(0);

    // load tilesets
    try
    {
        loaded_area = NSMB::load_area(source);
        insert_global_tilesets(loaded_area, nitro_root);
        insert_area_tileset(loaded_area, nitro_root);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
    }

    // init ui elements
    area_view.area = &loaded_area;
    populate_object_pickers(loaded_area);
    render_area_tilesets(loaded_area);

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

        // menu bar setup
        bool open_level_listing = false;
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
        if (ImGui::BeginMenu("Level"))
        {
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
        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::MenuItem("About BlueShell"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

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
                            level_source = level;

                            // load W1-1, area 1
                            NSMB::AreaSource source = level_source.areas.at(0);

                            loaded_area = NSMB::load_area(source);
                            insert_global_tilesets(loaded_area, nitro_root);
                            insert_area_tileset(loaded_area, nitro_root);
                            area_view._clear_selected_items();
                            populate_object_pickers(loaded_area);
                            render_area_tilesets(loaded_area);

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
        ImGui::Begin(std::format("{}###level_editor", level_source.name).c_str());
        area_view.draw(&app.render_manager, { 512, 256 });
        ImGui::End();

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