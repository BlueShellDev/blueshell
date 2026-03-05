#include "nitro/simple_nitro.h"

void Nitro::NitroBGRToTrueBGR(const Color &colorToTransform)
{
    // Signature currently takes const ref, so this keeps the converted value local.
    const u16 lowest_green_bit = (colorToTransform & (0x1 << 15)) >> 15;
    const u16 blue_ch = (colorToTransform & (0x1F << 10)) >> 10;
    const u16 green_ch = (((colorToTransform & (0x1F << 5)) >> 5) << 1) | lowest_green_bit;
    const u16 red_ch = colorToTransform & 0x1F;
    [[maybe_unused]] const Color converted = (blue_ch << 11) | (green_ch << 5) | red_ch;
}
