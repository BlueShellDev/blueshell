#include "graphics.h"

/// ================ CHARACTER ============== ///

void Graphics::Character::unpack(IO::ByteArray& in, sz_t& at)
{
    for (auto& row : pixels)
        for (auto& data : row)
            data = IO::read<u8>(in, at);
}

void Graphics::Character::pack(IO::ByteArray& out)
{
}

/// ================ PALETTE ================ ///

void Graphics::Palette::unpack(IO::ByteArray& in, sz_t& at)
{
    switch (type)
    {
    case Graphics::PaletteType::STANDARD:
        colors.resize(16);
        break;
    case Graphics::PaletteType::EXTENDED:
        colors.resize(256);
        break;
    default:
        // revert to standard if non specified
        return;
    }

    for (u16& color : colors)
    {
        color = IO::read<u16>(in, at) ^ (1 << 15);
    }
    colors[0] ^= 1 << 15;
}

void Graphics::Palette::pack(IO::ByteArray& out)
{

}

/// ================ MAP16 ================== ///

void Graphics::Map16Tile::unpack
(
    IO::ByteArray&  in, 
    sz_t&           at
)
{
    sz_t id = 0;
    for (Map16Quarter& quarter : quarters)
    {
        u16 packed              = IO::read<u16>(in, at);

        quarter.character_id    = packed            & 0x3FF;
        quarter.palette_id      = (packed >> 12)    & 0xF;
        quarter.h_flip          = packed            & 0x400;
        quarter.v_flip          = packed            & 0x800;

        quarter._id             = id++;
        quarter._from           = at;
    }
}

void Graphics::Map16Tile::pack(IO::ByteArray& out)
{
}

/// ================ OBJECTS ================= ///

void Graphics::ObjectTile::unpack(IO::ByteArray& in, sz_t& at, bool& hit_eor)
{
    u8 first_byte = 0x00;
    u16 first_short = 0x0000;

    // check first byte
    first_byte = IO::read<u8>(in, at);
    ObjectTileControlByte first_control = static_cast<ObjectTileControlByte> (first_byte);

    switch (first_control)
    {
    case Graphics::ObjectTileControlByte::NORMAL:
        break;
    case Graphics::ObjectTileControlByte::REPEAT_X:
        break;
    case Graphics::ObjectTileControlByte::REPEAT_Y:
        break;
    case Graphics::ObjectTileControlByte::REPEAT_XY:
        break;
    case Graphics::ObjectTileControlByte::SLOPE_THING:
        break;
    case Graphics::ObjectTileControlByte::NEW_LINE:
        // not tile, break and return 
        hit_eor = true;
        return;
    default:
        break;
    }

    // all good, is tile
    control = first_control;

    first_short = IO::read<u16>(in, at);
    map16_tile_offset = first_short;
}

void Graphics::ObjectTile::pack(IO::ByteArray& out)
{
}

void Graphics::ObjectRow::unpack(IO::ByteArray& in, sz_t& at, bool& hit_eoo)
{
    bool _row_over = false;

    // check for slope byte first
    u8 first_byte = IO::read<u8>(in, at);
    ObjectRowControlByte first_control = static_cast<ObjectRowControlByte> (first_byte);

    switch (first_control)
    {
    case ObjectRowControlByte::SLOPE_NE:
        control = first_control;
        break;
    case ObjectRowControlByte::SLOPE_NW:
        control = first_control;
        break;
    case ObjectRowControlByte::SLOPE_SE:
        control = first_control;
        break;
    case ObjectRowControlByte::SLOPE_SW:
        control = first_control;
        break;
    case ObjectRowControlByte::SLOPE_CONTROL_E:
        control = first_control;
        break;
    case ObjectRowControlByte::SLOPE_CONTROL_F:
        control = first_control;
        break;
    case ObjectRowControlByte::OBJECT_END:
        // hit FF, return
        hit_eoo = true;
        return;
    default:
        control = ObjectRowControlByte::SLOPE_CONTROL_NONE;
        // go back one in stream, since there is no slope control offset
        at--;
        break;
    }

    size_t id = 0;
    while (!_row_over)
    {
        ObjectTile tile;
        tile._id = id;
        tile.unpack(in, at, _row_over);
        if (!_row_over)
        {
            tiles.emplace_back(tile);
        }
        id++;
    }

    return;
}

void Graphics::ObjectRow::pack(IO::ByteArray& out)
{
}

void Graphics::Object::unpack(IO::ByteArray& in, sz_t& at)
{
    bool _object_over = false;
    // store the old width so we can compute the max between it and the current row
    size_t old_width = 0;

    size_t id = 0;
    while (!_object_over)
    {
        ObjectRow row;
        row._id = id;
        row.unpack(in, at, _object_over);
        if (!_object_over)
        {
            // update object dimensions
            width = std::max(old_width, row.tiles.size());
            height++;
            old_width = width;

            // add to rows
            rows.emplace_back(row);
        }
        id++;
    }

    // check for slope control
    switch (rows.at(0).control)
    {
    case Graphics::ObjectRowControlByte::SLOPE_CONTROL_NONE:
        // continue with standard object 
        break;
    default:
        width *= 2;
        //height *= 2;
        break;
    }

    return;
}

void Graphics::Object::pack(IO::ByteArray& out)
{
}
