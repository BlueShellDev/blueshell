#include "nitro/simple_nitro.h"

void Nitro::NitroBGRToTrueBGR(Color &to_true)
{
    const u16 lowest_green_bit = (to_true & (0x1 << 15)) >> 15;
    const u16 blue_ch = (to_true & (0x1F << 10)) >> 10;
    const u16 green_ch = (((to_true & (0x1F << 5)) >> 5) << 1) | lowest_green_bit;
    const u16 red_ch = to_true & 0x1F;
    to_true = (blue_ch << 11) | (green_ch << 5) | red_ch;
}

void Nitro::TrueBGRToNitroBGR(Color &to_nitro)
{
    // transform BBBBBGGGGGGRRRRR To GBBBBBGGGGGRRRRR
}
