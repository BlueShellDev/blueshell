#include "object_picker_widget.h"

namespace UI
{
    void ObjectPickerWidget::_alias_selected_item()
    {
        selection = nullptr;
        for (auto& item : items)
        {
            if (item.is_selected)
            {
                selection = &item;
            }
        }
    }

    void ObjectPickerWidget::_check_mouse_items()
    {
        for (auto& item : items)
        {
            item.is_selected = UI::is_imvec2_inside(item.transform, draw_env.mouse_pos);
        }
    }

    void ObjectPickerWidget::check_mouse_tooltip()
    {
        for (auto& item : items)
        {
            if (UI::is_imvec2_inside(item.transform, draw_env.mouse_pos))
            {
                ImGui::BeginTooltip();
                ImGui::Text(std::format("Object {}", item.object->_id).c_str());
                ImGui::EndTooltip();
            }
        }
    }

    void ObjectPickerWidget::_position_items()
    {
        u16 max_h = 0,
            next_x = x_padding,
            next_y = y_padding;

        for (auto& item : items)
        {
            // first, check if what we're about to draw is not outside right margin
            const u16& width = item.object->width * (draw_env.pixels_per_unit.x);
            const u16& height = item.object->height * (draw_env.pixels_per_unit.y);

            if (next_x + width >= (ImGui::GetWindowWidth() - right_margin) / draw_env.scale)
            {
                next_x = x_padding;
                next_y += max_h + y_padding;
                max_h = 0;
            }

            max_h = std::max(max_h, height);

            item.transform.x = next_x;
            item.transform.y = next_y;
            item.transform.w = width;
            item.transform.h = height;

            next_x += width + x_padding;
        }
    }

    void ObjectPickerWidget::_outline_selected_item()
    {
        if (selection)
        {
            UI::draw_transform(draw_env, selection->transform, 0x7FFF3f3f);
            UI::draw_transform_o(draw_env, selection->transform, 0xFFFF7F7F);
        }
    }

    void ObjectPickerWidget::_draw_items
    (
        Render::RenderManager* render_mgr,
        UI::DrawEnviroment draw_env
    )
    {
        for (auto& item : items)
        {
            ImVec2 draw_at = UI::transform_to_canvas(draw_env, item.transform);
            UI::draw_transform(draw_env, item.transform, 0x0FFFFFFF, 4.0f);

            UI::draw_object
            (
                render_mgr,
                draw_env.canvas,
                draw_at,
                draw_env.scale,
                *tileset,
                item.object->_id,
                item.object->width,
                item.object->height
            );
        }
    }

    void ObjectPickerWidget::populate()
    {

        items.clear();
        _alias_selected_item();

        sz_t id = 0;
        for (auto& object : tileset->objects)
        {
            ObjectPickerItem item;
            item.object = &object;
            item.item_id = id;
            items.push_back(item);
            id++;
        }

        draw_env.scale = 2.0f;
    }

    void ObjectPickerWidget::draw
    (
        Render::RenderManager* render_mgr,
        const ImVec2& size_in_units
    )
    {
        const ImVec2 canvas_scale =
        {
            draw_env.pixels_per_unit.x * draw_env.scale,
            draw_env.pixels_per_unit.y * draw_env.scale
        };

        draw_env.content_size =
        {
            size_in_units.x * canvas_scale.x,
            size_in_units.y * canvas_scale.y
        };

        ImGui::SetNextWindowContentSize(draw_env.content_size);
        ImGui::BeginChild("##object_picker_widget", { 0, 0 }, ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

        draw_env.canvas = ImGui::GetWindowDrawList();
        draw_env.canvas_origin = ImGui::GetCursorScreenPos();
        draw_env.mouse_pos =
        {
            (ImGui::GetMousePos().x - draw_env.canvas_origin.x) / draw_env.scale,
            (ImGui::GetMousePos().y - draw_env.canvas_origin.y) / draw_env.scale
        };

        _position_items();
        _draw_items(render_mgr, draw_env);
        _outline_selected_item();
        check_mouse_tooltip();

        if (ImGui::IsWindowHovered())
        {
            bool left_mb_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            bool right_mb_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

            if (left_mb_clicked)
            {
                _check_mouse_items();
                _alias_selected_item();
            }
        }

        ImGui::EndChild();
    }
}