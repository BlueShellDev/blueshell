#pragma once

#include "core/types.h"

namespace BS
{
    namespace NSMB
    {
        struct AreaCamera
        {
            i32 topOffset;
            i32 topOffsetEx;
            i32 bottomOffset;
            i32 bottomOffsetEx;

            u16 ID;

            u16 scrollHeight;

            u32 unk_0x14;
        };
    }
};