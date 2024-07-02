#pragma once

#include "transform.hpp"

namespace UI
{
    struct DrawEnviroment
    {
        ImDrawList*                         canvas                  = nullptr;
        ImVec2                              canvas_origin           = { 0, 0 };
        ImVec2                              content_size            = { 0, 0 };

        const ImVec2                        pixels_per_unit         = { 16, 16 };
        ImVec2                              mouse_pos               = { 0, 0 };

        float                               scale                   = 1.0f;
    };

    static ImVec2 transform_to_canvas
    (
        DrawEnviroment&                     env, 
        const Transform&                    t
    )
    {
        return         
        {
            env.canvas_origin.x + (t.x * (t.pixels_per_unit.x * env.scale)),
            env.canvas_origin.y + (t.y * (t.pixels_per_unit.y * env.scale))
        };
    }

    static void draw_transform
    (
        DrawEnviroment&                     env, 
        const Transform&                    t,
        const u32&                          color,
        const float&                        rounding = 0.0f
    )
    {
        const ImVec2 top_left =
        {
            env.canvas_origin.x + (t.x * (t.pixels_per_unit.x * env.scale)),
            env.canvas_origin.y + (t.y * (t.pixels_per_unit.y * env.scale))
        };

        const ImVec2 btm_right = 
        {
            env.canvas_origin.x + ((t.x + t.w) * (t.pixels_per_unit.x * env.scale)),
            env.canvas_origin.y + ((t.y + t.h) * (t.pixels_per_unit.y * env.scale))
        };

        env.canvas->AddRectFilled(top_left, btm_right, color, rounding);
        //env.canvas->AddRect(top_left, btm_right, 0x3F000000);
    }

    static void draw_transform_o
    (
        DrawEnviroment& env,
        const Transform& t,
        const u32& color
    )
    {
        const ImVec2 top_left =
        {
            env.canvas_origin.x + (t.x * (t.pixels_per_unit.x * env.scale)),
            env.canvas_origin.y + (t.y * (t.pixels_per_unit.y * env.scale))
        };

        const ImVec2 btm_right =
        {
            env.canvas_origin.x + ((t.x + t.w) * (t.pixels_per_unit.x * env.scale)),
            env.canvas_origin.y + ((t.y + t.h) * (t.pixels_per_unit.y * env.scale))
        };

        //env.canvas->AddRectFilled(top_left, btm_right, color);
        env.canvas->AddRect(top_left, btm_right, color, 0, 0, 2.0f);
    }

    static void draw_stage_obj
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        draw_transform(env, t, 0x7FFF00FF, 4.0f);
        draw_transform_o(env, t, 0xBF000000);
    }
    
    static void draw_entrance
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        draw_transform(env, t, 0x7F0FFF00, 4.0f);
        draw_transform_o(env, t, 0xBF000000);
    }

    static void draw_view
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        UI::draw_transform(env, t, 0x07FFDFDF);
        UI::draw_transform_o(env, t, 0x7FFFDFDF);
    }
}