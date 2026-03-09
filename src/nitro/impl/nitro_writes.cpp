#include "nitro/simple_nitro.h"

namespace
{
    inline void EnsureByteCount(Core::ByteArray &bytes, size_t min_size)
    {
        if (bytes.bytes.size() < min_size)
            bytes.bytes.resize(min_size);
    }
}

void Nitro::FS::WriteFilePathTableToBytes(const Nitro::FS::FilePathTable& table, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}

void Nitro::WriteCharacterToBytes(const Nitro::Character& character, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + CharacterSizeBytes);

    for (size_t row = 0; row < CharacterHeightPixels; ++row)
        for (size_t col = 0; col < CharacterWidthPixels; ++col)
            bytes.Write<ColorID>(write_at + row * CharacterWidthPixels + col, character.pixels[row][col]);
}

void Nitro::WriteBitmapToBytes(const Nitro::CharacterBitmap& char_bmp, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}

void Nitro::WritePalette16ToBytes(const Nitro::Palette16& pal, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + Palette16SizeBytes);

    for (size_t i = 0; i < Palette16SizeColors; ++i)
    {
        //TrueBGRToNitroBGR(pal.colors[i]);
        //bytes.Write<Color>(write_at + i * sizeof(Color), pal.colors[i]);
        Core::Console::Global().Warn("just so you know, your palette16 edits didn't save. lol!");
    }
}

void Nitro::WritePalette256ToBytes(const Nitro::Palette256& pal, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + Palette256SizeBytes);

    for (size_t i = 0; i < Palette256SizeColors; ++i)
    {
        //TrueBGRToNitroBGR(pal.colors[i]);
        //bytes.Write<Color>(write_at + i * sizeof(Color), pal.colors[i]);
        Core::Console::Global().Warn("just so you know, your palette256 edits didn't save. lol!");
    }
}

void Nitro::WriteExPaletteToBytes(const Nitro::ExPalette& ex_pal, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + ExPaletteSizeBytes);
    for (size_t i = 0; i < ExPaletteMaxPaletteCount; ++i)
        WritePalette256ToBytes(ex_pal.pals[i], bytes, write_at + i * Palette256SizeBytes);
}

void Nitro::WriteScreenTileToBytes(const Nitro::ScreenTile& scr_tile, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + ScreenTileSizeBytes);

    u16 tile_value = 0;
    tile_value |= (scr_tile.characterID & 0x3FF);
    tile_value |= ((scr_tile.renderFlags & ScreenTileRenderFlag_FlipH) ? 1 : 0) << 10;
    tile_value |= ((scr_tile.renderFlags & ScreenTileRenderFlag_FlipV) ? 1 : 0) << 11;
    tile_value |= (scr_tile.paletteID & 0xF) << 12;
    bytes.Write<u16>(write_at, tile_value);
}

void Nitro::WriteScreenToBytes(const Nitro::Screen& scr, Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}
