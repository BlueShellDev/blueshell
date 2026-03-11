#pragma once

#include <string>
#include "nitro/simple_nitro.h"
#include "nsmb/simple_nsmb.h"

/*
{0x2F8E4}, //TS_UNT_HD
{0x2FA14}, //TS_UNT
{0x2FB44}, //TS_CHK
{0x2FC74}, //TS_ANIM_NCG
{0x30D74}, //BG_NCG
{0x30EA4}, //TS_NCG
{0x30FD4}, //FG_NCG
{0x31104}, //FG_NSC
{0x31234}, //BG_NSC
{0x31364}, //BG_NCL
{0x31494}, //TS_NCL
{0x315C4}, //FG_NCL
{0x316F4}, //TS_PNL
{0x30CD8}, //Jyotyu_NCL
{0x2FDA4}, //Jyotyu_CHK
{0x2C930}, //Modifiers
{0x29BD8}, //Sprite Class IDs
{0x2CBBC}, //Sprite Data
*/

namespace BS::Game
{
    inline constexpr size_t MaxTilesetSlots     = 76;
    inline constexpr size_t MaxBackgroundSlots  = 76;

    struct BackgroundSlot
    {
        NSMB::Backgrounds::BackgroundID id;

        Nitro::FS::FileID bitmapFileID;
        Nitro::FS::FileID paletteFileID;
        Nitro::FS::FileID screenFileID;
    };

    struct TilesetSlot
    {
        NSMB::Tilesets::TilesetID id;

        Nitro::FS::FileID bitmapFileID;
        Nitro::FS::FileID paletteFileID;
        Nitro::FS::FileID map16FileID;
        Nitro::FS::FileID objectFileID;
        Nitro::FS::FileID behaviorFileID;
    };

    using BackgroundTable = std::array<BackgroundSlot, MaxBackgroundSlots>;
    using TilesetTable = std::array<TilesetSlot, MaxBackgroundSlots>;

    struct GameInternal
    {
        Nitro::FS::FileNameTable filenames;

        BackgroundTable topBackgrounds;
        BackgroundTable btmBackgrounds;

        TilesetTable tilesets;
    };

    // will house functions like
    // LoadArea, LoadTileset, LoadBackground
};