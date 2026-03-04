#pragma once
#include <string>
#include <vector>
#include "core/types.h"
#include "core/systems/io/byte_array.h"

namespace BS
{
    namespace IO
    {
        ByteArray ReadByteArrayFromFile(const std::string &filePath);
        void WriteByteArrayToFile(const std::string &filePath, const ByteArray &data);
    };
}