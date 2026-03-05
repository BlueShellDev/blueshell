#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "core/types.h"
#include "core/io/byte_array.h"

namespace BS
{
    namespace Nitro
    {
        using FileID = u16;
        using ExFileID = u32;
        using FilenameTable = std::unordered_map<FileID, std::string>;

        FilenameTable ReadFNTFromBytes(const BS::IO::ByteArray &fntData);
        FilenameTable ReadFNTFromFile(const BS::IO::ByteArray &fntData);
        void WriteFNTToBytes(BS::IO::ByteArray &fntData, const FilenameTable &filenameTable);
        void WriteFNTToFile(const std::string &filePath, const FilenameTable &filenameTable);
    };
}