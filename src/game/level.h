#pragma once

#include "tileset.h"
#include "objects.h"

namespace NSMB
{
    struct AreaSource
    {
        sz_t                            area_id;
        std::string                     header_fn;
        std::string                     bgdat_fn;
    };

    struct LevelSource
    {
        std::string name;
        std::vector<NSMB::AreaSource>   areas;
    };

    struct Area 
    {
        AreaSource                      source;

        std::array<Tileset, 3>          tilesets;
       
        u8                              start_entrance_id;
        u8                              midway_entrance_id;
        u8                              settings;
        u16                             time_limit;

        u8                              main_tileset_table_offset;
        u8                              background_top_offset;
        u8                              background_btm_offset;
        u8                              bgnscid;

        std::vector<UI::AreaRenderItem> items;
    };

    Area load_area
    (
        const AreaSource& source
    );

    void save_area
    (
        NSMB::Area&                level,
        std::string                 bgdat_fn
    );

    void add_item
    (   
        NSMB::Area&                level,
        UI::AreaRenderItem&        item
    );

    void remove_item
    (
        NSMB::Area&                level,
        UI::AreaRenderItem&        remove
    );
}