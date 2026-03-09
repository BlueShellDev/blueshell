#include "nsmb/simple_nsmb.h"

namespace
{
    inline Core::Addr32 ConsumeTilesetObjectRow(const Core::ByteArray &bytes, Core::Addr32 read_at)
    {
        while (read_at < bytes.bytes.size())
        {
            const u8 control = bytes.Read<u8>(read_at);
            if (control == NSMB::Tilesets::TilesetObjectControlFlag_TerminateRow)
                return read_at + 1;

            if (control & NSMB::Tilesets::TilesetObjectControlFlag_Slope)
                ++read_at;

            if (read_at + NSMB::Tilesets::ObjectTileSizeBytes > bytes.bytes.size())
                return static_cast<Core::Addr32>(bytes.bytes.size());
            read_at += NSMB::Tilesets::ObjectTileSizeBytes;
        }

        return static_cast<Core::Addr32>(bytes.bytes.size());
    }

    inline Core::Addr32 ConsumeTilesetObject(const Core::ByteArray &bytes, Core::Addr32 read_at)
    {
        while (read_at < bytes.bytes.size())
        {
            const u8 control = bytes.Read<u8>(read_at);
            if (control == NSMB::Tilesets::TilesetObjectControlFlag_TerminateObject)
                return read_at + 1;

            read_at = ConsumeTilesetObjectRow(bytes, read_at);
        }

        return static_cast<Core::Addr32>(bytes.bytes.size());
    }
}

NSMB::Tilesets::Map16Tile NSMB::Tilesets::ReadMap16TileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Map16Tile tile;
    for (size_t y = 0; y < Map16TileHeightTiles; y++)
        for (size_t x = 0; x < Map16TileWidthTiles; x++)
            tile.scr_tiles[y][x] = Nitro::ReadScreenTileFromBytes(bytes, read_at + ((x * Map16TileWidthTiles + y) * Map16TileSizeBytes));
    //tile.id = static_cast<Map16TileID>(read_at / Map16TileSizeBytes);
    return tile;
}

NSMB::Tilesets::Map16 NSMB::Tilesets::ReadMap16FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Map16 map16_set;
    Core::Addr32 offset = read_at;
    Map16TileID id = 0;
    while (offset + Map16TileSizeBytes <= bytes.bytes.size())
    {
        Map16Tile tile = ReadMap16TileFromBytes(bytes, offset);
        tile.id = id++;
        map16_set.push_back(tile);
        offset += Map16TileSizeBytes;
    }
    return map16_set;
}

NSMB::Tilesets::BehaviorTile NSMB::Tilesets::ReadBehaviorTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    BehaviorTile tile;
    tile.id = bytes.Read<BehaviorTileID>(read_at);
    return tile;
}

NSMB::Tilesets::BehaviorSet NSMB::Tilesets::ReadBehaviorSetFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    BehaviorSet behavior_set;
    Core::Addr32 offset = read_at;
    BehaviorTileID id = 0;
    while (offset + BehaviorTileSizeBytes <= bytes.bytes.size())
    {
        BehaviorTile tile = ReadBehaviorTileFromBytes(bytes, offset);
        tile.id = id++;
        behavior_set.push_back(tile);
        offset += BehaviorTileSizeBytes;
    }
    return behavior_set;
}

NSMB::Tilesets::ObjectTile NSMB::Tilesets::ReadObjectTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    ObjectTile tile;
    tile.id = bytes.Read<ObjectTileID>(read_at + 1);
    return tile;
}

NSMB::Tilesets::ObjectRow NSMB::Tilesets::ReadObjectRowFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    ObjectRow row;
    row.id = static_cast<ObjectRowID>(read_at / ObjectTileSizeBytes);
    (void)ConsumeTilesetObjectRow(bytes, read_at);
    return row;
}

NSMB::Tilesets::Object NSMB::Tilesets::ReadObjectFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Object object;
    object.id = static_cast<ObjectID>(read_at);
    (void)ConsumeTilesetObject(bytes, read_at);
    return object;
}

NSMB::Tilesets::ObjectSet NSMB::Tilesets::ReadObjectSetFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    ObjectSet object_set;
    Core::Addr32 offset = read_at;
    ObjectID id = 0;
    while (offset < bytes.bytes.size())
    {
        if (bytes.Read<u8>(offset) == TilesetObjectControlFlag_TerminateObject)
        {
            ++offset;
            continue;
        }

        Object object = ReadObjectFromBytes(bytes, offset);
        object.id = id++;
        object_set.push_back(object);

        const Core::Addr32 next = ConsumeTilesetObject(bytes, offset);
        if (next <= offset)
            break;
        offset = next;
    }
    return object_set;
}

NSMB::Backgrounds::Background NSMB::Backgrounds::ReadBackgroundFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Background background;
    if (read_at < bytes.bytes.size())
        background.id = bytes.Read<BackgroundID>(read_at);
    return background;
}
