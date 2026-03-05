#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaActor
        {
            u16 ID;

            UPosition16 position;

            u8 eventIDs[2];
            u16 settings;
        };
    }
};