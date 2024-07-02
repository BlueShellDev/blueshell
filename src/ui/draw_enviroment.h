#pragma once

#include "../editor.hpp"
#include "transform.hpp"

namespace UI
{
    struct DrawEnviroment
    {
        ImDrawList* canvas = nullptr;
        ImVec2 canvas_origin = { 0, 0 };
        ImVec2 content_size = { 0, 0 };

        const ImVec2 pixels_per_unit = { 16, 16 };
        ImVec2 mouse_pos = { 0, 0 };

        float scale = 1.0f;
    };

    ImVec2 transform_to_canvas(DrawEnviroment& env, const Transform& t);
    void draw_transform(DrawEnviroment& env, const Transform& t, const u32& color, const float& rounding = 0.0f);
    void draw_transform_o(DrawEnviroment& env, const Transform& t, const u32& color);
    void draw_stage_obj(DrawEnviroment& env, const Transform& t);
    void draw_entrance(DrawEnviroment& env, const Transform& t);
    void draw_view(DrawEnviroment& env, const Transform& t);
}