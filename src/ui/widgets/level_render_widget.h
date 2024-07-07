#pragma once

#include "imgui_internal.h"

#include "../../nsmb/area/area.h"
#include "../items/level_render_item.h"
#include "../draw_enviroment.h"
#include "object_picker_widget.h"
#include "terrain_builder.h"

namespace UI
{
    enum class ResizeDirections
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    struct ResizeNub
    {
        ResizeDirections direction;
        Transform transform = { .pixels_per_unit = {1, 1} };
        bool is_active = false;
    };

    struct AreaRenderWidget
    {
        DrawEnviroment draw_env;
        ImVec4 background_color = { 0.0f, 0.0f, 0.0f, 0.5f };

        NSMB::Area* area = nullptr;

        std::vector<AreaRenderItem*> selection;
        std::vector<AreaRenderItem>  clipboard;
        std::vector<AreaRenderItem>  trashcan;

        ObjectPickerItem* paint_item;

        ImVec2 click_cache;

        // stuff for multiselect
        bool can_grab = false;
        Transform grab_transform = { .pixels_per_unit = {1, 1} };

        // perimiter of all selected objects
        Transform max_perimeter = 
        {
            .pixels_per_unit = {1, 1},
            .x = (u16)-1,
            .y = (u16)-1,
            .w = 0,
            .h = 0
        };

        // stuff for resize
        bool is_resizing = false;
        ResizeNub* active_nub = nullptr;
        std::array<ResizeNub, 4> resize_nub_regions = 
        {
            ResizeNub {.direction = ResizeDirections::NORTH },
            ResizeNub {.direction = ResizeDirections::SOUTH },
            ResizeNub {.direction = ResizeDirections::EAST },
            ResizeNub {.direction = ResizeDirections::WEST }
        };

        // zoom/pan stuff
        ImVec2 pan_origin = {};

        // visibility states
        bool                                
        show_objects = true,
        show_actors = true,
        show_entrances = true,
        show_views = true,
        show_paths = true;

        // internal-use methods (state altering)
        void _alias_selected_items();
        void _alias_active_resize_nub();
        void _clear_selected_items();
        void _clear_resize_nubs();
        void _check_mouse_all_items(bool toggle);
        void _check_mouse_resize_nubs();
        void _check_grab_all_items();
        void _update_grab_transform
        (
            const u16& x,
            const u16& y,
            const s16& dx,
            const s16& dy
        );
        void _update_max_perimeter();
        void _update_resize_nubs();
        void _resize_selection
        (
            const s16& dx,
            const s16& dy
        );

        void _draw_items
        (
            Render::RenderManager* render_mgr
        );
        void _outline_selected_items();
        void _draw_max_perimeter();
        void _draw_resize_nubs();

        // external use methods
        void reset(NSMB::Area* area);

        void draw
        (
            Render::RenderManager* render_mgr,
            const ImVec2& size_in_units
        );

        void scale(const float& new_scale);
        void set_items_visibility
        (
            AreaItemType_ item_type, 
            const bool& visible
        );
        void paint_picker_selection(const Transform& transform);
        void copy_selection();
        void paste_selection();
        void delete_selection();
    };

};