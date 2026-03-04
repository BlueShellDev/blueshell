#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaView
        {
            URect16 rect;

            u8 ID;
            u8 cameraID;
            u8 musicID;

            u8 unusedBackgroundID;
            u8 unusedTilesetID;
            u8 unusedForegroundID;

            u8 lightingType;
            u8 progressPathID;
        };
    }
};