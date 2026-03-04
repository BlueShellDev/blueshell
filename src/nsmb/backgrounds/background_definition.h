#pragma once
#include <string>
#include "core/types.h"
#include "nitro/filesystem/fnt.h"

namespace BS
{
    namespace NSMB
    {
        enum class BackgroundType
        {
            BottomLayer,
            TopLayer
        };

        struct BackgroundTableDefinition
        {
            size_t entryCount{0};
            u32 NCGBaseOffset{0x00030EA4};
            u32 NCLBaseOffset{0x00031494};
            u32 NSCBaseOffset{0x000316F4};
        };

        struct BackgroundDefinition
        {
            size_t id{0};
            std::string name{""};
            Nitro::ExFileID characterFileID{0};
            Nitro::ExFileID paletteFileID{0};
            Nitro::ExFileID tilemapFileID{0};
        };

        using BackgroundIndex = std::vector<BackgroundDefinition>;

        BackgroundIndex ReadBackgroundIndexFromOverlay0(const BS::IO::ByteArray &overlay0, const BackgroundTableDefinition &definition);
        void WriteBackgroundIndexToOverlay0(BS::IO::ByteArray &overlay0, const BackgroundTableDefinition &definition, const BackgroundIndex &backgroundIndex);
    }
}