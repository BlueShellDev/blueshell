#pragma once
#include <string>
#include "core/io/byte_array.h"
#include "nitro/filesystem/fnt.h"

namespace BS
{
    namespace NSMB
    {
        enum class TilesetType
        {
            General,
            Main,
            LevelEnd,
        };

        struct TilesetTableDefinition
        {
            size_t entryCount{0};
            u32_address NCGBaseOffset{0x00030EA4};
            u32_address NCLBaseOffset{0x00031494};
            u32_address PNLBaseOffset{0x000316F4};
            u32_address UNTBaseOffset{0x0002FA14};
            u32_address CHKBaseOffset{0x0002FB44};
        };

        struct TilesetDefinition
        {
            size_t id{0};
            std::string name{""};

            Nitro::ExFileID characterFileID{0};
            Nitro::ExFileID paletteFileID{0};
            Nitro::ExFileID map16FileID{0};
            Nitro::ExFileID objectFileID{0};
            Nitro::ExFileID tileBehaviorFileID{0};
        };

        using TilesetIndex = std::vector<TilesetDefinition>;

        TilesetIndex ReadTilesetIndexFromOverlay0(const BS::IO::ByteArray &overlay0, const TilesetTableDefinition &definition);
        void WriteTilesetIndexToOverlay0(BS::IO::ByteArray &overlay0, const TilesetTableDefinition &definition, const TilesetIndex &tilesetIndex);
    };
}