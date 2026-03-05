#include "ncg.h"
#include "core/io/binary_file.h"

BS::Nitro::Character BS::Nitro::ReadCharacterFromBytes(const BS::IO::ByteArray &ncgData, size_t readStartingAt)
{
    Character character;
    for (size_t row = 0; row < Character::HeightPixels; row++)
        for (size_t col = 0; col < Character::WidthPixels; col++)
            character.pixels[row][col] = ncgData.Read<ColorIndex>(readStartingAt + row * Character::WidthPixels + col);
    return character;
}

void BS::Nitro::WriteCharacterToBytes(BS::IO::ByteArray &ncgData, size_t writeStartingAt, const Character &character)
{
    for (size_t row = 0; row < Character::HeightPixels; row++)
        for (size_t col = 0; col < Character::WidthPixels; col++)
            ncgData.Write<ColorIndex>(writeStartingAt + row * Character::WidthPixels + col, character.pixels[row][col]);
}

BS::Nitro::CharacterSet BS::Nitro::ReadCharacterSetFromBytes(const BS::IO::ByteArray &ncgData)
{
    CharacterSet characterSet;
    for (size_t i = 0; i < ncgData.bytes.size(); i += Character::SizeBytes)
    {
        Character character;
        for (size_t row = 0; row < Character::HeightPixels; row++)
            for (size_t col = 0; col < Character::WidthPixels; col++)
                character.pixels[row][col] = ncgData.Read<ColorIndex>(i + row * Character::WidthPixels + col);

        characterSet.push_back(character);
    }
    return characterSet;
}

BS::Nitro::CharacterSet BS::Nitro::ReadCharacterSetFromFile(const std::string &filePath)
{
    BS::IO::ByteArray ncgData = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadCharacterSetFromBytes(ncgData);
}

void BS::Nitro::WriteCharacterSetToBytes(BS::IO::ByteArray &ncgData, const CharacterSet &characterSet)
{
    ncgData.bytes.clear();
    size_t writeTo = 0;
    for (size_t i = 0; i < characterSet.size(); i++)
        for (size_t row = 0; row < Character::HeightPixels; row++)
            for (size_t col = 0; col < Character::WidthPixels; col++)
                ncgData.Write<ColorIndex>(writeTo + row * Character::WidthPixels + col, (characterSet[i]).pixels[row][col]);
}

void BS::Nitro::WriteCharacterSetToFile(const std::string &filePath, const CharacterSet &characterSet)
{
}
