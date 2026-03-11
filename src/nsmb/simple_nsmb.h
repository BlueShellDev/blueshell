// single-header dead simple NSMB lib (blueshell '26)
#pragma once

#include "core/types.h"
#include "nitro/simple_nitro.h"

namespace NSMB
{
    //
    // TYPES
    //

    using AreaBlockID                                           = u8;
    struct AreaBlockDefinition;
    struct AreaHeader;
    struct AreaSettings;
    struct AreaCamera;
    struct AreaScreen;
    struct AreaEntrance;
    struct AreaObject;
    struct AreaView;
    struct AreaZone;
    struct AreaPath;
    struct AreaPathNode;
    struct Area;

    struct BGDatObject;

    enum AreaBlockType : AreaBlockID
    {

        SettingsBlock,
        CameraBlock,
        BottomBackgroundBlock,
        TilesetBlock,
        TopBackgoundBlock,
        EntrancesBlock,
        SpritesBlock,
        ViewsBlock,
        ZonesBlock,
        ProgressPathsBlock,
        PathsBlock,
        ProgressPathNodesBlock,
        PathNodesBlock,
        ObjectBanksBlock
    };

    //
    // CONSTANTS
    //

    inline constexpr size_t     AreaBlockDefSizeBytes           = 8;
    inline constexpr size_t     AreaHeaderBlockDefCount         = 14;
    inline constexpr size_t     AreaHeaderSizeBytes             = AreaHeaderBlockDefCount * AreaBlockDefSizeBytes;

    //
    // FUNCTIONS: READS
    //

    AreaBlockDefinition         ReadAreaBlockDefFromBytes       (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    AreaHeader                  ReadAreaHeaderFromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    AreaSettings                ReadAreaSettingsFromBytes       (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    BGDatObject                 ReadBGDatObjectFromBytes        (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTION: WRITES
    //

    void                        WriteAreaBlockDefToBytes        (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    void                        WriteAreaHeaderToBytes          (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    void                        WriteAreaSettingsToBytes        (Core::ByteArray &bytes, Core::Addr32 write_at = 0);

    void                        WriteBGDatObjectToBytes         (Core::ByteArray &bytes, Core::Addr32 write_at = 0);

    //
    // FUNCTION: UTILITIES
    //

    //
    // STRUCTURES
    //

    struct AreaBlockDefinition
    {
        AreaBlockID             id;
        Core::Addr32            base;
        size_t                  size;
    };

    struct AreaHeader
    {
        std::array<AreaBlockDefinition, AreaHeaderBlockDefCount> blockDefinitions;
    };
};

namespace NSMB::Tilesets
{
    //
    // TYPES
    //

    struct Map16Tile;
    struct BehaviorTile;
    struct Object;
    struct ObjectRow;
    struct ObjectTile;
    struct Tileset;
    
    using Map16TileID                                           = u16;
    using Map16                                                 = std::vector<Map16Tile>;
    using BehaviorTileID                                        = u16;
    using BehaviorSet                                           = std::vector<BehaviorTile>;
    using ObjectTileID                                          = u16;
    using ObjectRowID                                           = u16;
    using ObjectID                                              = u16;
    using ObjectSet                                             = std::vector<Object>;
    using TilesetID                                             = u8;


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
    inline constexpr size_t     Map16TileWidthTiles             = 2;
    inline constexpr size_t     Map16TileHeightTiles            = 2;
    inline constexpr size_t     Map16TileSizeTiles              = Map16TileWidthTiles * Map16TileHeightTiles;                  
    inline constexpr size_t     Map16TileSizeBytes              = Map16TileSizeTiles * Nitro::ScreenTileSizeBytes;

    inline constexpr size_t     BehaviorTileSizeBytes           = 4;
    
    inline constexpr size_t     ObjectTileSizeBytes             = 3;

    //
    // FUNCTIONS: READS
    //

    Map16Tile                   ReadMap16TileFromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    Map16                       ReadMap16FromBytes              (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    BehaviorTile                ReadBehaviorTileFromBytes       (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    BehaviorSet                 ReadBehaviorSetFromBytes        (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    
    ObjectTile                  ReadObjectTileFromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    ObjectRow                   ReadObjectRowFromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    Object                      ReadObjectFromBytes             (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    ObjectSet                   ReadObjectSetFromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    void                        WriteMap16TileToBytes           (const Map16Tile& map16_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    void                        WriteMap16ToBytes               (const Map16& map16, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    void                        WriteBehaviorTileToBytes        (const BehaviorTile& behav_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    void                        WriteBehaviorSetToBytes         (const BehaviorSet& behav_set, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    
    void                        WriteObjectTileToBytes          (const ObjectTile& obj_tile, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    void                        WriteObjectRowToBytes           (const ObjectRow& obj_row, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    void                        WriteObjectToBytes              (const Object& obj, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);
    void                        WriteObjectSetToBytes           (const ObjectSet& obj_set, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    //
    // STRUCTURES
    //

    struct Map16Tile
    {
        Map16TileID             id;

        Nitro::ScreenTile       scr_tiles[Map16TileWidthTiles][Map16TileHeightTiles];

        inline                  Map16Tile()                     {};
    };

    struct BehaviorTile
    {
        BehaviorTileID          id;

        inline                  BehaviorTile()                  {};
    };

    struct ObjectTile
    {
        ObjectTileID            id;

        inline                  ObjectTile()                    {};
    };

    struct ObjectRow
    {
        ObjectRowID             id;

        inline                  ObjectRow()                     {};
    };

    struct Object
    {
        ObjectID                id;

        inline                  Object()                        {};
    };

    struct Tileset
    {
        TilesetID               id;
        TilesetType             type;

        Nitro::CharacterBitmap  bitmap;
        Nitro::ExPalette        palette;
        Map16                   map16;
        BehaviorSet             behaviors;
        ObjectSet               objects;

        inline                  Tileset()                       {};
    };
}

namespace NSMB::Backgrounds
{
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
    // FUNCTIONS: READS
    //

    Background                  ReadBackgroundFromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    void                        WriteBackgroundToBytes          (const Background& bg, Core::ByteArray &bytes, Core::Addr32 Write_at = 0);

    //
    // STRUCTURES
    //

    struct Background
    {
        BackgroundID            id;
        BackgroundType          type;

        inline                  Background()                    {};
    };
};