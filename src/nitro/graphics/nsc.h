#pragma once
#include <array>
#include <vector>
#include "core/types.h"
#include "core/systems/io/binary_file.h"

namespace BS
{
    namespace Nitro
    {
        using CharacterIndex = u16; // 10-bit value, indexes to a 8x8 character in the character set
        using PaletteIndex = u8;    // 4-bit value, indexes to a 16-color sub-palette in the standard palette, OR a 256-color sub-palette in the extended palette

        // screen tile format is 16 bits per tile, with the following bit layout:
        // PPPPVHCCCCCCCCCC
        // P = palette index bit (4 bits)
        // V = vertical flip bit
        // H = horizontal flip bit
        // C = character index bits (10 bits)
        struct TilemapTile
        {
            static constexpr size_t SizeBytes = 2;

            size_t id{0};
            CharacterIndex characterIndex{0};
            PaletteIndex paletteIndex{0};
            bool hFlip{false};
            bool vFlip{false};
        };

        struct Tilemap
        {
            std::vector<TilemapTile> tiles;
        };

        TilemapTile ReadTilemapTileFromBytes(const BS::IO::ByteArray &tileData, size_t offset);
        Tilemap ReadTilemapFromBytes(const BS::IO::ByteArray &tilemapData);
        Tilemap ReadTilemapFromFile(const std::string &filePath);

        void WriteTilemapTileToBytes(BS::IO::ByteArray &tileData, size_t offset, const TilemapTile &tile);
        void WriteTilemapToBytes(BS::IO::ByteArray &tilemapData, const Tilemap &tilemap);
        void WriteTilemapToFile(const std::string &filePath, const Tilemap &tilemap);
    }
}