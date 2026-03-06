#include "nsmb/simple_nsmb.h"

namespace
{
    inline Core::Addr32 ConsumeTilesetObjectRow(const Core::ByteArray &bytes, Core::Addr32 read_at)
    {
        while (read_at < bytes.bytes.size())
        {
            const u8 control = bytes.Read<u8>(read_at);
            if (control == NSMB::GFX::TilesetObjectControlFlag_TerminateRow)
                return read_at + 1;

            if (control & NSMB::GFX::TilesetObjectControlFlag_Slope)
                ++read_at;

            if (read_at + NSMB::GFX::TilesetObjectTileSizeBytes > bytes.bytes.size())
                return static_cast<Core::Addr32>(bytes.bytes.size());
            read_at += NSMB::GFX::TilesetObjectTileSizeBytes;
        }

        return static_cast<Core::Addr32>(bytes.bytes.size());
    }

    inline Core::Addr32 ConsumeTilesetObject(const Core::ByteArray &bytes, Core::Addr32 read_at)
    {
        while (read_at < bytes.bytes.size())
        {
            const u8 control = bytes.Read<u8>(read_at);
            if (control == NSMB::GFX::TilesetObjectControlFlag_TerminateObject)
                return read_at + 1;

            read_at = ConsumeTilesetObjectRow(bytes, read_at);
        }

        return static_cast<Core::Addr32>(bytes.bytes.size());
    }
}

NSMB::GFX::TilesetMap16Tile NSMB::GFX::ReadTilesetMap16TileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    (void)bytes;
    TilesetMap16Tile tile;
    tile.id = static_cast<TilesetMap16TileID>(read_at / TilesetMap16TileSizeBytes);
    return tile;
}

NSMB::GFX::TilesetMap16Set NSMB::GFX::ReadTilesetMap16SetFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetMap16Set map16_set;
    Core::Addr32 offset = read_at;
    TilesetMap16TileID id = 0;
    while (offset + TilesetMap16TileSizeBytes <= bytes.bytes.size())
    {
        TilesetMap16Tile tile = ReadTilesetMap16TileFromBytes(bytes, offset);
        tile.id = id++;
        map16_set.push_back(tile);
        offset += TilesetMap16TileSizeBytes;
    }
    return map16_set;
}

NSMB::GFX::TilesetBehaviorTile NSMB::GFX::ReadTilesetBehaviorTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetBehaviorTile tile;
    tile.id = bytes.Read<TilesetBehaviorTileID>(read_at);
    return tile;
}

NSMB::GFX::TilesetBehaviorSet NSMB::GFX::ReadTilesetBehaviorSetFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetBehaviorSet behavior_set;
    Core::Addr32 offset = read_at;
    TilesetBehaviorTileID id = 0;
    while (offset + TilesetBehaviorTileSizeBytes <= bytes.bytes.size())
    {
        TilesetBehaviorTile tile = ReadTilesetBehaviorTileFromBytes(bytes, offset);
        tile.id = id++;
        behavior_set.push_back(tile);
        offset += TilesetBehaviorTileSizeBytes;
    }
    return behavior_set;
}

NSMB::GFX::TilesetObjectTile NSMB::GFX::ReadTilesetObjectTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetObjectTile tile;
    tile.id = bytes.Read<TilesetObjectTileID>(read_at + 1);
    return tile;
}

NSMB::GFX::TilesetObjectRow NSMB::GFX::ReadTilesetObjectRowFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetObjectRow row;
    row.id = static_cast<TilesetObjectRowID>(read_at / TilesetObjectTileSizeBytes);
    (void)ConsumeTilesetObjectRow(bytes, read_at);
    return row;
}

NSMB::GFX::TilesetObject NSMB::GFX::ReadTilesetObjectFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetObject object;
    object.id = static_cast<TilesetObjectID>(read_at);
    (void)ConsumeTilesetObject(bytes, read_at);
    return object;
}

NSMB::GFX::TilesetObjectSet NSMB::GFX::ReadTilesetObjectSetFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    TilesetObjectSet object_set;
    Core::Addr32 offset = read_at;
    TilesetObjectID id = 0;
    while (offset < bytes.bytes.size())
    {
        if (bytes.Read<u8>(offset) == TilesetObjectControlFlag_TerminateObject)
        {
            ++offset;
            continue;
        }

        TilesetObject object = ReadTilesetObjectFromBytes(bytes, offset);
        object.id = id++;
        object_set.push_back(object);

        const Core::Addr32 next = ConsumeTilesetObject(bytes, offset);
        if (next <= offset)
            break;
        offset = next;
    }
    return object_set;
}

NSMB::GFX::Background NSMB::GFX::ReadBackgroundFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Background background;
    if (read_at < bytes.bytes.size())
        background.id = bytes.Read<BackgroundID>(read_at);
    return background;
}
