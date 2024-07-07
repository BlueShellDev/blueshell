#include "level_render_item.h"

namespace UI
{
    AreaRenderItem create_object_item
    (
        const u16& object_id,
        const u8&  tileset_id
    )
    {
        AreaRenderItem item;

        item.item_type = UI::AreaItemType_BgDat;
        item.resizeable = true;

        PropertyEntry entry;
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