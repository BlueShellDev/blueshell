#include "level_render_item.h"

namespace UI
{
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
        item.properties.push_back(entry);

        entry.display_name = "Tileset ID";
        entry.type = ImGuiDataType_U8;
        entry.value = tileset_id & 0xF;
        item.properties.push_back(entry);

        return item;
    }
}