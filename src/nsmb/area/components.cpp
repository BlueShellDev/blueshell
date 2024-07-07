#include "components.h"

namespace NSMB::LevelComponents
{
    /* bgdat */
    void BGDatObject::unpack
    (
        IO::ByteArray& from,
        sz_t& at
    )
    {
        item_type = UI::AreaItemType_BgDat;

        u16 packed = IO::read<u16>(from, at);

        UI::PropertyEntry entry;
        entry.display_name = "Object ID";
        entry.type = ImGuiDataType_U16;
        entry.value = packed & 0xFFF;
        properties.push_back(entry);

        entry.display_name = "Tileset ID";
        entry.type = ImGuiDataType_U8;
        entry.value = (packed >> 12) & 0xF;
        properties.push_back(entry);

        transform.x = IO::read<u16>(from, at);
        transform.y = IO::read<u16>(from, at);
        transform.w = IO::read<u16>(from, at);
        transform.h = IO::read<u16>(from, at);
        prev_transform = transform;

        resizeable = true;
    }

    void BGDatObject::pack
    (
        IO::ByteArray& to,
        sz_t& at
    )
    {
        const u16 packed = (properties.at(0).value | ((u16)properties.at(1).value << 12));

        IO::write(packed, to, at);
        IO::write(transform.x, to, at);
        IO::write(transform.y, to, at);
        IO::write(transform.w, to, at);
        IO::write(transform.h, to, at);
    }

    /* stage obj */
    void StageObject::unpack
    (
        IO::ByteArray& from,
        sz_t& at
    )
    {
        item_type = UI::AreaItemType_StageObject;

        UI::PropertyEntry stage_obj_id;
        stage_obj_id.display_name = "Sprite Type";
        stage_obj_id.type = ImGuiDataType_U16;
        stage_obj_id.value = IO::read<u16>(from, at);

        properties.push_back(stage_obj_id);

        transform.x = IO::read<u16>(from, at);
        transform.y = IO::read<u16>(from, at);
        transform.w = 1;
        transform.h = 1;
        prev_transform = transform;

        data.reserve(6);
        for (sz_t i = 0; i < 6; i++)
        {
            data.push_back(IO::read<u8>(from, at));
        }
    }

    void StageObject::pack
    (
        IO::ByteArray& to,
        sz_t& at
    )
    {
    }

    /* entrance */
    void Entrance::unpack
    (
        IO::ByteArray& from,
        sz_t& at
    )
    {
        item_type = UI::AreaItemType_Entrance;

        transform.pixels_per_unit = { 1, 1 };
        transform.x = IO::read<u16>(from, at);
        transform.y = IO::read<u16>(from, at);
        transform.w = 16;
        transform.h = 16;
        prev_transform = transform;

        UI::PropertyEntry entry;
        entry.display_name = "Camera X";
        entry.type = ImGuiDataType_U16;
        entry.value = IO::read<u16>(from, at);
        properties.push_back(entry);

        entry.display_name = "Camera Y";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u16>(from, at);
        properties.push_back(entry);

        entry.display_name = "Entrance ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Destination Area";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Connected Pipe ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        at++;

        entry.display_name = "Destination Entrance ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        at++;

        entry.display_name = "Entrance Type";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Entrance Settings";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Unknown 0";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        at++;

        entry.display_name = "Is Zone";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Unknown 1";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);
    }

    void Entrance::pack
    (
        IO::ByteArray& to,
        sz_t& at
    )
    {
    }

    /* view */
    void View::unpack
    (
        IO::ByteArray& from,
        sz_t& at
    )
    {
        item_type = UI::AreaItemType_View;

        transform.pixels_per_unit = { 1, 1 };
        transform.x = IO::read<u16>(from, at);
        transform.y = IO::read<u16>(from, at);
        transform.w = IO::read<u16>(from, at);
        transform.h = IO::read<u16>(from, at);
        prev_transform = transform;

        UI::PropertyEntry entry;
        entry.display_name = "View ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Camera ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Background Music";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Unknown 0";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Unknown 1";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Unknown 2";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Lighting";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);

        entry.display_name = "Goal ID";
        entry.type = ImGuiDataType_U8;
        entry.value = IO::read<u8>(from, at);
        properties.push_back(entry);
    }
    
    void View::pack
    (
        IO::ByteArray& to, 
        sz_t& at
    )
    {
    }

    /* paths */
    void Path::unpack
    (
        IO::ByteArray& from, 
        sz_t& at
    )
    {
    }

    void Path::pack
    (
        IO::ByteArray& to,
        sz_t& at
    )
    {
    }
}