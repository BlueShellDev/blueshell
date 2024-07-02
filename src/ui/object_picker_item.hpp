#pragma once

#include "../editor.hpp"
#include "../graphics/graphics.h"
#include "draw_enviroment.hpp"
#include "object_image.hpp"
#include "transform.hpp"

namespace UI
{
    struct ObjectPickerItem
    {
        sz_t                item_id     = 0;

        Transform           transform   = { .pixels_per_unit = {1, 1} };

        Graphics::Object*   object      = nullptr;
    
        bool                is_selected = false;
    };
}