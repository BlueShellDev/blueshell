#include "nitro/simple_nitro.h"

namespace
{
    inline void EnsureByteCount(Core::ByteArray &bytes, size_t min_size)
    {
        if (bytes.bytes.size() < min_size)
            bytes.bytes.resize(min_size);
    }
}

void Nitro::FS::WriteFilePathTableToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}

void Nitro::WriteCharacterToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + CharacterSizeBytes);
    const Character character{};

    for (size_t row = 0; row < CharacterHeightPixels; ++row)
        for (size_t col = 0; col < CharacterWidthPixels; ++col)
            bytes.Write<ColorID>(write_at + row * CharacterWidthPixels + col, character.pixels[row][col]);
}

void Nitro::WriteBitmapToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}

void Nitro::WritePalette16ToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + Palette16SizeBytes);
    const Palette16 palette{};

    for (size_t i = 0; i < Palette16SizeColors; ++i)
        bytes.Write<Color>(write_at + i * sizeof(Color), palette.colors[i]);
}

void Nitro::WritePalette256ToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + Palette256SizeBytes);
    const Palette256 palette{};

    for (size_t i = 0; i < Palette256SizeColors; ++i)
        bytes.Write<Color>(write_at + i * sizeof(Color), palette.colors[i]);
}

void Nitro::WriteExPaletteToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + ExPaletteSizeBytes);
    for (size_t i = 0; i < ExPaletteMaxPaletteCount; ++i)
        WritePalette256ToBytes(bytes, write_at + i * Palette256SizeBytes);
}

void Nitro::WriteScreenTileToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    EnsureByteCount(bytes, write_at + ScreenTileSizeBytes);

    const ScreenTile tile{};
    u16 tile_value = 0;
    tile_value |= (tile.characterID & 0x3FF);
    tile_value |= ((tile.renderFlags & ScreenTileRenderFlag_FlipH) ? 1 : 0) << 10;
    tile_value |= ((tile.renderFlags & ScreenTileRenderFlag_FlipV) ? 1 : 0) << 11;
    tile_value |= (tile.paletteID & 0xF) << 12;
    bytes.Write<u16>(write_at, tile_value);
}

void Nitro::WriteScreenToBytes(Core::ByteArray &bytes, Core::Addr32 write_at)
{
    (void)bytes;
    (void)write_at;
}
