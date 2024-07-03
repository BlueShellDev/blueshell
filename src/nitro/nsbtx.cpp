#include "nsbtx.h"

namespace Nitro
{
    void TextureHeader::unpack(IO::ByteArray& from, sz_t& at)
    {
        tex_offset = IO::read<u16>(from, at) * 8;
        u16 packed_param = IO::read<u16>(from, at);
        format = (packed_param >> 10) & 0b111;
        width = 0b1000 << ((packed_param >> 4) & 0b111);
        height = 0b1000 << ((packed_param >> 7) & 0b111);
        color_0 = !((packed_param >> 13) & 0b001);
        unused = IO::read<u32>(from, at);
    }

    void NitroSubHeader::unpack(IO::ByteArray& from, sz_t& at)
    {
        dummy = IO::read<u8>(from, at);
        num_objects = IO::read<u8>(from, at);
        header_size = IO::read<u16>(from, at);

        unk_section_header_size = IO::read<u16>(from, at);
        unk_section_size = IO::read<u16>(from, at);
        unk_constant = IO::read<u32>(from, at);

        for (sz_t i = 0; i < num_objects; i++)
        {
            u32 unk_data = IO::read<u32>(from, at);
            unk_entries.push_back(unk_data);
        }

        data_info_entry_size = IO::read<u16>(from, at);
        data_info_section_size = IO::read<u16>(from, at);

        for (sz_t i = 0; i < num_objects; i++)
        {
            TextureHeader data;
            data.unpack(from, at);
            data_info.push_back(data);
        }

        for (sz_t i = 0; i < num_objects; i++)
        {
            std::string name;
            for (sz_t j = 0; j < 0x10; j++)
            {
                name += IO::read<char>(from, at);
            }
            object_names.push_back(name);
        }
    }
}