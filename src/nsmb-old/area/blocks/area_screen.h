#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaScreen
        {
            u16 ID;

            u16 tilemapID;
            u16 paletteID;

            u16 parallaxInfoX;
            u16 parallaxInfoY;

            u16 scrollSpeedX;
            u16 scrollSpeedY;

            u16 verticalOffset;

            u16 unk_0x10;
            u16 unk_0x12;
        };
    }
}