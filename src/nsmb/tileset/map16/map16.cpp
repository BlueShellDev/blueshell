#include "map16.h"

BS::NSMB::Map16Tile BS::NSMB::ReadMap16TileFromBytes(const BS::IO::ByteArray &map16Data, size_t readStartingAt)
{
    Map16Tile map16Tile;
    map16Tile.topLeft = Nitro::ReadTilemapTileFromBytes(map16Data, readStartingAt);
    map16Tile.topRight = Nitro::ReadTilemapTileFromBytes(map16Data, readStartingAt + Nitro::TilemapTile::SizeBytes);
    map16Tile.bottomLeft = Nitro::ReadTilemapTileFromBytes(map16Data, readStartingAt + 2 * Nitro::TilemapTile::SizeBytes);
    map16Tile.bottomRight = Nitro::ReadTilemapTileFromBytes(map16Data, readStartingAt + 3 * Nitro::TilemapTile::SizeBytes);
    return map16Tile;
}

void BS::NSMB::WriteMap16TileToBytes(BS::IO::ByteArray &map16Data, size_t writeStartingAt, const Map16Tile &map16Tile)
{
    Nitro::WriteTilemapTileToBytes(map16Data, writeStartingAt, map16Tile.topLeft);
    Nitro::WriteTilemapTileToBytes(map16Data, writeStartingAt + Nitro::TilemapTile::SizeBytes, map16Tile.topRight);
    Nitro::WriteTilemapTileToBytes(map16Data, writeStartingAt + 2 * Nitro::TilemapTile::SizeBytes, map16Tile.bottomLeft);
    Nitro::WriteTilemapTileToBytes(map16Data, writeStartingAt + 3 * Nitro::TilemapTile::SizeBytes, map16Tile.bottomRight);
}

BS::NSMB::Map16 BS::NSMB::ReadMap16FromBytes(const BS::IO::ByteArray &map16Data)
{
    Map16 map16;
    size_t offset = 0;
    while (offset < map16Data.bytes.size())
    {
        Map16Tile tile = ReadMap16TileFromBytes(map16Data, offset);
        map16.push_back(tile);
        offset += Map16Tile::SizeBytes;
    }
    return map16;
}

BS::NSMB::Map16 BS::NSMB::ReadMap16FromFile(const std::string &filePath)
{
    BS::IO::ByteArray map16Data = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadMap16FromBytes(map16Data);
}

void BS::NSMB::WriteMap16ToBytes(BS::IO::ByteArray &map16Data, const Map16 &map16)
{
    map16Data.bytes.clear();
    for (const Map16Tile &tile : map16)
        WriteMap16TileToBytes(map16Data, map16Data.bytes.size(), tile);
}

void BS::NSMB::WriteMap16ToFile(const std::string &filePath, const Map16 &map16)
{
    BS::IO::ByteArray map16Data;
    WriteMap16ToBytes(map16Data, map16);
    BS::IO::WriteByteArrayToFile(filePath, map16Data);
}
