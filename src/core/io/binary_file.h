#pragma once
#include <filesystem>
#include "core/types.h"
#include "core/io/byte_array.h"

// this is dumb, not even endian support
namespace Core
{
    ByteArray ReadByteArrayFromFile(const std::filesystem::path &filePath);
    void WriteByteArrayToFile(const std::filesystem::path &filePath, const ByteArray &data);
};
