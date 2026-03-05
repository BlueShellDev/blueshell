#pragma once

#include "nitro/graphics/ncg.h"
#include "nitro/graphics/ncl.h"
#include "nitro/graphics/nsc.h"

namespace BS
{
    namespace NSMB
    {
        struct Background
        {
            Nitro::CharacterSet characters;
            Nitro::ExtendedPalette palette;
            Nitro::Tilemap tilemap;
        };
    }
}