#pragma once

// includes
#include <fstream>
#include <vector>
#include <array>
#include <string>

// local includes
#include "../util/util.hpp"

namespace Graphics
{
    struct Character : Util::Data
    {
        using PixelGrid = std::array<std::array<u8, 8>, 8>;

        PixelGrid pixels;

        void unpack(IO::ByteArray& in, sz_t& at);
        void pack(IO::ByteArray& out);
    };

    enum class PaletteType
    {
        STANDARD,
        EXTENDED
    };

    struct Palette : Util::Data
    {
        using PaletteVector = std::vector<u16>;

        PaletteVector   colors;
        PaletteType     type = PaletteType::STANDARD;

        void unpack(IO::ByteArray& in, sz_t& at);
        void pack(IO::ByteArray& out);
    };

    struct Map16Quarter : Util::Data
    {
        u8      palette_id      = 0x00;
        u16     character_id    = 0x0000;

        bool    v_flip          = false;
        bool    h_flip          = false;
    };

    struct Map16Tile : Util::Data
    {
        std::array<Map16Quarter, 4> quarters;

        void unpack(IO::ByteArray& in, sz_t& at);
        void pack(IO::ByteArray& out);
    };

    const enum class ObjectTileControlByte
    {
        NORMAL      = 0x00,
        REPEAT_X    = 0x01,
        REPEAT_Y    = 0x02,
        REPEAT_XY   = 0x03,
        SLOPE_THING = 0x04,
        NEW_LINE    = 0xFE,
    };

    const enum class ObjectRowControlByte
    {
        OBJECT_END          = 0xFF,
        SLOPE_NE            = 0x80,
        SLOPE_NW            = 0x81,
        SLOPE_SE            = 0x82,
        SLOPE_SW            = 0x83,
        SLOPE_CONTROL_E     = 0x84,
        SLOPE_CONTROL_F     = 0x85,
        SLOPE_CONTROL_G     = 0x86,
        SLOPE_CONTROL_H     = 0x87,
        SLOPE_CONTROL_I     = 0x88,
        SLOPE_CONTROL_J     = 0x89,
        SLOPE_CONTROL_NONE  = -1
    };

    struct ObjectTile : Util::Data
    {
        ObjectTileControlByte control;
        u16 map16_tile_offset = 0;

        void unpack(IO::ByteArray& in, sz_t& at, bool& hit_eor);
        void pack(IO::ByteArray& out);
    };

    struct ObjectRow : Util::Data
    {
        ObjectRowControlByte control;
        std::vector<ObjectTile> tiles;

        void unpack(IO::ByteArray& in, sz_t& at, bool& hit_eoo);
        void pack(IO::ByteArray& out);
    };

    struct Object : Util::Data
    {
        std::size_t width = 0, height = 0;
        std::vector<ObjectRow> rows;

        void unpack(IO::ByteArray& in, sz_t& at);
        void pack(IO::ByteArray& out);
    };
}