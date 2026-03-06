// single-header dead simple NSMB lib (blueshell '26)
#pragma once

#include "core/types.h"
#include "nitro/simple_nitro.h"

namespace NSMB
{
    //
    // TYPES
    //

    struct Area;
    struct AreaSettings;
    struct AreaScreen;
    struct AreaEntrance;
    struct AreaObject;
    struct AreaView;
    struct AreaZone;
    struct AreaPath;
    struct AreaPathNode;

    struct AreaBGDat;
    struct AreaBGDatObject;

    //
    // CONSTANTS
    //

    //
    // FUNCTIONS: READS
    //

    //
    // FUNCTION: WRITES
    //

    //
    // FUNCTION: UTILITIES
    //

    //
    // STRUCTURES
    //
};

namespace NSMB::GFX
{
    /// BEG-SECTION: TILESETS ///

    //
    // TYPES
    //

    struct TilesetMap16Tile;
    struct TilesetBehaviorTile;
    struct TilesetObject;
    struct TilesetObjectRow;
    struct TilesetObjectTile;
    struct Tileset;
    
    using TilesetID                                             = u8;
    using TilesetMap16TileID                                    = u16;
    using TilesetMap16Set                                       = std::vector<TilesetMap16Tile>;
    using TilesetBehaviorTileID                                 = u16;
    using TilesetBehaviorSet                                    = std::vector<TilesetBehaviorTile>;
    using TilesetObjectTileID                                   = u16;
    using TilesetObjectRowID                                    = u16;
    using TilesetObjectID                                       = u16;
    using TilesetObjectSet                                      = std::vector<TilesetObject>;

    enum TilesetObjectControlFlag : u8
    {
        TilesetObjectControlFlag_None                           = 0,
        TilesetObjectControlFlag_RepeatX                        = 1 << 0,
        TilesetObjectControlFlag_RepeatY                        = 1 << 1,
        TilesetObjectControlFlag_Slope                          = 1 << 7,
        TilesetObjectControlFlag_TerminateRow                   = 0xFE,
        TilesetObjectControlFlag_TerminateObject                = 0xFF
    };

    enum class TilesetType
    {
        Jyotyu,
        CourseSpecific,
        SubNohara
    };

    //
    // CONSTANTS
    //

    // TOOD: add background constants?
    inline constexpr size_t     TilesetMap16TileSizeBytes       = 4 * Nitro::ScreenTileSizeBytes;

    inline constexpr size_t     TilesetBehaviorTileSizeBytes    = 4;
    
    inline constexpr size_t     TilesetObjectTileSizeBytes      = 3;

    //
    // FUNCTIONS: READS
    //

    inline TilesetMap16Tile     ReadTilesetMap16TileFromBytes   (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline TilesetMap16Set      ReadTilesetMap16SetFromBytes    (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    inline TilesetBehaviorTile  ReadTilesetBehaviorTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline TilesetBehaviorSet   ReadTilesetBehaviorSetFromBytes (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    
    inline TilesetObjectTile    ReadTilesetObjectTileFromBytes  (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline TilesetObjectRow     ReadTilesetObjectRowFromBytes   (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline TilesetObject        ReadTilesetObjectFromBytes      (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline TilesetObjectSet     ReadTilesetObjectSetFromBytes   (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    inline void                 WriteTilesetMap16TileToBytes    (const TilesetMap16Tile& map16_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    inline void                 WriteTilesetMap16SetToBytes     (const TilesetMap16Set& map16, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    inline void                 WriteTilesetBehaviorTileToBytes (const TilesetBehaviorTile& behav_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    inline void                 WriteTilesetBehaviorSetToBytes  (const TilesetBehaviorSet& behav_set, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    
    inline void                 WriteTilesetObjectTileToBytes   (const TilesetObjectTile& obj_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    inline void                 WriteTilesetObjectRowToBytes    (const TilesetObjectRow& obj_row, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    inline void                 WriteTilesetObjectToBytes       (const TilesetObject& obj, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    inline void                 WriteTilesetObjectSetToBytes    (const TilesetObjectSet& obj_set, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    //
    // STRUCTURES
    //

    struct TilesetMap16Tile
    {
        TilesetMap16TileID      id;

        inline                  TilesetMap16Tile()              {};
    };

    struct TilesetBehaviorTile
    {
        TilesetBehaviorTileID   id;

        inline                  TilesetBehaviorTile()           {};
    };

    struct TilesetObjectTile
    {
        TilesetObjectTileID     id;

        inline                  TilesetObjectTile()             {};
    };

    struct TilesetObjectRow
    {
        TilesetObjectRowID      id;

        inline                  TilesetObjectRow()              {};
    };

    struct TilesetObject
    {
        TilesetObjectID         id;

        inline                  TilesetObject()                 {};
    };

    struct Tileset
    {
        TilesetID               id;

        Nitro::CharacterBitmap  characterBitmap;
        Nitro::ExPalette        extendedPalette;
        TilesetMap16Set         map16;
        TilesetObjectSet        objects;
        TilesetBehaviorSet      behaviors;

        inline                  Tileset()                       { id = 0; };
    };
    
    /// END-SECTION: TILESETS ///
    /// BEG-SECTION: BACKGROUNDS ///

    //
    // TYPES
    //

    struct Background;

    using BackgroundID                                          = u8;

    enum class BackgroundType
    {
        Top,
        Bottom
    };

    //
    // CONSTANTS
    //

    //
    // FUNCTIONS: READS
    //

    inline Background           ReadBackgroundFromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    inline void                 WriteBackgroundToBytes          (const Background& bg, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    //
    // FUNCTIONS: UTILITIES
    //

    //
    // STRUCTURES
    //

    struct Background
    {
        BackgroundID            id;

        inline                  Background()                    {};
    };

    /// END-SECTION: BACKGROUNDS ///
};