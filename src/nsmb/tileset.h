#pragma once

#include "../util/lz.hpp"
#include "../graphics/graphics.h"

namespace NSMB
{
    enum TilesetType_
    {
       TilesetType_Jyotyu,
       TilesetType_Main,
       TilesetType_Sub
    };

    struct Tileset
    {
        sz_t            id      = 0;
        TilesetType_    type    = TilesetType_Jyotyu;

        std::vector<Graphics::Character>    characters;
        std::vector<Graphics::Palette>      palettes;
        std::vector<Graphics::Map16Tile>    metatiles;
        std::vector<Graphics::Object>       objects;

        bool use_overrides = false;
    };

    Tileset load_tileset_from_raw
    (
        TilesetType_ type,
        IO::ByteArray& ncl_raw,
        IO::ByteArray& ncg_raw,
        IO::ByteArray& pnl_raw,
        IO::ByteArray& unt_raw
    );

    Tileset load_tileset_from_disk
    (
        TilesetType_ type,
        const std::filesystem::path& ncg_path,
        const std::filesystem::path& ncl_path,
        const std::filesystem::path& pnl_path,
        const std::filesystem::path& unt_path
    );

    void
    unpack_characters
    (
        std::vector<Graphics::Character>& container,
        IO::ByteArray& raw_data
    );
    
    void 
    unpack_palettes
    (
        std::vector<Graphics::Palette>& container,
        IO::ByteArray& raw_data
    );

    void
    unpack_metatiles
    (
        std::vector<Graphics::Map16Tile>&   container,
        IO::ByteArray&                      raw_data
    );

    void
    unpack_objects
    (
        std::vector<Graphics::Object>& container,
        IO::ByteArray& raw_data
    );
}