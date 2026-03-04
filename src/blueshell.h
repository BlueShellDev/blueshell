#pragma once

#include <filesystem>
#include "core/systems/console/console.h"
#include "core/systems/render/render.h"
#include "core/systems/render/texture.h"
#include "core/systems/io/binary_file.h"

#include "nitro/filesystem/fnt.h"

#include "nsmb/tileset/tileset_definition.h"
#include "nsmb/backgrounds/background_definition.h"

#include "nsmb/tileset/tileset.h"
#include "nsmb/backgrounds/background.h"
#include "nsmb/area/area.h"

namespace BS
{
    struct BSContext // things the editor needs to access that probably shouldn't be global but also shouldn't be passed around to every function that needs it
    {
        std::filesystem::path basePath; // the base path for the game files, set by the user when they open a file dialog to load something, this is used to resolve relative paths for files loaded from the overlay and such, and is also used as the default path for file dialogs when opening/saving files
        BS::Nitro::FilenameTable internalROMFilenames;

        BS::IO::ByteArray overlay0; // ugh, this is gross but it is what it is for now (throws 200kb on the stack cutely)

        BS::NSMB::TilesetTableDefinition tilesetTableOffsets;
        BS::NSMB::TilesetIndex tilesetDefinitions; // these are loaded from the above offsets

        BS::NSMB::BackgroundTableDefinition topBGTableOffsets;
        BS::NSMB::BackgroundIndex topBGDefinitions; // these are loaded from the above offsets

        BS::NSMB::BackgroundTableDefinition btmBGTableOffsets;
        BS::NSMB::BackgroundIndex btmBGDefinitions; // these are loaded from the above offsets

        std::vector<NSMB::Tileset> loadedTilesets; // these are loaded from the tileset definitions when selected in the editor
        std::vector<NSMB::Area> loadedAreas;       // these are loaded when an area is opened in the editor, they are not necessarily tied to the tileset/background definitions in any way,
                                                   // but they will be eventually when the editor is more fully featured and can edit areas and their associated tilesets/backgrounds
    };
}