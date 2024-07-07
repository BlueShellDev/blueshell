#pragma once

#include "../util/util.hpp"

namespace Nitro
{
    struct NitroHeader
    {
        char identifier[4];
        u32 magic;
        u32 file_size;
        u16 header_size;
        u16 num_blocks;
    };

    struct TEX0Block
    {
        char identifier[4];
        u32 size;
        u32 padding_a;
        u16 tex_data_size;
        u16 tex_info_offset;
        u32 padding_b;
        u32 tex_data_offset;
        u32 padding_c;
        u16 cmp_tex_data_size;
        u16 cmp_tex_info_offset;
        u32 padding_d;
        u32 cmp_tex_data_offset;
        u32 cmp_text_info_data_offset;
        u32 padding_e;
        u32 pal_data_size;
        u32 pal_info_offset;
        u32 pal_data_offset;
    };

    struct PaletteHeader
    {
        u16 pal_offset;
        u16 unk;
    };

    struct TextureHeader
    {
        u16     tex_offset;
        u8      format;
        u8      width;
        u8      height;
        bool    color_0;
        u32     unused;

        void unpack(IO::ByteArray& from, sz_t& at);
    };

    struct TextureData
    {
        IO::ByteArray pixel_data;
    };

    struct NitroSubHeader
    {
        u8  dummy;
        u8  num_objects;
        u16 header_size;

        u16 unk_section_header_size;
        u16 unk_section_size;
        u32 unk_constant;
        std::vector<u32> unk_entries;

        u16 data_info_entry_size;
        u16 data_info_section_size;
        std::vector<TextureHeader> data_info;

        std::vector<std::string> object_names;

        void unpack(IO::ByteArray& from, sz_t& at);
    };

    struct TileNSBTXTableEntry
    {
        u32 i_file_id;
        u32 tex_id;
        u16 pal_id;
    };
}