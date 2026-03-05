// single-header dead simple nitro lib (blueshell '26)
#pragma once

#include <unordered_map>
#include <vector>
#include "core/io/byte_array.h"
#include "core/types.h"

namespace Nitro
{
    //
    // TYPES
    //

    struct Character;
    struct CharacterBitmap;
    struct Palette16;
    struct Palette256;
    struct ExPalette;
    struct ScreenTile;
    struct Screen;

    using Color                                                 = u16; // bgr565 format
    using ColorID                                               = u8;
    using CharacterID                                           = u16; // actual size: 10 bits
    using PaletteID                                             = u8;    // actual size: 4 bits
    using ScreenTileID                                          = u16;

    enum ScreenTileRenderFlag : u8 // actual size: 2 bits
    {
        ScreenTileRenderFlag_FlipH = 0b01,
        ScreenTileRenderFlag_FlipV = 0b10
    };

    //
    // CONSTANTS
    //

    inline constexpr size_t     CharacterWidthPixels            = 8;
    inline constexpr size_t     CharacterHeightPixels           = 8;
    inline constexpr size_t     CharacterSizeBytes              = CharacterWidthPixels * CharacterHeightPixels * sizeof(ColorID);
    inline constexpr size_t     Palette16SizeColors             = 16;
    inline constexpr size_t     Palette16SizeBytes              = Palette16SizeColors * sizeof(Color);
    inline constexpr size_t     Palette256SizeColors            = 256;
    inline constexpr size_t     Palette256SizeBytes             = Palette256SizeColors * sizeof(Color);
    inline constexpr size_t     ExPaletteMaxPaletteCount        = 16;
    inline constexpr size_t     ExPaletteSizeBytes              = ExPaletteMaxPaletteCount * Palette256SizeBytes;
    inline constexpr size_t     ScreenTileSizeBytes             = 2;

    //
    // FUNCTIONS: READS
    //

    inline Character            ReadCharacterFromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline CharacterBitmap      ReadBitmapFromBytes             (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    inline Palette16            ReadPalette16FromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline Palette256           ReadPalette256FromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline ExPalette            ReadExPaletteFromBytes          (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    inline ScreenTile           ReadScreenTileFromBytes         (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);
    inline Screen               ReadScreenFromBytes             (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    inline void                 WriteCharacterToBytes           (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    inline void                 WriteBitmapToBytes              (Core::ByteArray &bytes, Core::Addr32 write_at = 0);

    inline void                 WritePalette16ToBytes           (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    inline void                 WritePalette256ToBytes          (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    inline void                 WriteExPaletteToBytes           (Core::ByteArray &bytes, Core::Addr32 write_at = 0);

    inline void                 WriteScreenTileToBytes          (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
    inline void                 WriteScreenToBytes              (Core::ByteArray &bytes, Core::Addr32 write_at = 0);

    //
    // FUNCTIONS: UTILITY
    //

    // ehhhh i dont like the semantics of this function
    inline void                 NitroBGRToTrueBGR(const Color &colorToTransform);

    //
    // STRUCTURES
    //

    struct Character
    {
        CharacterID             id;

        ColorID                 pixels[CharacterWidthPixels][CharacterHeightPixels];

        inline                  Character()                     { id = 0; };
        inline                  Character(const CharacterID &char_id) : id(char_id) {};
    };

    // i should prob implement direct bitmaps too
    struct CharacterBitmap
    {
        std::vector<Character>  chars;

        inline                  CharacterBitmap()               {};
    };

    struct Palette16
    {
        PaletteID               id;

        Color                   colors[Palette16SizeColors];

        inline                  Palette16()                     { id = 0; };
        inline                  Palette16(const PaletteID &pal_id) : id(pal_id) {};
    };

    struct Palette256
    {
        PaletteID               id;

        Color                   colors[Palette256SizeColors];

        inline                  Palette256()                    { id = 0; };
        inline                  Palette256(const PaletteID &pal_id) : id(pal_id)    {};
    };

    struct ExPalette
    {
        Palette256              palettes[ExPaletteMaxPaletteCount];

        inline                  ExPalette()                     {};
    };

    struct ScreenTile
    {
        ScreenTileID            id;

        CharacterID             characterID;
        ScreenTileRenderFlag    renderFlags;
        PaletteID               paletteID;

        inline                  ScreenTile()                    {};
        inline                  ScreenTile(const CharacterID &char_id, const PaletteID &pal_id, ScreenTileRenderFlag flags) : 
                                            characterID(char_id), renderFlags(flags), paletteID(pal_id) {};
    };

    struct Screen
    {
        std::vector<ScreenTile> tiles;

        inline                  Screen()                        {};
    };
};

namespace Nitro::FS
{
    //
    // TYPES
    //

    using FileAddr                                              = u32;
    using FileID                                                = u16;
    using ExFileID                                              = u32;
    using FilePath                                              = std::string;
    using FilePathTable                                         = std::unordered_map<FileID, FilePath>;

    //
    // FUNCTIONS: READS
    //

    inline FilePathTable        ReadFilePathTableFromBytes      (const Core::ByteArray &bytes, Core::Addr32 read_at = 0);

    //
    // FUNCTIONS: WRITES
    //

    inline void                 WriteFilePathTableToBytes       (Core::ByteArray &bytes, Core::Addr32 write_at = 0);
};
