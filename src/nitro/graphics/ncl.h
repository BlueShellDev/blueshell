#pragma once
#include <array>
#include <vector>
#include "core/types.h"
#include "core/systems/io/byte_array.h"

namespace BS
{
    namespace Nitro
    {
        // we won't store palettes interally as 16x16 colors, because they can just be represented as such in the frontend
        // though we don't currenly have a way of reading a 16 color palette from bytes/file, only representing a 16x16 color palette
        using BGR565 = u16;

        struct Palette
        {
            static constexpr size_t ColorsCount = 256;
            static constexpr size_t SizeBytes = sizeof(BGR565) * ColorsCount;

            size_t id{0};
            std::array<BGR565, ColorsCount> colors;
        };

        struct ExtendedPalette
        {
            std::vector<Palette> subPalettes;
        };

        Palette ReadPaletteFromBytes(const BS::IO::ByteArray &nclData);
        Palette ReadPaletteFromFile(const std::string &filePath);
        ExtendedPalette ReadExPaletteFromBytes(const BS::IO::ByteArray &nclData);
        ExtendedPalette ReadExPaletteFromFile(const std::string &filePath);

        void WritePaletteToBytes(BS::IO::ByteArray &nclData, const Palette &palette);
        void WritePaletteToFile(const std::string &filePath, const Palette &palette);
        void WriteExPaletteToBytes(BS::IO::ByteArray &nclData, const ExtendedPalette &palette);
        void WriteExPaletteToFile(const std::string &filePath, const ExtendedPalette &palette);

        // transforms the nitro BGR565 format GBBBBBGGGGGRRRRR to BBBBBGGGGGGRRRRR
        void NitroBGRToTrueBGR(BGR565 &colorToTransform);
    }
}