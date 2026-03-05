#pragma once
#include <array>
#include <vector>
#include "core/types.h"
#include "core/io/byte_array.h"

namespace BS
{
    namespace Nitro
    {
        using ColorIndex = u8;

        struct Character
        {
            static constexpr size_t HeightPixels = 8;
            static constexpr size_t WidthPixels = 8;
            static constexpr size_t SizeBytes = HeightPixels * WidthPixels * sizeof(ColorIndex);

            size_t id{0};
            std::array<std::array<ColorIndex, WidthPixels>, HeightPixels> pixels;
        };

        using CharacterSet = std::vector<Character>;

        Character ReadCharacterFromBytes(const BS::IO::ByteArray &ncgData, size_t readStartingAt);
        CharacterSet ReadCharacterSetFromBytes(const BS::IO::ByteArray &ncgData);
        CharacterSet ReadCharacterSetFromFile(const std::string &filePath);

        void WriteCharacterToBytes(BS::IO::ByteArray &ncgData, size_t writeStartingAt, const Character &character);
        void WriteCharacterSetToBytes(BS::IO::ByteArray &ncgData, const CharacterSet &characterSet);
        void WriteCharacterSetToFile(const std::string &filePath, const CharacterSet &characterSet);
    };
}