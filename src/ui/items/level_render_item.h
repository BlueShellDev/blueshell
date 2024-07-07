#pragma once

#include "../draw_enviroment.h"
#include "../object_image.hpp"
#include "../transform.hpp"

namespace UI
{
    struct PropertyEntry
    {
        std::string display_name = "";
        ImGuiDataType type;
        sz_t value = 0;
    };

    enum AreaItemType_
    {
        AreaItemType_BgDat,
        AreaItemType_StageObject,
        AreaItemType_Entrance,
        AreaItemType_View,
        AreaItemType_Zone,
        AreaItemType_Path
    };

    struct AreaRenderItem
    {
        // represents render order (rename later)
        size_t item_id = 0;

        // item position
        Transform transform, prev_transform;

        // item type
        AreaItemType_ item_type;

        // item-specific properties
        std::vector<PropertyEntry> properties;

        bool    is_selected = false,
                resizeable = false;
    };

    // create defs for rest of item types
    UI::AreaRenderItem create_object_item(const u16& object_id, const u8& tileset_id);
}