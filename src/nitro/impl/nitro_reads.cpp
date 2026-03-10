#include "nitro/simple_nitro.h"

// none of these do bounds checking!!

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
    {
        palette.colors[i] = bytes.Read<Color>(read_at + i * sizeof(Color));
        NitroBGRToTrueBGR(palette.colors[i]);
    }
    return palette;
}

Nitro::Palette256 Nitro::ReadPalette256FromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    Palette256 palette;
    for (size_t i = 0; i < Palette256SizeColors; ++i)
    {
        palette.colors[i] = bytes.Read<Color>(read_at + i * sizeof(Color));
        NitroBGRToTrueBGR(palette.colors[i]);
    }
    return palette;
}

Nitro::ExPalette Nitro::ReadExPaletteFromBytes(const Core::ByteArray &bytes, Core::Addr32 read_at)
{
    ExPalette ex_palette;
    for (size_t i = 0; i < ExPaletteMaxPaletteCount; ++i)
    {
        Palette256 sub_palette = ReadPalette256FromBytes(bytes, read_at + i * Palette256SizeBytes);
        sub_palette.id = i;
        ex_palette.pals.push_back(sub_palette);
    }
    return ex_palette;
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

Nitro::FS::FileNameTable Nitro::FS::ReadFileNameTableFromBytes(const Core::ByteArray &bytes, DirectoryID starting_dir)
{
    // this is a super bare bones scraper of the fnt; just getting filenames mapped to ids
    FileNameTable filetable;
    Core::Addr32 metadata_offset = TranslateDirectoryIDToOffset(starting_dir);

    using TableCtrl = u8;
    enum TableCtrlFlag : TableCtrl
    {
        Reserved = 0x00,
        EndOfTable = 0x80
    };

    Core::Addr32 table_offset = bytes.ReadEx<Core::Addr32>(metadata_offset);
    FileID first_file_id = bytes.ReadEx<FileID>(metadata_offset);
    while (table_offset < bytes.bytes.size())
    {
        TableCtrl ctrl = bytes.ReadEx<TableCtrl>(table_offset);
        // if the flag is ONLY 0x00 or 0x80 then its a special case, else treat as flag
        if (ctrl == TableCtrlFlag::Reserved)
            break;
        
        if (ctrl == TableCtrlFlag::EndOfTable)
            break;

        const bool is_dir = (ctrl & TableCtrlFlag::EndOfTable);
        // filenames can have 127 chars
        std::string name("", ctrl & 0x7F);
        for (char &ch : name)
            ch = (char)bytes.ReadEx<u8>(table_offset);
        
        if (!is_dir)
        {
            filetable.insert({first_file_id, name});
            first_file_id++;
        }
        else
        {
            FileNameTable sub_table = ReadFileNameTableFromBytes(bytes, bytes.ReadEx<DirectoryID>(table_offset));
            filetable.insert(sub_table.begin(), sub_table.end());
        }
    }

    return filetable;
}
