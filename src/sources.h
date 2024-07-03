#pragma once

#include <string>
#include <vector>
#include "util/types.h"

namespace Editor
{
    struct AreaSource
    {
        sz_t area_id;
        std::string header_fn;
        std::string bgdat_fn;
    };

    struct LevelSource
    {
        std::string name;
        std::vector<AreaSource> areas;
    };

    struct WorldEntry
    {
        std::string name;
        std::vector<LevelSource> levels;
    };

    struct TilesetEntry
    {
        u32 id;
        std::string name;
        bool unused;
    };
}