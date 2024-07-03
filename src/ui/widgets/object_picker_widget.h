#pragma once

#include "../items/object_picker_item.h"
#include "../../nsmb/tileset.h"
#include "../draw_enviroment.h"
#include "../object_image.hpp"

namespace UI
{
    struct ObjectPickerWidget
    {
        DrawEnviroment draw_env;

        NSMB::Tileset* tileset = nullptr;

        std::vector<UI::ObjectPickerItem> items;
        ObjectPickerItem* selection = nullptr;

        // padding inbetween items
        u16 x_padding = 3,
            y_padding = 3,
            right_margin = 35;

        // internal-use methods
        void _alias_selected_item();
        void _check_mouse_items();
        void check_mouse_tooltip();
        void _position_items();
        void _outline_selected_item();
        void _draw_items
        (
            Render::RenderManager* render_mgr, 
            DrawEnviroment draw_env
        );

        // external methods
        void populate();
        void draw
        (
            Render::RenderManager* render_mgr, 
            const ImVec2& size_in_units
        );
    };
}