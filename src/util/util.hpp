#pragma once

#include "debug.hpp"
#include "byteio.hpp"

namespace Util
{
    static std::ifstream begin_ifstream(const std::string& filename)
    {
        std::ifstream in
        (
            filename, 
            std::ios::in | std::ios::binary
        );

        if (!in.is_open())
            throw std::runtime_error("cannot open " + filename);

        return in;
    }

    static std::ofstream begin_ofstream(const std::string& filename)
    {
        std::ofstream out
        (
            filename,
            std::ios::out | std::ios::binary
        );

        if (!out.is_open())
            throw std::runtime_error("cannot open " + filename);

        return out;
    }

    struct Data
    {
        sz_t _id = 0;
        sz_t _from = 0;
    };

    inline sz_t xy_to_i
    (
        const sz_t &x,
        const sz_t &y,
        const sz_t &w
    )
    {
        return (y * w) + x;
    }

    inline void i_to_xy
    (
        const sz_t& i,
        const sz_t& w,
        sz_t& x,
        sz_t& y
    )
    {
        x = i % w;
        y = i / w;
    }
}

namespace Color
{
    static const u8 _max_uint5 = 0x1F;
    static const u8 _max_uint8 = 0xFF;

    /*
    RGB15 color format:

    A RRRRR GGGGG BBBBB
    */

    static
    u32 rgb15_to_rgb24(const u16& rgb15)
    {
        /* parse individual channels */
        u8 a_1 = (rgb15 >> 15) & 0b00001;
        u8 r_5 = (rgb15 >> 10) & 0b11111;
        u8 g_5 = (rgb15 >> 5) & 0b11111;
        u8 b_5 = (rgb15) & 0b11111;

        /* now we need to scale 5 active bits to 8 */
        /* this method isn't the fastest, nor the most bit-accurate, but it's */
        u8 a_8 = (a_1 * _max_uint8);
        u8 r_8 = (r_5 * _max_uint8) / _max_uint5;
        u8 g_8 = (g_5 * _max_uint8) / _max_uint5;
        u8 b_8 = (b_5 * _max_uint8) / _max_uint5;

        /* now concatinate into a single 32-bit wide integer */
        u32 res = (r_8 << 24) | (g_8 << 16) | (b_8 << 8) | a_8;

        return res;
    }

    static
    u16 rgb24_to_rgb15(const u32& rgb24)
    {
        /* again, parse the channels; 8 bits per channel this time */
        u8 r_8 = (rgb24 >> 24) & 0b11111111;
        u8 g_8 = (rgb24 >> 16) & 0b11111111;
        u8 b_8 = (rgb24 >> 8) & 0b11111111;
        u8 a_8 = (rgb24) & 0b11111111;

        /* scaling is easier this time, just shift to the right */
        u8 r_5 = r_8 >> 3;
        u8 g_5 = g_8 >> 3;
        u8 b_5 = b_8 >> 3;
        u8 a_5 = a_8 >> 7;

        /* concatinate */
        u16 res = (a_5 << 15) | (r_5 << 10) | (g_5 << 5) | b_5;

        return res;
    }
}