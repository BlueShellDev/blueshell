#pragma once
#include "nitro/graphics/nsc.h"

namespace BS
{
    namespace NSMB
    {
        struct Map16Tile
        {
            static constexpr size_t SizeBytes = 4 * Nitro::TilemapTile::SizeBytes;

            size_t id{0};
            Nitro::TilemapTile topLeft, topRight, bottomLeft, bottomRight;
        };

        Map16Tile ReadMap16TileFromBytes(const BS::IO::ByteArray &map16Data, size_t readStartingAt);
        void WriteMap16TileToBytes(BS::IO::ByteArray &map16Data, size_t writeStartingAt, const Map16Tile &map16Tile);

        using Map16 = std::vector<Map16Tile>;

        Map16 ReadMap16FromBytes(const BS::IO::ByteArray &map16Data);
        Map16 ReadMap16FromFile(const std::string &filePath);
        void WriteMap16ToBytes(BS::IO::ByteArray &map16Data, const Map16 &map16);
        void WriteMap16ToFile(const std::string &filePath, const Map16 &map16);
    }
}