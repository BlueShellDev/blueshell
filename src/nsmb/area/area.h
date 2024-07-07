#pragma once

#include "../../sources.h"
#include "../tileset.h"
#include "components.h"

namespace NSMB
{
    enum AreaJyotyuType_
    {
        AreaJyotyuType_Normal,
        AreaJyotyuType_B,
        AreaJyotyuType_R,
        AreaJyotyuType_W
    };

    struct Area 
    {
        sz_t area_index = 0;

        std::array<Tileset, 3> tilesets;

        u8 start_entrance_id;
        u8 midway_entrance_id;
        u8 settings;
        u16 time_limit;

        u8 main_tileset_table_offset;
        u8 background_top_offset;
        u8 background_btm_offset;
        u8 bgnscid;

        std::vector<UI::AreaRenderItem> items;
    };

    AreaJyotyuType_ get_jyotyu_type
    (
        const Area& area,
        const IO::ByteArray ov0,
        const sz_t& ov0_offset
    );

    Area load_area_from_disk
    (
        const std::filesystem::path& header_file,
        const std::filesystem::path& bgdat_file,
        const sz_t& index
    );

    void save_area
    (
        Area& level,
        std::string bgdat_fn
    );



    void add_item
    (   
        Area& level,
        UI::AreaRenderItem& item
    );

    void remove_item
    (
        Area& level,
        UI::AreaRenderItem& remove
    );
}