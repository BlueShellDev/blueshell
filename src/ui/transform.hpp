#pragma once

#include "../editor.hpp"

namespace UI
{
    struct Transform
    {
        ImVec2 pixels_per_unit = { 16, 16 };

        // position in units;
        u16     x = 0,
                y = 0,
                w = 0,
                h = 0;
    };

    static Transform translate
    (
        const Transform& t,
        const ImVec2& scale
    )
    {
        return Transform
        {
            .pixels_per_unit = scale,
            .x = (u16)(t.x * (u16)t.pixels_per_unit.x / (u16)scale.x),
            .y = (u16)(t.y * (u16)t.pixels_per_unit.y / (u16)scale.y),
            .w = (u16)(t.w * (u16)t.pixels_per_unit.x / (u16)scale.x),
            .h = (u16)(t.h * (u16)t.pixels_per_unit.y / (u16)scale.y)
        };
    }

    static void move_to
    (
        Transform& t,
        const u16& x,
        const u16& y
    )
    {
        t.x = x;
        t.y = y;
    }

    static void move_by
    (
        Transform& t,
        const s16& dx,
        const s16& dy
    )
    {
        t.x += dx;
        t.y += dy;
    }

    static void resize_to
    (
        Transform& t,
        const u16& w,
        const u16& h
    )
    {
        t.w = w;
        t.h = h;
    }

    static void resize_by
    (
        Transform& t,
        const s16& dw,
        const s16& dh
    )
    {
        t.w += dw;
        t.h += dh;
    }

    static bool is_imvec2_inside
    (
        const Transform& t,
        const ImVec2& v
    )
    {
        return
            (
                (v.x > t.x * t.pixels_per_unit.x) && (v.y > t.y * t.pixels_per_unit.y) &&
                (v.x <= (t.w + t.x) * t.pixels_per_unit.x) && (v.y <= (t.h + t.y) * t.pixels_per_unit.y)
                );
    }

    static bool is_transform_inside
    (
        const Transform& a,
        const Transform& b
    )
    {
        return
            !(
                ((a.x + a.w) * a.pixels_per_unit.x < b.x * b.pixels_per_unit.x) || (a.x * a.pixels_per_unit.x > (b.x + b.w) * b.pixels_per_unit.x) ||
                ((a.y + a.h) * a.pixels_per_unit.y < b.y * b.pixels_per_unit.y) || (a.y * a.pixels_per_unit.y > (b.y + b.h) * b.pixels_per_unit.y)
                );
    }
}