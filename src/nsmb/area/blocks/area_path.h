#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaPathNode
        {
            UPosition16 position;

            float speed;
            float acceleration;
            u16 delay;
            u16 flags;
        };

        struct AreaPath
        {
            u16 ID;
            u16 headNodeID;

            u16 totalNodes;

            u16 unk_0x06;
        };
    }
};