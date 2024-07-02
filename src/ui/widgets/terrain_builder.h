#pragma once

#include "../draw_enviroment.h"
#include "../../game/level.h"

namespace UI
{
    struct TerrainNode
    {
        sz_t node_id = 0;
        UI::Transform transform = { .w = 1, .h = 1 };
    };

    struct TerrainBuilder
    {
        bool    is_active = false,
            can_plot = false;

        std::vector<UI::TerrainNode>        nodes;

        // internal methods
        double _calc_slope(const TerrainNode& node_0, const TerrainNode& node_1);
        void _check_next(const u16& x, const u16& y);

        // external methods
        void add_node(const u16& x, const u16& y);
        void draw_nodes(UI::DrawEnviroment draw_env);
        void preview_next_node(UI::DrawEnviroment draw_env, const u16& x, const u16& y);
        void render_platform(NSMB::Area* area);
    };
}