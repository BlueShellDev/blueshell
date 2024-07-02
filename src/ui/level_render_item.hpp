#pragma once

// file: level_render_object.hpp
// purpose: represents an object in the area renderer

#include "../editor.hpp"
#include "draw_enviroment.hpp"
#include "object_image.hpp"
#include "transform.hpp"

namespace UI
{
    enum class AreaItemType
    {
                                            AreaItemType_BgDat,
                                            AreaItemType_StageObject,
                                            AreaItemType_Entrance,
                                            AreaItemType_View,
                                            AreaItemType_Zone,
                                            AreaItemType_Path
    };

    struct PropertyEntry
    {
        std::string                         display_name            = "";
        ImGuiDataType                       type;
        sz_t                                value                   = 0;
    };

    struct LevelItemProperties
    {
        std::vector<PropertyEntry>          entries;
    };

    struct AreaRenderItem
    {
        // represents render order (rename later)
        size_t                              item_id                 = 0;

        // item position
        Transform                           transform,
                                            prev_transform;

        // item type
        AreaItemType                        item_type;

        // item-specific properties
        LevelItemProperties                 properties;

        bool                                is_selected             = false;
        bool                                resizeable              = false;
    };

    static
    UI::AreaRenderItem create_object_item
    (
        const u16& object_id,
        const u8&  tileset_id
    )
    {
        UI::AreaRenderItem item;

        item.item_type = UI::AreaItemType::AreaItemType_BgDat;
        item.resizeable = true;

        UI::PropertyEntry entry;
        entry.display_name = "Object ID";
        entry.type = ImGuiDataType_U16;
        entry.value = object_id & 0xFFF;
        item.properties.entries.push_back(entry);

        entry.display_name = "Tileset ID";
        entry.type = ImGuiDataType_U8;
        entry.value = tileset_id & 0xF;
        item.properties.entries.push_back(entry);

        return item;
    }
}