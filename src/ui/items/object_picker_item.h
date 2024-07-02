#pragma once

#include "../../graphics/graphics.h"
#include "../transform.hpp"

namespace UI
{
    struct ObjectPickerItem
    {
        sz_t item_id = 0;

        Transform transform = { .pixels_per_unit = {1, 1} };

        Graphics::Object* object = nullptr;
    
        bool is_selected = false;
    };
}