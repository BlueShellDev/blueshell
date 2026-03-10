#pragma once

#include <string>
#include "nitro/simple_nitro.h"

namespace BS::Game
{
    struct BackgroundDefinition
    {
        Nitro::FS::FileID bitmapFileID;
        Nitro::FS::FileID paletteFileID;
        Nitro::FS::FileID screenFileID;
    };

    struct TilesetDefinition
    {
        Nitro::FS::FileID bitmapFileID;
        Nitro::FS::FileID paletteFileID;
        Nitro::FS::FileID map16FileID;
        Nitro::FS::FileID objectFileID;
        Nitro::FS::FileID behaviorFileID;
    };

    // will house functions like
    // LoadArea, LoadTileset, LoadBackground
};