#include "level_render_widget.h"

namespace UI
{
    // internal methods

    void AreaRenderWidget::_alias_selected_items()
    {
        // copy pointers of selected items to vector
        selection.clear();
            
        for (auto& item : area->items)
            if (item.is_selected)
                selection.push_back(&item);
    }

    void AreaRenderWidget::_alias_active_resize_nub()
    {
        active_nub = nullptr;
        is_resizing = false;
        for (auto& nub : resize_nub_regions)
            if (nub.is_active)
            {
                active_nub = &nub;
                is_resizing = true;
            }
    }

    void AreaRenderWidget::_clear_selected_items()
    {
        // clear selection
        for (auto& sel : selection)
            sel->is_selected = false;

        selection.clear();
    }

    void AreaRenderWidget::_clear_resize_nubs()
    {
        for (auto& nub : resize_nub_regions)
            nub.transform = Transform{ .pixels_per_unit = {1, 1} };
    }

    void AreaRenderWidget::_check_mouse_all_items(bool toggle)
    {
        // iterate through items from last -> first so that elements drawn last are selected first
        for (auto& item : area->items | std::ranges::views::reverse)
        {
            // cheap hack to not select views
            if (UI::is_imvec2_inside(item.transform, draw_env.mouse_pos) && item.item_type != UI::AreaItemType_View)
            {
                item.prev_transform = item.transform;
                if (toggle)
                    item.is_selected = !item.is_selected;
                else
                    item.is_selected = true;
                break;
            }
        }
    }

    void AreaRenderWidget::_check_mouse_resize_nubs()
    {
        for (auto& nub : resize_nub_regions)
            nub.is_active = UI::is_imvec2_inside(nub.transform, draw_env.mouse_pos);
    }

    void AreaRenderWidget::_check_grab_all_items()
    {
        // iterate through items from last -> first so that elements drawn last are selected first
        for (auto& item : area->items | std::ranges::views::reverse)
        {
            if (UI::is_transform_inside(grab_transform, item.transform) && item.item_type != UI::AreaItemType_View)
            {
                item.prev_transform = item.transform;
                item.is_selected = true;
            }
            else
                item.is_selected = false;
        }
    }

    void AreaRenderWidget::_update_grab_transform
    (
        const u16& x, 
        const u16& y, 
        const s16& dx, 
        const s16& dy
    )
    {
        grab_transform.x = (dx < 0 ? click_cache.x + dx : click_cache.x);
        grab_transform.y = (dy < 0 ? click_cache.y + dy : click_cache.y);
        grab_transform.w = std::abs(dx) + 1;
        grab_transform.h = std::abs(dy) + 1;
    }

    void AreaRenderWidget::_update_max_perimeter()
    {
        max_perimeter = { .pixels_per_unit = {1, 1}, .x = (u16)-1, .y = (u16)-1, .w = 0, .h = 0 };

        if (!selection.empty() && selection.size() < 2)
            max_perimeter = UI::translate(selection.front()->transform, { 1, 1 });

        else if (!selection.empty())
        {    
            for (auto& item : selection)
            {
                Transform t = UI::translate(item->transform, { 1, 1 });
                max_perimeter.x = (((max_perimeter.x) < (t.x)) ? (max_perimeter.x) : (t.x));
                max_perimeter.y = (((max_perimeter.y) < (t.y)) ? (max_perimeter.y) : (t.y));
                max_perimeter.w = ((((u16)max_perimeter.w) > (u16(t.x + t.w))) ? ((u16)max_perimeter.w) : (u16(t.x + t.w)));
                max_perimeter.h = ((((u16)max_perimeter.h) > (u16(t.y + t.h))) ? ((u16)max_perimeter.h) : (u16(t.y + t.h)));
            }
            max_perimeter.w -= max_perimeter.x;
            max_perimeter.h -= max_perimeter.y;
        }
    }

    void AreaRenderWidget::_update_resize_nubs()
    {
        // check if any objects in selection cannot be resized
        for (auto& item : selection)
            if (!item->resizeable)
            {
                _clear_resize_nubs();
                return;
            }

        u16 nub_thickness   = 8 / draw_env.scale;
        u16 nub_padding     = 8 / draw_env.scale;

        for (auto& nub : resize_nub_regions)
        {
            // configure the transforms for each nub (relative to selection perimeter)
            nub.transform = max_perimeter;

            switch (nub.direction)
            {
            case UI::ResizeDirections::NORTH:
                nub.transform.x += (max_perimeter.w / 4);
                nub.transform.y -= nub_thickness + nub_padding;
                nub.transform.w = (max_perimeter.w / 2);
                nub.transform.h = nub_thickness;
                break;
            case UI::ResizeDirections::SOUTH:
                nub.transform.x += (max_perimeter.w / 4);
                nub.transform.y += max_perimeter.h + nub_padding;
                nub.transform.w = (max_perimeter.w / 2);
                nub.transform.h = nub_thickness;
                break;
            case UI::ResizeDirections::EAST:
                nub.transform.x -= nub_thickness + nub_padding;
                nub.transform.y += (max_perimeter.h / 4);
                nub.transform.w = nub_thickness;
                nub.transform.h = (max_perimeter.h / 2);
                break;
            case UI::ResizeDirections::WEST:
                nub.transform.x += max_perimeter.w + nub_padding;
                nub.transform.y += (max_perimeter.h / 4);
                nub.transform.w = nub_thickness;
                nub.transform.h = (max_perimeter.h / 2);
                break;
            }
        }
    }

    void AreaRenderWidget::_resize_selection
    (
        const s16& dx,
        const s16& dy
    )
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        for (auto& item : selection)
        {
            switch (active_nub->direction)
            {
            case UI::ResizeDirections::NORTH:
                item->transform.y = item->prev_transform.y + (dy / item->transform.pixels_per_unit.y);
                item->transform.h = item->prev_transform.h - (dy / item->transform.pixels_per_unit.y);
                break;
            case UI::ResizeDirections::SOUTH:
                item->transform.h = item->prev_transform.h + (dy / item->transform.pixels_per_unit.y);
                break;
            case UI::ResizeDirections::EAST:
                item->transform.x = item->prev_transform.x + (dx / item->transform.pixels_per_unit.x);
                item->transform.w = item->prev_transform.w - (dx / item->transform.pixels_per_unit.x);
                break;
            case UI::ResizeDirections::WEST:
                item->transform.w = item->prev_transform.w + (dx / item->transform.pixels_per_unit.x);
                break;
            }

            item->transform.x = std::clamp<s16>(item->transform.x, 1, item->prev_transform.x + item->prev_transform.w - 1);
            item->transform.y = std::clamp<s16>(item->transform.y, 1, item->prev_transform.y + item->prev_transform.h - 1);
            item->transform.w = std::clamp<s16>(item->transform.w, 1, 0xFF);
            item->transform.h = std::clamp<s16>(item->transform.h, 1, 0xFF);
        }
    }

    void AreaRenderWidget::_draw_items(Render::RenderManager* render_mgr)
    {
        // for level items
        for (auto& item : area->items)
        {
            ImVec2 draw_at = UI::transform_to_canvas(draw_env, item.transform);
            switch (item.item_type)
            {
            case UI::AreaItemType_BgDat:
                if (!show_objects)
                    break;

                UI::draw_object
                (
                    render_mgr,
                    draw_env.canvas,
                    draw_at,
                    draw_env.scale,
                    area->tilesets.at(item.properties.at(1).value),
                    item.properties.at(0).value,
                    item.transform.w,
                    item.transform.h
                );
                break;
                
            case UI::AreaItemType_StageObject:
                if (!show_actors)
                    break;

                UI::draw_stage_obj(draw_env, item.transform);
                draw_env.canvas->AddText
                (
                    ImGui::GetDefaultFont(),
                    9.0f * draw_env.scale,
                    draw_at,
                    0xFFFFFFFF,
                    std::format("{}", item.properties.at(0).value).c_str()
                );
                break;
                
            case UI::AreaItemType_Entrance:
                if (!show_entrances)
                    break;

                UI::draw_entrance(draw_env, item.transform);
                draw_env.canvas->AddText
                (
                    ImGui::GetDefaultFont(),
                    9.0f * draw_env.scale,
                    draw_at,
                    0xFFFFFFFF,
                    std::format("{}", item.properties.at(2).value).c_str()
                );
                break;
                
            case UI::AreaItemType_View:
                if (!show_views)
                    break;

                UI::draw_view(draw_env, item.transform);
                draw_at.y += (8 * draw_env.scale);
                draw_at.x += (8 * draw_env.scale);
                draw_env.canvas->AddText
                (
                    ImGui::GetDefaultFont(),
                    12.0f * (draw_env.scale),
                    draw_at,
                    0xFFFFFFFF,
                    std::format("View {}", item.properties.at(0).value).c_str()
                );
                break;
            }
        }
    }

    void AreaRenderWidget::_outline_selected_items()
    {
        for (auto& item : selection)
            UI::draw_transform(draw_env, item->transform, 0x7FFF3f3f);
    }

    void AreaRenderWidget::_draw_max_perimeter()
    {
        UI::draw_transform_o(draw_env, max_perimeter, 0x7FFF7F7F);
        UI::draw_transform_o(draw_env, max_perimeter, 0x7FFF7F7F);
    }

    void AreaRenderWidget::_draw_resize_nubs()
    {
        for (auto& nub : resize_nub_regions)
            UI::draw_transform(draw_env, nub.transform, 0xFFFF7F7F, 4.0f);
    }

    // external methods
    
    void AreaRenderWidget::reset(NSMB::Area* area)
    {
        _clear_selected_items();
        
    }

    void AreaRenderWidget::draw
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
        ImGui::PushStyleColor(ImGuiCol_ChildBg, background_color);
        ImGui::BeginChild("##level_render_widget", { 0, 0 }, ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

        draw_env.canvas          = ImGui::GetWindowDrawList();
        draw_env.canvas_origin   = ImGui::GetCursorScreenPos();
        draw_env.mouse_pos =
        {
            (ImGui::GetMousePos().x - draw_env.canvas_origin.x) / draw_env.scale,
            (ImGui::GetMousePos().y - draw_env.canvas_origin.y) / draw_env.scale
        };

        _draw_items(render_mgr);
        _outline_selected_items();

        _update_max_perimeter();
        _update_resize_nubs();

        _draw_max_perimeter();
        _draw_resize_nubs();

        if (ImGui::IsWindowHovered())
        {
            bool shift_mod = ImGui::IsKeyDown(ImGuiMod_Shift);
            bool ctrl_mod = ImGui::IsKeyDown(ImGuiMod_Ctrl);
            bool alt_mod = ImGui::IsKeyDown(ImGuiMod_Alt);

            bool left_mb_clicked        = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            bool ctr_mb_clicked         = ImGui::IsMouseClicked(ImGuiMouseButton_Middle);
            bool right_mb_clicked       = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

            bool left_mb_dragging       = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
            bool right_mb_dragging      = ImGui::IsMouseDragging(ImGuiMouseButton_Right);

            ImVec2 left_mb_delta        = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImVec2 ctr_mb_delta         = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
            ImVec2 right_mb_delta       = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                
            u16 mouse_x_units_s = (u16)(draw_env.mouse_pos.x + (draw_env.pixels_per_unit.x / 2)) / (u16)draw_env.pixels_per_unit.x;
            u16 mouse_y_units_s = (u16)(draw_env.mouse_pos.y + (draw_env.pixels_per_unit.x / 2)) / (u16)draw_env.pixels_per_unit.y;

            u16 mouse_x_units = (u16)(draw_env.mouse_pos.x) / (u16)draw_env.pixels_per_unit.x;
            u16 mouse_y_units = (u16)(draw_env.mouse_pos.y) / (u16)draw_env.pixels_per_unit.y;

            s16 x_delta = (left_mb_dragging ? (left_mb_delta.x) : (right_mb_delta.x)) / draw_env.scale;
            s16 y_delta = (left_mb_dragging ? (left_mb_delta.y) : (right_mb_delta.y)) / draw_env.scale;
                
            // integer chopping (not actually units, rename later)
            s16 x_delta_units = (x_delta / (u16)draw_env.pixels_per_unit.x) * draw_env.pixels_per_unit.x;
            s16 y_delta_units = (y_delta / (u16)draw_env.pixels_per_unit.y) * draw_env.pixels_per_unit.y;

            if (ctr_mb_clicked)
                pan_origin = 
                {
                    ImGui::GetScrollX(),
                    ImGui::GetScrollY()
                };

            if (left_mb_clicked)
            {
                // create a copy of where we just clicked to use for selection areas
                click_cache = draw_env.mouse_pos;

                // update all selection items' previous transform
                for (auto& item : selection)
                    item->prev_transform = item->transform;

                // check if we are clicking on a resize nub
                _check_mouse_resize_nubs();
                _alias_active_resize_nub();

                if (!is_resizing)
                {
                    //_clear_selected_items();
                    if (shift_mod)
                        _check_mouse_all_items(true);
                    else
                    {
                        // 3 am
                        bool good = false;
                        for (auto& item : selection)
                            if (UI::is_imvec2_inside(item->transform, draw_env.mouse_pos))
                                good = true;
                        if (!good)
                            _clear_selected_items();

                        _check_mouse_all_items(false);
                    }
                    _alias_selected_items();
                }

                // if selection is empty at this point, we clicked on a free space. 
                // logically, it should be good to start grabbing if free.
                can_grab = selection.empty();
            }

            if (left_mb_dragging)
            {
                if (is_resizing) // resize routine 
                    _resize_selection(x_delta_units, y_delta_units);
                else if (can_grab)
                {
                    // grab routine
                    _update_grab_transform(click_cache.x, click_cache.y, x_delta, y_delta);
                    _check_grab_all_items();
                    _alias_selected_items();
                    UI::draw_transform(draw_env, grab_transform, 0x2FFF7F7F);
                }
                else // not-grabbing routine (dragging)
                    for (auto& item : selection)
                        UI::move_to
                        (
                            item->transform, 
                            item->prev_transform.x + (x_delta_units / item->transform.pixels_per_unit.x),
                            item->prev_transform.y + (y_delta_units / item->transform.pixels_per_unit.y)
                        );
            }

            if (right_mb_clicked)
            {
                _clear_selected_items();
                paint_picker_selection
                (
                    { 
                        .x = mouse_x_units, .y = mouse_y_units, 
                        .w = 1, .h = 1 
                    }
                );
                _alias_selected_items();
            }

            if (right_mb_dragging)
            {
                // resize drag
                for (auto& item : selection)
                {
                    UI::move_to
                    (
                        item->transform,
                        (x_delta_units < 0) ? item->prev_transform.x + (x_delta_units / item->transform.pixels_per_unit.x) : item->prev_transform.x,
                        (y_delta_units < 0) ? item->prev_transform.y + (y_delta_units / item->transform.pixels_per_unit.y) : item->prev_transform.y
                    );

                    UI::resize_to
                    (
                        item->transform,
                        item->prev_transform.w + (std::abs(x_delta_units) / item->transform.pixels_per_unit.x),
                        item->prev_transform.h + (std::abs(y_delta_units) / item->transform.pixels_per_unit.y)
                    );

                    item->transform.w = std::clamp<s16>(item->transform.w, 1, 0xFF);
                    item->transform.h = std::clamp<s16>(item->transform.h, 1, 0xFF);
                }
            }
                
            // copy handling
            if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_C))
                copy_selection();

            // paste handling
            if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_V))
                paste_selection();

            // delete handling
            if ((ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_X)) || ImGui::IsKeyPressed(ImGuiKey_Delete))
                delete_selection();

            // visibility command handling
            if (ctrl_mod && alt_mod && ImGui::IsKeyPressed(ImGuiKey_O))
                set_items_visibility(AreaItemType_BgDat, !show_objects);

            if (ctrl_mod && alt_mod && ImGui::IsKeyPressed(ImGuiKey_A))
                set_items_visibility(AreaItemType_StageObject, !show_actors);

            if (ctrl_mod && alt_mod && ImGui::IsKeyPressed(ImGuiKey_E))
                set_items_visibility(AreaItemType_Entrance, !show_entrances);

            if (ctrl_mod && alt_mod && ImGui::IsKeyPressed(ImGuiKey_V))
                set_items_visibility(AreaItemType_View, !show_views);

            if (ctrl_mod && alt_mod && ImGui::IsKeyPressed(ImGuiKey_P))
                set_items_visibility(AreaItemType_Path, !show_paths);

            // zoom/pan handling
            if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_Equal))
                scale(draw_env.scale + 0.25f);

            if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_Minus))
                scale(draw_env.scale - 0.25f);

            if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_MouseWheelY))
                scale(draw_env.scale + 0.25f * (std::abs(ImGui::GetIO().MouseWheel) / ImGui::GetIO().MouseWheel));

            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                ImGui::SetScrollX(pan_origin.x - ctr_mb_delta.x);
                ImGui::SetScrollY(pan_origin.y - ctr_mb_delta.y);
            }
        }       

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void AreaRenderWidget::paint_picker_selection(const Transform& transform)
    {
        if (paint_item)
        {
            UI::AreaRenderItem item = UI::create_object_item
            (
                paint_item->object->_id,
                (u8)paint_item->tileset->type
            );
            item.transform = transform;
            item.prev_transform = item.transform;
            item.is_selected = true;
            NSMB::add_item(*area, item);
        }
    }

    void AreaRenderWidget::copy_selection()
    {
        clipboard.clear();
        // copy selection into clipboard
        for (auto item : selection)
        {
            clipboard.push_back(*item);
        }
    }

    void AreaRenderWidget::paste_selection()
    {
        _clear_selected_items();
        for (auto copy : clipboard)
        {
            copy.prev_transform = copy.transform;
            copy.is_selected = true;

            NSMB::add_item(*area, copy);

            _alias_selected_items();
        }
    }

    void AreaRenderWidget::delete_selection()
    {
        // dereference selection and copy into "purgatory"
        for (auto item : selection)
        {
            trashcan.push_back(*item);
        }
        // wipe selection
        _clear_selected_items();

        // iterate through trash
        // issue: when items are deleted, the ids shift by how many items were deleted.
        // but the ids of these items in the trash DONT get updated, so the error keeps increasing
        for (auto item : trashcan)
        {
            NSMB::remove_item(*area, item);
        }

        trashcan.clear();
    }

    // useful commands
    void AreaRenderWidget::scale(const float& new_scale)
    {
        draw_env.scale = std::clamp<float>(new_scale, 0.25f, 4.0f);
    }

    void AreaRenderWidget::set_items_visibility(AreaItemType_ item_type, const bool& visible)
    {
        show_objects = (item_type == AreaItemType_BgDat) && visible;
    }
}