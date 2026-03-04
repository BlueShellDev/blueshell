#include "tileset_definition.h"
#include <format>

BS::NSMB::TilesetIndex BS::NSMB::ReadTilesetIndexFromOverlay0(const BS::IO::ByteArray &overlay0, const TilesetTableDefinition &definition)
{
    TilesetIndex tilesetIndex;
    if (overlay0.bytes.size() < 5 * definition.entryCount * sizeof(Nitro::ExFileID))
    {
        BS::Console::Global().Err("Overlay 0 byte array is too small to contain the expected number of tileset definitions! Expected at least {} bytes but got {} bytes.",
                                  5 * definition.entryCount * sizeof(Nitro::ExFileID), overlay0.bytes.size());
        return tilesetIndex;
    }

    for (size_t i = 0; i < definition.entryCount; ++i)
    {
        TilesetDefinition entry;
        entry.id = i;
        entry.characterFileID = overlay0.Read<Nitro::ExFileID>(definition.NCGBaseOffset + (i * sizeof(Nitro::ExFileID)));
        entry.paletteFileID = overlay0.Read<Nitro::ExFileID>(definition.NCLBaseOffset + (i * sizeof(Nitro::ExFileID)));
        entry.map16FileID = overlay0.Read<Nitro::ExFileID>(definition.PNLBaseOffset + (i * sizeof(Nitro::ExFileID)));
        entry.objectFileID = overlay0.Read<Nitro::ExFileID>(definition.UNTBaseOffset + (i * sizeof(Nitro::ExFileID)));
        entry.tileBehaviorFileID = overlay0.Read<Nitro::ExFileID>(definition.CHKBaseOffset + (i * sizeof(Nitro::ExFileID)));
        entry.name = std::format("Tileset {:02}", i);
        tilesetIndex.push_back(entry);
    }
    return tilesetIndex;
}

void BS::NSMB::WriteTilesetIndexToOverlay0(BS::IO::ByteArray &overlay0, const TilesetTableDefinition &definition, const TilesetIndex &tilesetIndex)
{
    for (const TilesetDefinition &entry : tilesetIndex)
    {
        overlay0.Write<Nitro::ExFileID>(definition.NCGBaseOffset + (entry.id * sizeof(Nitro::ExFileID)), entry.characterFileID);
        overlay0.Write<Nitro::ExFileID>(definition.NCLBaseOffset + (entry.id * sizeof(Nitro::ExFileID)), entry.paletteFileID);
        overlay0.Write<Nitro::ExFileID>(definition.PNLBaseOffset + (entry.id * sizeof(Nitro::ExFileID)), entry.map16FileID);
        overlay0.Write<Nitro::ExFileID>(definition.UNTBaseOffset + (entry.id * sizeof(Nitro::ExFileID)), entry.objectFileID);
        overlay0.Write<Nitro::ExFileID>(definition.CHKBaseOffset + (entry.id * sizeof(Nitro::ExFileID)), entry.tileBehaviorFileID);
    }
    BS::Console::Global().Log("Wrote {} bytes to overlay 0 byte array", tilesetIndex.size() * (sizeof(Nitro::ExFileID) * 5));
}
