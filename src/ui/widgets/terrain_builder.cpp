#include "terrain_builder.h"

namespace UI
{
    // internal methods

    double TerrainBuilder::_calc_slope(const TerrainNode& node_0, const TerrainNode& node_1)
    {
        const double num = (node_1.transform.y - node_0.transform.y);
        const double den = (node_1.transform.x - node_0.transform.x);
        return (den != 0) ? (num / den) : (den);
    }

    void TerrainBuilder::_check_next(const u16& x, const u16& y)
    {
        // bascially checks slope
        if (nodes.empty())
        {
            can_plot = true;
            return;
        }

        UI::Transform& last = nodes.back().transform;
        const double num = (y - last.y);
        const double den = (x - last.x);
        const double slope = (den != 0) ? (std::abs(num / den)) : (den);

        can_plot =
            (
                slope == 0 ||
                slope == 0.5 ||
                slope == 1
                );
    }

    // external methods

    void TerrainBuilder::add_node(const u16& x, const u16& y)
    {
        _check_next(x, y);
        if (!can_plot)
            return;

        UI::TerrainNode node{ .node_id = nodes.size() };
        node.transform.x = x;
        node.transform.y = y;

        nodes.push_back(node);

        can_plot = false;
    }

    void TerrainBuilder::draw_nodes(UI::DrawEnviroment draw_env)
    {
        for (auto& node : nodes)
        {
            ImVec2 draw_at_0 = UI::transform_to_canvas(draw_env, node.transform);
            draw_env.canvas->AddCircleFilled(draw_at_0, 5.0f, 0xFF00FF00);

            if (node.node_id > 0)
            {
                ImVec2 draw_at_1 = UI::transform_to_canvas(draw_env, nodes.at(node.node_id - 1).transform);
                draw_env.canvas->AddLine(draw_at_1, draw_at_0, 0x2F00FF00);
            }
        }
    }

    void TerrainBuilder::preview_next_node(UI::DrawEnviroment draw_env, const u16& x, const u16& y)
    {
        _check_next(x, y);

        u32 color = (can_plot) ? 0xFF00FF00 : 0xFF0000FF;

        UI::Transform t = { .x = x, .y = y };

        ImVec2 draw_at_0 = UI::transform_to_canvas(draw_env, t);
        draw_env.canvas->AddCircle(draw_at_0, 5.0f, color);

        if (!nodes.empty())
        {
            ImVec2 draw_at_1 = UI::transform_to_canvas(draw_env, nodes.back().transform);
            draw_env.canvas->AddLine(draw_at_1, draw_at_0, color);
        }
    }

    void TerrainBuilder::render_platform(NSMB::Area* area)
    {
        if (nodes.empty())
            return;

        for (auto& node : nodes)
        {
            if (node.node_id > 0)
            {
                TerrainNode& node_1 = nodes.at(node.node_id - 1);
                double slope = _calc_slope(node_1, node);
                Debug::log(slope);
            }
        }
    }
}