#pragma once

#include "../util/byteio.hpp"

namespace Nitro
{
    struct ROM
    {
        u32 arm9_offset;
        u32 arm9_size;

        u32 fnt_offset;
        u32 fnt_size;

        u32 fat_offset;
        u32 fat_size;

        u32 arm9_table_offset;
        u32 arm9_table_size;
    };

    struct FileAllocationEntry
    {
        u32 start_addr;
        u32 end_addr;
    };

    struct FileAllocationTable
    {
        std::vector<FileAllocationEntry> entries;
    };

    struct FileMetaData
    {
        u16         id;
        std::string name;
    };

    struct Directory
    {
        u16 directory_id;
        u16 initial_file_id;
        u32 sub_fnt_offset;

        std::string name;
        std::vector<FileMetaData>   files;
        std::vector<Directory>      dirs;

        void unpack(IO::ByteArray& from, sz_t& at)
        {
            sub_fnt_offset      = IO::read<u32>(from, at);
            initial_file_id     = IO::read<u16>(from, at);
        }
    };

    // returns recursive directory of specified directory
    Directory parse_directory_filename_table
    (
        IO::ByteArray& fnt_bin,
        const u16& directory_id
    )
    {
        Directory root;
        root.directory_id = directory_id;
        sz_t file_id_iterator = 0;

        sz_t addr = (root.directory_id & 0xFFF) * 8;
        root.unpack(fnt_bin, addr);
        
        addr = root.sub_fnt_offset;
        while (addr < fnt_bin.size())
        {
            const u8& _entry_flag = IO::read<u8>(fnt_bin, addr);
            if (_entry_flag == 0x80)
            {
                Debug::log(std::format("Reserve flag hit while parsing directory {:X}", root.directory_id));
                break;
            }
            else 
            if (_entry_flag == 0x00)
            {
                break;
            }

            const bool         _entry_is_dir    = _entry_flag & 0x80;
            const u8           _entry_length    = _entry_flag & 0x7F;
            std::string        _entry_name      = "";

            // this is so lame, just make a read bytearray as string function :p
            for (sz_t i = 0; i < _entry_length; i++)
            {
                _entry_name += IO::read<char>(fnt_bin, addr);
            }

            if (!_entry_is_dir)
            {
                FileMetaData _meta_data;
                _meta_data.id   = root.initial_file_id + file_id_iterator;
                _meta_data.name = _entry_name;
                root.files.push_back(_meta_data);
                file_id_iterator++;
            }
            else
            {
                const u16 _subdir_id = IO::read<u16>(fnt_bin, addr);
                Directory _subdir = parse_directory_filename_table(fnt_bin, _subdir_id);
                _subdir.name = _entry_name;
                root.dirs.push_back(_subdir);
            }
        }

        return root;
    }

    bool search_dir(const Directory& root, const u16& file_id, std::string& return_name)
    {
        for (auto& file : root.files)
        {
            if (file.id == file_id)
            {
                return_name = file.name;
                return true;
            }
        }

        for (auto& dir : root.dirs)
        {
            if (search_dir(dir, file_id, return_name))
            {
                return true;
            }
        }

        return false;
    }

    FileAllocationTable load_fat
    (
        const IO::ByteArray& raw
    )
    {
        FileAllocationTable table;

        sz_t num_entries = raw.size() / sizeof(FileAllocationEntry);
        table.entries.reserve(num_entries);

        sz_t addr = 0;
        for (sz_t i = 0; i < num_entries; i++)
        {
            FileAllocationEntry entry;
            entry.start_addr    = IO::read<u32>(raw, addr);
            entry.end_addr      = IO::read<u32>(raw, addr);
            table.entries.push_back(entry);
        }

        return table;
    }
}