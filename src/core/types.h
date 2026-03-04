#pragma once
#include <cstdint>

using u32_address = uint32_t;
using u32_size_t = uint32_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

namespace BS
{
    struct UPosition16;
    struct URect16;

    struct UPosition16
    {
        u16 x;
        u16 y;
    };

    struct UPosition32
    {
        u32 x;
        u32 y;
    };

    struct URect16
    {
        UPosition16 pos;
        u16 width;
        u16 height;
    };

    struct URect32
    {
        UPosition32 pos;
        u32 width;
        u32 height;
    };
};