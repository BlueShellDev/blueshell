#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaHeader
        {
            u8 mainEntranceID{0};
            u8 midwayEntranceID{0};

            enum ExtraSettings : u16
            {
                StartAsMiniMario = 1 << 0,
                ForceMiniPhysics = 1 << 1,
                WrapAroundEdges = 1 << 5
            } settings{};
            u16 timeLimit{0};

            u16 bottomBackgroundIDs[3]{0};
            u16 tilesetIDs[3]{0};
            u16 topBackgroundIDs[3]{0};

            u16 events{0};
            u16 soundSet{0};
            u16 spritePriority{0};

            enum MegaDropChance : u16
            {
                OneInThree,
                OneInTwo,
                None
            } megaDropChance{OneInThree};
        };
    };
};