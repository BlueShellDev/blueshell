#include "nitro/simple_nitro.h"


Nitro::Character Nitro::ReadCharacterFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Character character;
    for (size_t row = 0; row < CharacterHeightPixels; ++row)
        for (size_t col = 0; col < CharacterWidthPixels; ++col)
            character.pixels[row][col] = bytes.Read<ColorID>(read_at + row * CharacterWidthPixels + col);
    return character;
}

Nitro::CharacterBitmap Nitro::ReadBitmapFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    CharacterBitmap bitmap;
    Core::Addr32 offset = read_at;
    CharacterID id = 0;
    while (offset + CharacterSizeBytes <= bytes.bytes.size())
    {
        Character character = ReadCharacterFromBytes(bytes, offset);
        character.id = id++;
        bitmap.chars.push_back(character);
        offset += CharacterSizeBytes;
    }
    return bitmap;
}

Nitro::Palette16 Nitro::ReadPalette16FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Palette16 palette;
    for (size_t i = 0; i < Palette16SizeColors; ++i)
        palette.colors[i] = bytes.Read<Color>(read_at + i * sizeof(Color));
    return palette;
}

Nitro::Palette256 Nitro::ReadPalette256FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Palette256 palette;
    for (size_t i = 0; i < Palette256SizeColors; ++i)
        palette.colors[i] = bytes.Read<Color>(read_at + i * sizeof(Color));
    return palette;
}

Nitro::ExPalette Nitro::ReadExPaletteFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    ExPalette palette;
    for (size_t i = 0; i < ExPaletteMaxPaletteCount; ++i)
    {
        palette.palettes[i] = ReadPalette256FromBytes(bytes, read_at + i * Palette256SizeBytes);
        palette.palettes[i].id = i;
    }
    return palette;
}

Nitro::ScreenTile Nitro::ReadScreenTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    const u16 tile_value = bytes.Read<u16>(read_at);

    ScreenTile tile;
    tile.characterID = tile_value & 0x3FF;
    tile.renderFlags = static_cast<ScreenTileRenderFlag>((tile_value >> 10) & 0x3);
    tile.paletteID = (tile_value >> 12) & 0xF;
    return tile;
}

Nitro::Screen Nitro::ReadScreenFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Screen screen;
    Core::Addr32 offset = read_at;
    ScreenTileID id = 0;
    while (offset + ScreenTileSizeBytes <= bytes.bytes.size())
    {
        ScreenTile tile = ReadScreenTileFromBytes(bytes, offset);
        tile.id = id++;
        screen.tiles.push_back(tile);
        offset += ScreenTileSizeBytes;
    }
    return screen;
}

Nitro::FS::FilePathTable Nitro::FS::ReadFilePathTableFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    (void)bytes;
    (void)read_at;
    return {};
}
