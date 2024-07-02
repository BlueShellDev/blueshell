#include "draw_enviroment.h"

namespace UI
{
    ImVec2 UI::transform_to_canvas
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

    void UI::draw_transform
    (
        DrawEnviroment&                     env, 
        const Transform&                    t,
        const u32&                          color,
        const float&                        rounding
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

    void UI::draw_transform_o
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

    void UI::draw_stage_obj
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        draw_transform(env, t, 0x7FFF00FF, 4.0f);
        draw_transform_o(env, t, 0xBF000000);
    }
    
    void UI::draw_entrance
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        draw_transform(env, t, 0x7F0FFF00, 4.0f);
        draw_transform_o(env, t, 0xBF000000);
    }

    void UI::draw_view
    (
        DrawEnviroment&                     env,
        const Transform&                    t
    )
    {
        UI::draw_transform(env, t, 0x07FFDFDF);
        UI::draw_transform_o(env, t, 0x7FFFDFDF);
    }
}