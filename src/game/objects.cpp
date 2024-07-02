#include "objects.h"

using namespace NSMB;

/* bgdat */

void
LevelComponents::BGDatObject::unpack
(
    IO::ByteArray& from, 
    sz_t& at
)
{
    item_type = UI::AreaItemType::AreaItemType_BgDat;

    u16 packed = IO::read<u16>(from, at);

    UI::PropertyEntry object_id;
    object_id.display_name = "Object ID";
    object_id.type = ImGuiDataType_U16;
    object_id.value = packed & 0xFFF;

    UI::PropertyEntry tileset_id;
    tileset_id.display_name = "Tileset ID";
    tileset_id.type = ImGuiDataType_U8;
    tileset_id.value = (packed >> 12) & 0xF;

    properties.entries.push_back(object_id);
    properties.entries.push_back(tileset_id);

    transform.x = IO::read<u16>(from, at);
    transform.y = IO::read<u16>(from, at);
    transform.w = IO::read<u16>(from, at);
    transform.h = IO::read<u16>(from, at);
    prev_transform = transform;

    resizeable = true;
}

void 
LevelComponents::BGDatObject::pack
(
    IO::ByteArray& to, 
    sz_t& at
)
{
    const u16 packed = (properties.entries.at(0).value | ( (u16)properties.entries.at(1).value << 12 ));

    IO::write(packed, to, at);
    IO::write(transform.x, to, at);
    IO::write(transform.y, to, at);
    IO::write(transform.w, to, at);
    IO::write(transform.h, to, at);
}

/* stage obj */

void
LevelComponents::StageObject::unpack
(
    IO::ByteArray& from,
    sz_t& at
)
{   
    item_type = UI::AreaItemType::AreaItemType_StageObject;

    UI::PropertyEntry stage_obj_id;
    stage_obj_id.display_name = "Sprite Type";
    stage_obj_id.type = ImGuiDataType_U16;
    stage_obj_id.value = IO::read<u16>(from, at);

    properties.entries.push_back(stage_obj_id);

    transform.x         = IO::read<u16>(from, at);
    transform.y         = IO::read<u16>(from, at);
    transform.w         = 1;
    transform.h         = 1;
    prev_transform = transform;
    
    data.reserve(6);
    for (sz_t i = 0; i < 6; i++)
    {
        data.push_back(IO::read<u8>(from, at));
    }
}

void
LevelComponents::StageObject::pack
(
    IO::ByteArray& to,
    sz_t& at
)
{
}

/* entrance */

void
LevelComponents::Entrance::unpack
(
    IO::ByteArray& from,
    sz_t& at
)
{
    item_type = UI::AreaItemType::AreaItemType_Entrance;

    transform.pixels_per_unit = { 1, 1 };
    transform.x               = IO::read<u16>(from, at);
    transform.y               = IO::read<u16>(from, at);
    transform.w = 16;
    transform.h = 16;
    prev_transform = transform;

    UI::PropertyEntry entry;
    entry.display_name = "Camera X";
    entry.type = ImGuiDataType_U16;
    entry.value = IO::read<u16>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Camera Y";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u16>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Entrance ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Destination Area";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Connected Pipe ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    at++;

    entry.display_name = "Destination Entrance ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    at++;

    entry.display_name = "Entrance Type";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Entrance Settings";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Unknown 0";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    at++;

    entry.display_name = "Is Zone";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Unknown 1";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);
}

void
LevelComponents::Entrance::pack
(
    IO::ByteArray& to,
    sz_t& at
)
{
}

/* view */

void
LevelComponents::View::unpack
(
    IO::ByteArray& from,
    sz_t& at
)
{
    item_type = UI::AreaItemType::AreaItemType_View;

    transform.pixels_per_unit = { 1, 1 };
    transform.x           = IO::read<u16>(from, at);
    transform.y           = IO::read<u16>(from, at);
    transform.w           = IO::read<u16>(from, at);
    transform.h           = IO::read<u16>(from, at);
    prev_transform = transform;

    UI::PropertyEntry entry;
    entry.display_name = "View ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Camera ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Background Music";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Unknown 0";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Unknown 1";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Unknown 2";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Lighting";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);

    entry.display_name = "Goal ID";
    entry.type = ImGuiDataType_U8;
    entry.value = IO::read<u8>(from, at);
    properties.entries.push_back(entry);
}
