#include "tileset_object.h"
#include "core/console.h"

constexpr size_t BS::NSMB::GetObjectRowSizeBytes(const TilesetObjectRow &objectRow)
{
    size_t size = 0;
    for (size_t i = 0; i < objectRow.tiles.size(); i++)
        size += TilesetObjectTile::SizeBytes;

    if (objectRow.flags & TilesetObjectControlFlag::Slope)
        size += sizeof(TilesetObjectControl); // for the slope flag before each tile

    size += sizeof(TilesetObjectControl); // for the control flag at the end of the row
    return size;
}

constexpr size_t BS::NSMB::GetObjectSizeBytes(const TilesetObject &object)
{
    size_t size = 0;
    for (const TilesetObjectRow &row : object.rows)
        size += GetObjectRowSizeBytes(row);

    size += sizeof(TilesetObjectControl); // for the terminate object flag
    return size;
}

BS::NSMB::TilesetObjectTile BS::NSMB::ReadObjectTileFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt)
{
    TilesetObjectTile objectTile;
    objectTile.flags = objectDataBytes.Read<TilesetObjectControl>(readStartingAt);
    objectTile.map16TileIndex = objectDataBytes.Read<Map16TileIndex>(readStartingAt + sizeof(TilesetObjectControl));
    return objectTile;
}

BS::NSMB::TilesetObjectRow BS::NSMB::ReadObjectRowFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt)
{
    TilesetObjectRow objectRow;
    size_t tileID = 0;
    while (readStartingAt < objectDataBytes.bytes.size())
    {
        const TilesetObjectControl initialFlag = objectDataBytes.Read<TilesetObjectControl>(readStartingAt);
        if (initialFlag == TilesetObjectControlFlag::TerminateRow)
            break;

        if (tileID == 0) // for the first tile, read the flags directly
        {
            objectRow.flags = initialFlag;
            if (objectRow.flags & TilesetObjectControlFlag::Slope)
                readStartingAt++;
        }

        TilesetObjectTile tile = ReadObjectTileFromBytes(objectDataBytes, readStartingAt);
        tile.id = tileID++;
        objectRow.tiles.push_back(tile);
        readStartingAt += TilesetObjectTile::SizeBytes;
    }
    return objectRow;
}

BS::NSMB::TilesetObject BS::NSMB::ReadObjectFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt)
{
    TilesetObject object;
    size_t rowID = 0;
    while (readStartingAt < objectDataBytes.bytes.size())
    {
        const TilesetObjectControl initialFlag = objectDataBytes.Read<TilesetObjectControl>(readStartingAt);
        if (initialFlag == TilesetObjectControlFlag::TerminateObject)
            break;

        TilesetObjectRow row = ReadObjectRowFromBytes(objectDataBytes, readStartingAt);
        row.id = rowID++;
        object.rows.push_back(row);
        readStartingAt += GetObjectRowSizeBytes(row);
    }
    return object;
}

BS::NSMB::TilesetObjectSet BS::NSMB::ReadObjectSetFromBytes(const BS::IO::ByteArray &objectDataBytes)
{
    TilesetObjectSet objectSet;
    size_t offset = 0;
    size_t objectID = 0;
    while (offset < objectDataBytes.bytes.size())
    {
        TilesetObject object = ReadObjectFromBytes(objectDataBytes, offset);
        object.id = objectID++;
        objectSet.push_back(object);
        offset += GetObjectSizeBytes(object);
    }
    return objectSet;
}

BS::NSMB::TilesetObjectSet BS::NSMB::ReadObjectSetFromFile(const std::string &filePath)
{
    BS::IO::ByteArray objectData = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadObjectSetFromBytes(objectData);
}

void BS::NSMB::WriteObjectTileToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObjectTile &objectTile)
{
    objectDataBytes.Write<TilesetObjectControl>(writeStartingAt, objectTile.flags);
    objectDataBytes.Write<Map16TileIndex>(writeStartingAt + sizeof(TilesetObjectControl), objectTile.map16TileIndex);
}

void BS::NSMB::WriteObjectRowToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObjectRow &objectRow)
{
    if (objectRow.tiles.empty())
    {
        objectDataBytes.Write<TilesetObjectControl>(writeStartingAt, TilesetObjectControlFlag::TerminateRow);
        return;
    }

    if (objectRow.flags & TilesetObjectControlFlag::Slope)
    {
        objectDataBytes.Write<TilesetObjectControl>(writeStartingAt, TilesetObjectControlFlag::Slope);
        writeStartingAt++;
    }

    for (const TilesetObjectTile &tile : objectRow.tiles)
    {
        if (tile.flags & TilesetObjectControlFlag::Slope)
            objectDataBytes.Write<TilesetObjectControl>(writeStartingAt++, TilesetObjectControlFlag::Slope);

        WriteObjectTileToBytes(objectDataBytes, writeStartingAt, tile);
        writeStartingAt += TilesetObjectTile::SizeBytes;
    }
    objectDataBytes.Write<TilesetObjectControl>(writeStartingAt, TilesetObjectControlFlag::TerminateRow);
}

void BS::NSMB::WriteObjectToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObject &object)
{
}
