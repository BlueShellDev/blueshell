#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        enum class AreaEntranceType : u8
        {
            Normal,
            OverriddenNormal,
            Door,
            PipeUp,
            PipeDown,
            PipeLeft,
            PipeRight,
            Falling,
            GroundPounding,
            Sliding,
            Swimming,
            TransitNormal,
            TransitDoor,
            MvsLPipe,
            Unknown14,
            Unknown15,
            MiniPipeUp,
            MiniPipeDown,
            MiniPipeLeft,
            MiniPipeRight,
            Jumping,
            Vine
        };

        enum class AreaEntranceFlags : u8
        {
            SubScreen = 1 << 0,
            LockedPlayer1 = 1 << 1,
            LockedPlayer0 = 1 << 2,
            ConnectedPipe = 1 << 3,
            WipeFade = 1 << 4,
            AlignedX = 1 << 6,
            ExitOnly = 1 << 7
        };

        struct AreaEntrance
        {
            UPosition16 position;
            UPosition16 cameraPosition;

            u8 ID;

            u8 destinationAreaID;
            u8 destinationLevelID;
            u8 destinationWorldID;
            u8 destinationEntranceID;
            u8 cameraZoomLevel;

            AreaEntranceType type;
            AreaEntranceFlags flags;

            u8 unk_0x10;
            u8 unk_0x11;

            u8 viewID; // applies to ORIGIN entrance
            u8 destinationWorldmapNodeID;
        };
    }
};