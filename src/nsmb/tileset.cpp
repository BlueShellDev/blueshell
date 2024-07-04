#include "tileset.h"

NSMB::Tileset NSMB::load_tileset
(
    NSMB::TilesetType_ type, 
    IO::ByteArray& ncl_raw,
    IO::ByteArray& ncg_raw,
    IO::ByteArray& pnl_raw,
    IO::ByteArray& unt_raw
)
{
    NSMB::Tileset tileset;
    tileset.type = type;

    IO::ByteArray decomp;

    decomp = LZ::decompress(ncg_raw);
    NSMB::unpack_characters
    (
        tileset.characters,
        decomp
    );

    decomp = LZ::decompress(ncl_raw);
    NSMB::unpack_palettes
    (
        tileset.palettes,
        decomp
    );

    NSMB::unpack_metatiles
    (
        tileset.metatiles,
        pnl_raw
    );

    NSMB::unpack_objects
    (
        tileset.objects,
        unt_raw
    );

    return tileset;
};

NSMB::Tileset
NSMB::load_tileset
(
    NSMB::TilesetType_ type,
    const std::filesystem::path& ncg_path,
    const std::filesystem::path& ncl_path,
    const std::filesystem::path& pnl_path,
    const std::filesystem::path& unt_path
)
{
    NSMB::Tileset tileset;
    tileset.type = type;

    std::ifstream in;
    IO::ByteArray raw;
    IO::ByteArray decomp;

    in = Util::begin_ifstream(ncg_path);
    raw = IO::bytes_from_file(in);
    decomp = LZ::decompress(raw);

    in.close();
    NSMB::unpack_characters
    (
        tileset.characters, 
        decomp
    );

    in = Util::begin_ifstream(ncl_path);
    raw = IO::bytes_from_file(in);
    decomp = LZ::decompress(raw);
    in.close();
    NSMB::unpack_palettes
    (
        tileset.palettes, 
        decomp
    );

    in = Util::begin_ifstream(pnl_path);
    raw = IO::bytes_from_file(in);
    in.close();
    NSMB::unpack_metatiles
    (
        tileset.metatiles,
        raw
    );

    in = Util::begin_ifstream(unt_path);
    raw = IO::bytes_from_file(in);
    in.close();
    NSMB::unpack_objects
    (
        tileset.objects, 
        raw
    );

    return tileset;
}

void 
NSMB::unpack_characters
(
    std::vector<Graphics::Character>& container,
    IO::ByteArray& raw_data
)
{
    sz_t addr = 0, id = 0;
    while (addr < raw_data.size())
    {
        Graphics::Character character;
        character._from = addr;
        character._id = id++;
        character.unpack(raw_data, addr);
        container.emplace_back(character);
    }
}

void
NSMB::unpack_palettes
(
    std::vector<Graphics::Palette>& container,
    IO::ByteArray& raw_data
)
{
    sz_t addr = 0, id = 0;
    while (addr < raw_data.size())
    {
        Graphics::Palette palette;
        palette.type = Graphics::PaletteType::EXTENDED;
        palette._from = addr;
        palette._id = id++;
        palette.unpack(raw_data, addr);
        container.emplace_back(palette);
    }
}

void
NSMB::unpack_metatiles
(
    std::vector<Graphics::Map16Tile>& container,
    IO::ByteArray& raw_data
)
{
    sz_t addr = 0, id = 0;
    while (addr < raw_data.size())
    {
        Graphics::Map16Tile metatile;
        metatile._from = addr;
        metatile._id = id++;
        metatile.unpack(raw_data, addr);
        container.emplace_back(metatile);
    }
}

void
NSMB::unpack_objects
(
    std::vector<Graphics::Object>& container,
    IO::ByteArray& raw_data
)
{
    sz_t addr = 0, id = 0;
    while (addr < raw_data.size())
    {
        Graphics::Object object;
        object._from = addr;
        object._id = id++;
        object.unpack(raw_data, addr);
        container.emplace_back(object);
    }
}