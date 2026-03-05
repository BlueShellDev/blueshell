#include "nitro/simple_nitro.h"


Nitro::Character Nitro::ReadCharacterFromBytes(const Core::ByteArray & bytes, Core::Addr32 read_at)
{
    return Character();
}

Nitro::CharacterBitmap Nitro::ReadBitmapFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return CharacterBitmap();
}

Nitro::Palette16 Nitro::ReadPalette16FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return Palette16();
}

Nitro::Palette256 Nitro::ReadPalette256FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return Palette256();
}

Nitro::ExPalette Nitro::ReadExPaletteFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return ExPalette();
}

Nitro::ScreenTile Nitro::ReadScreenTileFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return ScreenTile();
}

Nitro::Screen Nitro::ReadScreenFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    return Screen();
}