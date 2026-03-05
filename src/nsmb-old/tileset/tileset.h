#pragma once

#include "nitro/graphics/ncg.h"
#include "nitro/graphics/ncl.h"
#include "nsmb/tileset/map16/map16.h"
#include "nsmb/tileset/map16/map16_behavior.h"
#include "nsmb/tileset/tileset_object.h"

namespace BS
{
    namespace NSMB
    {
        // TODO: complete
        struct Tileset
        {
            Nitro::CharacterSet characters;
            Nitro::ExtendedPalette palette;
            Map16 map16;
            TilesetObjectSet objects;
            // behaviors
        };
    }
}