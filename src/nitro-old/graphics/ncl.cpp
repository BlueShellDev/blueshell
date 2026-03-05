#include "ncl.h"
#include "core/io/binary_file.h"

BS::Nitro::Palette BS::Nitro::ReadPaletteFromBytes(const BS::IO::ByteArray &nclData)
{
    Palette palette;
    for (size_t i = 0; i < Palette::ColorsCount; i++)
        palette.colors[i] = nclData.Read<BGR565>(i * sizeof(BGR565));
    return palette;
}

BS::Nitro::Palette BS::Nitro::ReadPaletteFromFile(const std::string &filePath)
{
    BS::IO::ByteArray nclData = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadPaletteFromBytes(nclData);
}

BS::Nitro::ExtendedPalette BS::Nitro::ReadExPaletteFromBytes(const BS::IO::ByteArray &nclData)
{
    ExtendedPalette extendedPalette;
    size_t offset = 0;
    while (offset < nclData.bytes.size())
    {
        Palette palette;
        for (size_t i = 0; i < Palette::ColorsCount; i++)
            palette.colors[i] = nclData.Read<BGR565>(offset + i * sizeof(BGR565));
        extendedPalette.subPalettes.push_back(palette);
        offset += Palette::SizeBytes;
    }
    return extendedPalette;
}

BS::Nitro::ExtendedPalette BS::Nitro::ReadExPaletteFromFile(const std::string &filePath)
{
    BS::IO::ByteArray nclData = BS::IO::ReadByteArrayFromFile(filePath);
    return ReadExPaletteFromBytes(nclData);
}

void BS::Nitro::WritePaletteToBytes(BS::IO::ByteArray &nclData, const Palette &palette)
{
    nclData.bytes.clear();
    for (size_t i = 0; i < Palette::ColorsCount; i++)
        nclData.Write<BGR565>(i * sizeof(BGR565), palette.colors[i]);
}

void BS::Nitro::WritePaletteToFile(const std::string &filePath, const Palette &palette)
{
    BS::IO::ByteArray nclData;
    WritePaletteToBytes(nclData, palette);
    BS::IO::WriteByteArrayToFile(filePath, nclData);
}

void BS::Nitro::WriteExPaletteToBytes(BS::IO::ByteArray &nclData, const ExtendedPalette &palette)
{
    nclData.bytes.clear();
    size_t offset = 0;
    for (const Palette &subPalette : palette.subPalettes)
    {
        for (size_t i = 0; i < Palette::ColorsCount; i++)
            nclData.Write<BGR565>(offset + i * sizeof(BGR565), subPalette.colors[i]);
        offset += Palette::SizeBytes;
    }
}

void BS::Nitro::WriteExPaletteToFile(const std::string &filePath, const ExtendedPalette &palette)
{
    BS::IO::ByteArray nclData;
    WriteExPaletteToBytes(nclData, palette);
    BS::IO::WriteByteArrayToFile(filePath, nclData);
}

void BS::Nitro::NitroBGRToTrueBGR(BGR565 &colorToTransform)
{
    u16 lowest_green_bit = (colorToTransform & (0x1 << 15)) >> 15;
    u16 blue_ch = (colorToTransform & (0x1F << 10)) >> 10;
    u16 green_ch = (((colorToTransform & (0x1F << 5)) >> 5) << 1) | lowest_green_bit;
    u16 red_ch = colorToTransform & 0x1F;
    colorToTransform = (blue_ch << 11) | (green_ch << 5) | red_ch; // BBBBB GGGGGG RRRRR
}
