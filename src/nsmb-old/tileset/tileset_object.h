#pragma once
#include "map16/map16.h"

namespace BS
{
    namespace NSMB
    {
        using Map16TileIndex = u16;
        using TilesetObjectControl = u8;

        enum TilesetObjectControlFlag : TilesetObjectControl
        {
            None = 0,
            RepeatX = 1 << 0,
            RepeatY = 1 << 1,
            Slope = 1 << 7,
            TerminateRow = 0xFE,
            TerminateObject = 0xFF
        };

        struct TilesetObjectTile
        {
            static constexpr size_t SizeBytes = 3;

            size_t id{0};
            TilesetObjectControl flags{0};
            Map16TileIndex map16TileIndex;
        };

        struct TilesetObjectRow
        {
            size_t id{0};
            TilesetObjectControl flags{0};
            std::vector<TilesetObjectTile> tiles;
        };

        struct TilesetObject
        {
            size_t id{0};
            std::vector<TilesetObjectRow> rows;
        };

        using TilesetObjectSet = std::vector<TilesetObject>;

        constexpr size_t GetObjectRowSizeBytes(const TilesetObjectRow &objectRow);
        constexpr size_t GetObjectSizeBytes(const TilesetObject &object);

        TilesetObjectTile ReadObjectTileFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt);
        TilesetObjectRow ReadObjectRowFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt);
        TilesetObject ReadObjectFromBytes(const BS::IO::ByteArray &objectDataBytes, size_t readStartingAt = 0);
        TilesetObjectSet ReadObjectSetFromBytes(const BS::IO::ByteArray &objectDataBytes);
        TilesetObjectSet ReadObjectSetFromFile(const std::string &filePath);

        void WriteObjectTileToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObjectTile &objectTile);
        void WriteObjectRowToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObjectRow &objectRow);
        void WriteObjectToBytes(BS::IO::ByteArray &objectDataBytes, size_t writeStartingAt, const TilesetObject &object);
        void WriteObjectSetToBytes(BS::IO::ByteArray &objectDataBytes, const TilesetObjectSet &objectSet);
        void WriteObjectSetToFile(const std::string &filePath, const TilesetObjectSet &objectSet);
    }
}