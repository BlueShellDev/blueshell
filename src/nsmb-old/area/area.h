#pragma once

#include <vector>

#include "core/types.h"
#include "blocks/area_header.h"
#include "blocks/area_camera.h"
#include "blocks/area_screen.h"
#include "blocks/area_entrance.h"
#include "blocks/area_actor.h"
#include "blocks/area_view.h"
#include "blocks/area_zone.h"
#include "blocks/area_path.h"
#include "blocks/area_bgdat.h"

namespace BS
{
    namespace NSMB
    {
        enum class AreaBlockID : u32
        {
            Header,
            Camera,
            Background,
            Tileset,
            Foreground,
            Entrances,
            Actors,
            Views,
            Zones,
            ProgressPaths,
            Paths,
            ProgressPathNodes,
            PathNodes,
            ActorBanks
        };

        struct Area
        {
            AreaHeader header;                                 // block 0
            std::vector<NSMB::AreaCamera> cameras;             // block 1
            std::vector<NSMB::AreaScreen> screens;             // block 2,3,4
            std::vector<NSMB::AreaEntrance> entrances;         // block 5
            std::vector<NSMB::AreaActor> actors;               // block 6
            std::vector<NSMB::AreaView> views;                 // block 7
            std::vector<NSMB::AreaZone> zones;                 // block 8
            std::vector<NSMB::AreaPath> progressPaths;         // block 9
            std::vector<NSMB::AreaPath> paths;                 // block 10
            std::vector<NSMB::AreaPathNode> progressPathNodes; // block 11
            std::vector<NSMB::AreaPathNode> pathNodes;         // block 12
            std::vector<u8> actorBankData;                     // block 13
            std::vector<NSMB::AreaBGDat> bgdata;               // bgdat file data
        };
    }
}
