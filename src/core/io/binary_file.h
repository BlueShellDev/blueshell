#pragma once
#include <string>
#include "core/types.h"
#include "core/io/byte_array.h"

// this is dumb, not even endian support
namespace Core
{
    ByteArray ReadByteArrayFromFile(const std::string &filePath);
    void WriteByteArrayToFile(const std::string &filePath, const ByteArray &data);
};
