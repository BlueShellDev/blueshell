#include "nsc.h"
#include "core/systems/io/binary_file.h"

BS::Nitro::TilemapTile BS::Nitro::ReadTilemapTileFromBytes(const BS::IO::ByteArray &tileData, size_t offset)
{
    u16 tileValue = tileData.Read<u16>(offset);
    TilemapTile tile;
    tile.characterIndex = tileValue & 0x3FF;
    tile.hFlip = (tileValue >> 10) & 0x1;
    tile.vFlip = (tileValue >> 11) & 0x1;
    tile.paletteIndex = (tileValue >> 12) & 0xF;
    return tile;
}

void BS::Nitro::WriteTilemapTileToBytes(BS::IO::ByteArray &tileData, size_t offset, const TilemapTile &tile)
{
    u16 tileValue = 0;
    tileValue |= (tile.characterIndex & 0x3FF);
    tileValue |= ((tile.hFlip ? 1 : 0) << 10);
    tileValue |= ((tile.vFlip ? 1 : 0) << 11);
    tileValue |= ((tile.paletteIndex & 0xF) << 12);
    tileData.Write<u16>(offset, tileValue);
}

BS::Nitro::Tilemap BS::Nitro::ReadTilemapFromBytes(const BS::IO::ByteArray &tilemapData)
{
    Tilemap tilemap;
    size_t offset = 0;
    while (offset < tilemapData.bytes.size())
    {
        TilemapTile tile = ReadTilemapTileFromBytes(tilemapData, offset);
        tilemap.tiles.push_back(tile);
        offset += TilemapTile::SizeBytes;
    }
    return tilemap;
}

BS::Nitro::Tilemap BS::Nitro::ReadTilemapFromFile(const std::string &filePath)
{
    BS::IO::ByteArray tilemapData = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadTilemapFromBytes(tilemapData);
}

void BS::Nitro::WriteTilemapToBytes(BS::IO::ByteArray &tilemapData, const Tilemap &tilemap)
{
    tilemapData.bytes.clear();
    size_t offset = 0;
    for (const TilemapTile &tile : tilemap.tiles)
    {
        WriteTilemapTileToBytes(tilemapData, offset, tile);
        offset += TilemapTile::SizeBytes;
    }
}

void BS::Nitro::WriteTilemapToFile(const std::string &filePath, const Tilemap &tilemap)
{
    BS::IO::ByteArray tilemapData;
    WriteTilemapToBytes(tilemapData, tilemap);
    BS::IO::WriteByteArrayToFile(filePath, tilemapData);
}
