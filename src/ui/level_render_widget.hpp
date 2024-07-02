#pragma once

// file: level_render_widget.hpp
// purpose: renders area objects

#include "draw_enviroment.hpp"
#include "object_picker_widget.hpp"
#include "level_render_item.hpp"

namespace UI
{
    struct TerrainNode
    {
        sz_t                                node_id                 = 0;
        UI::Transform                       transform               = {.w = 1, .h = 1};
    };

    struct TerrainBuilder
    {
        bool                                is_active               = false;
        bool                                can_plot                = false;

        std::vector<UI::TerrainNode>        nodes;

        double _calc_slope(const TerrainNode& node_0, const TerrainNode& node_1)
        {
            const double num = (node_1.transform.y - node_0.transform.y);
            const double den = (node_1.transform.x - node_0.transform.x);
            return (den != 0) ? (num / den) : (den);
        }

        void _check_next(const u16& x, const u16& y)
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

        void add_node(const u16& x, const u16& y)
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

        void draw_nodes(UI::DrawEnviroment draw_env)
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

        void preview_next_node(UI::DrawEnviroment draw_env, const u16& x, const u16& y)
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

        void render_platform(NSMB::Area* area)
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

            //UI::AreaRenderItem item = UI::create_object_item(0, 33);
            //item.transform.x = mouse_x_units;
            //item.transform.y = mouse_y_units;
            //item.transform.w = 1;
            //item.transform.h = 1;
            //item.prev_transform = item.transform;
            //item.is_selected = true;
            //NSMB::add_item(*area, item);
        }
    };

    enum class ResizeDirections
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    struct ResizeNub
    {
        ResizeDirections                    direction;
        Transform                           transform               = { .pixels_per_unit = {1, 1} };
        bool                                is_active               = false;
    };

    struct AreaRenderWidget
    {
        UI::DrawEnviroment                  draw_env;
        ImVec4                              background_color        = { 0.0f, 0.0f, 0.0f, 0.5f };

        NSMB::Area*                         area                   = nullptr;

        std::vector<UI::AreaRenderItem*>    selection;
        std::vector<UI::AreaRenderItem>     clipboard;
        std::vector<UI::AreaRenderItem>     trashcan;

        UI::ObjectPickerWidget*             active_picker;

        ImVec2                              click_cache;

        // stuff for multiselect
        bool                                can_grab                = false;
        UI::Transform                       grab_transform          = { .pixels_per_unit = {1, 1} };

        // perimiter of all selected objects
        Transform                           max_perimeter           = { 
                                                                        .pixels_per_unit = {1, 1}, 
                                                                        .x = (u16)-1, 
                                                                        .y = (u16)-1, 
                                                                        .w = 0, 
                                                                        .h = 0 
                                                                      };

        // stuff for resize
        bool                                is_resizing             = false;
        ResizeNub*                          active_nub              = nullptr;
        std::array<ResizeNub, 4>            resize_nub_regions      = {
                                                                        ResizeNub {.direction = ResizeDirections::NORTH },
                                                                        ResizeNub {.direction = ResizeDirections::SOUTH },
                                                                        ResizeNub {.direction = ResizeDirections::EAST },
                                                                        ResizeNub {.direction = ResizeDirections::WEST }
                                                                      };

        // zoom/pan stuff
        ImVec2                              pan_origin              = {};

        // visibility states
        bool                                show_objects            = true,
                                            show_sprites            = true,
                                            show_entrances          = true,
                                            show_views              = true,
                                            show_zones              = true,
                                            show_paths              = true;


        // eventually push into its own struct to keep states
        TerrainBuilder                      builder;


        // internal methods
        void _alias_selected_items()
        {
            // copy pointers of selected items to vector
            selection.clear();
            
            for (auto& item : area->items)
            {
                if (item.is_selected)
                {
                    selection.push_back(&item);
                }
            }
        }

        void _alias_active_resize_nub()
        {
            active_nub = nullptr;
            is_resizing = false;
            for (auto& nub : resize_nub_regions)
            {
                if (nub.is_active)
                {
                    active_nub = &nub;
                    is_resizing = true;
                }
            }
        }

        void _clear_selected_items()
        {
            // clear selection
            for (auto& sel : selection)
            {
                sel->is_selected = false;
            }

            selection.clear();
        }

        void _clear_resize_nubs()
        {
            for (auto& nub : resize_nub_regions)
            {
                nub.transform = Transform{ .pixels_per_unit = {1, 1} };
            }
        }

        void _check_mouse_all_items
        (
            bool toggle
        )
        {
            // iterate through items from last -> first so that elements drawn last are selected first
            for (auto& item : area->items | std::ranges::views::reverse)
            {
                // cheap hack to not select views
                if (UI::is_imvec2_inside(item.transform, draw_env.mouse_pos) && item.item_type != UI::AreaItemType::AreaItemType_View)
                {
                    item.prev_transform = item.transform;
                    
                    if (toggle)
                    {
                        item.is_selected = !item.is_selected;
                    }
                    else
                    {
                        item.is_selected = true;
                    }

                    // cheap hack
                    break;
                }
            }
        }

        void _check_mouse_resize_nubs()
        {
            for (auto& nub : resize_nub_regions)
            {
                if (UI::is_imvec2_inside(nub.transform, draw_env.mouse_pos))
                {
                    nub.is_active = true;
                }
                else
                {
                    nub.is_active = false;
                }
            }
        }

        void _check_grab_all_items()
        {
            // iterate through items from last -> first so that elements drawn last are selected first
            for (auto& item : area->items | std::ranges::views::reverse)
            {
                if (UI::is_transform_inside(grab_transform, item.transform) && item.item_type != UI::AreaItemType::AreaItemType_View)
                {
                    item.prev_transform = item.transform;
                    item.is_selected = true;
                }
                else
                {
                    item.is_selected = false;
                }
            }
        }

        void _update_grab_transform
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

        void _update_max_perimeter()
        {
            max_perimeter = { .pixels_per_unit = {1, 1}, .x = (u16)-1, .y = (u16)-1, .w = 0, .h = 0 };

            if (!selection.empty() && selection.size() < 2)
            {
                max_perimeter = UI::translate(selection.front()->transform, { 1, 1 });
            }
            else if (!selection.empty())
            {    
                for (auto& item : selection)
                {
                    Transform t = UI::translate(item->transform, { 1, 1 });
                    max_perimeter.x = std::min(max_perimeter.x, t.x);
                    max_perimeter.y = std::min(max_perimeter.y, t.y);
                    max_perimeter.w = std::max((u16)max_perimeter.w, u16(t.x + t.w));
                    max_perimeter.h = std::max((u16)max_perimeter.h, u16(t.y + t.h));
                }
                max_perimeter.w -= max_perimeter.x;
                max_perimeter.h -= max_perimeter.y;
            }
        }

        void _update_resize_nubs()
        {
            // check if any objects in selection cannot be resized
            for (auto& item : selection)
            {
                if (!item->resizeable)
                {
                    _clear_resize_nubs();
                    return;
                }
            }

            u16 nub_thickness   = 12 / draw_env.scale;
            u16 nub_padding     = 6 / draw_env.scale;

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

        void _resize_selection
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
            }
        }

        void _draw_items
        (
            Render::RenderManager* render_mgr
        )
        {
            // for level items
            for (auto& item : area->items)
            {
                ImVec2 draw_at = UI::transform_to_canvas(draw_env, item.transform);
                switch (item.item_type)
                {
                case UI::AreaItemType::AreaItemType_BgDat:
                    if (!show_objects)
                        break;

                    UI::draw_object
                    (
                        render_mgr,
                        draw_env.canvas,
                        draw_at,
                        draw_env.scale,
                        area->tilesets.at(item.properties.entries[1].value),
                        item.properties.entries[0].value,
                        item.transform.w,
                        item.transform.h
                    );
                    break;
                
                case UI::AreaItemType::AreaItemType_StageObject:
                    if (!show_sprites)
                        break;

                    UI::draw_stage_obj(draw_env, item.transform);

                    draw_env.canvas->AddText
                    (
                        ImGui::GetDefaultFont(),
                        6.0f * draw_env.scale,
                        draw_at,
                        0xFFFFFFFF,
                        std::format("{}", item.properties.entries.at(0).value).c_str()
                    );
                    break;
                
                case UI::AreaItemType::AreaItemType_Entrance:
                    if (!show_entrances)
                        break;

                    UI::draw_entrance(draw_env, item.transform);

                    draw_env.canvas->AddText
                    (
                        ImGui::GetDefaultFont(),
                        6.0f * draw_env.scale,
                        draw_at,
                        0xFFFFFFFF,
                        std::format("{}", item.properties.entries.at(2).value).c_str()
                    );
                    break;
                
                case UI::AreaItemType::AreaItemType_View:
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
                        std::format("View {}", item.properties.entries.at(0).value).c_str()
                    );
                    break;
                
                default:
                    break;
                }
            }
        }

        void _outline_selected_items()
        {
            for (auto& item : selection)
                UI::draw_transform(draw_env, item->transform, 0x7FFF3f3f);
        }

        void _draw_max_perimeter()
        {
            UI::draw_transform_o(draw_env, max_perimeter, 0x7FFF7F7F);
            UI::draw_transform_o(draw_env, max_perimeter, 0x7FFF7F7F);
        }

        void _draw_resize_nubs()
        {
            for (auto& nub : resize_nub_regions)
            {
                UI::draw_transform(draw_env, nub.transform, 0xFFFF7F7F, 4.0f);
            }
        }

        /* external methods */
        void draw
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

            if (builder.is_active)
                builder.draw_nodes(draw_env);
          
            ImGui::Begin("builder");
            ImGui::Checkbox("Enable Builder", &builder.is_active);
            if (ImGui::Button("Render Platform"))
            {
                builder.render_platform(area);
            }
            ImGui::End();

            if (ImGui::IsWindowHovered())
            {
                bool left_mb_clicked        = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool ctr_mb_clicked         = ImGui::IsMouseClicked(ImGuiMouseButton_Middle);
                bool right_mb_clicked       = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

                bool left_mb_released       = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
                bool right_mb_released      = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

                bool left_mb_dragging       = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
                bool right_mb_dragging      = ImGui::IsMouseDragging(ImGuiMouseButton_Right);

                ImVec2 left_mb_delta        = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                ImVec2 ctr_mb_delta         = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
                ImVec2 right_mb_delta       = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

                bool shift_mod              = ImGui::IsKeyDown(ImGuiMod_Shift);
                bool ctrl_mod               = ImGui::IsKeyDown(ImGuiMod_Ctrl);
                
                u16 mouse_x_units = (u16)draw_env.mouse_pos.x / (u16)draw_env.pixels_per_unit.x;
                u16 mouse_y_units = (u16)draw_env.mouse_pos.y / (u16)draw_env.pixels_per_unit.y;

                u16 mouse_x_units_s = (u16)(draw_env.mouse_pos.x + 8) / (u16)draw_env.pixels_per_unit.x;
                u16 mouse_y_units_s = (u16)(draw_env.mouse_pos.y + 8) / (u16)draw_env.pixels_per_unit.y;

                s16 x_delta = (left_mb_dragging ? (left_mb_delta.x) : (right_mb_delta.x)) / draw_env.scale;
                s16 y_delta = (left_mb_dragging ? (left_mb_delta.y) : (right_mb_delta.y)) / draw_env.scale;
                
                // integer chopping (not actually units, rename later)
                s16 x_delta_units = (x_delta / (u16)draw_env.pixels_per_unit.x) * draw_env.pixels_per_unit.x;
                s16 y_delta_units = (y_delta / (u16)draw_env.pixels_per_unit.y) * draw_env.pixels_per_unit.y;

                if (builder.is_active)
                    builder.preview_next_node(draw_env, mouse_x_units_s, mouse_y_units_s);

                // mouse handling
                if (left_mb_released || right_mb_released)
                {
                }

                if (ctr_mb_clicked)
                {
                    pan_origin = 
                    {
                        ImGui::GetScrollX(),
                        ImGui::GetScrollY()
                    };
                }

                if (left_mb_clicked)
                {
                    // create a copy of where we just clicked to use for selection areas
                    click_cache = draw_env.mouse_pos;

                    // update all selection items' previous transform
                    for (auto& item : selection)
                    {
                        item->prev_transform = item->transform;
                    }

                    // check if we are clicking on a resize nub
                    _check_mouse_resize_nubs();
                    _alias_active_resize_nub();

                    if (!is_resizing)
                    {
                        //_clear_selected_items();
                        if (shift_mod)
                        {
                            _check_mouse_all_items(true);
                        }
                        else
                        {
                            // 3 am
                            bool good = false;
                            for (auto& item : selection)
                            {
                                if (UI::is_imvec2_inside(item->transform, draw_env.mouse_pos))
                                {
                                    good = true;
                                }
                            }
                            if (!good)
                            {
                                _clear_selected_items();
                            }
                            _check_mouse_all_items(false);
                        }
                        _alias_selected_items();
                    }

                    // if selection is empty at this point, we clicked on a free space. 
                    // logically, it should be good to start grabbing if free.
                    can_grab = selection.empty();
                }

                if (left_mb_clicked && builder.is_active)
                {
                    builder.add_node(mouse_x_units_s, mouse_y_units_s);
                }

                if (left_mb_dragging)
                {
                    if (is_resizing)
                    {
                        // resize routine
                        _resize_selection(x_delta_units, y_delta_units);
                    }
                    else if (can_grab)
                    {
                        // grab routine
                        _update_grab_transform(click_cache.x, click_cache.y, x_delta, y_delta);
                        _check_grab_all_items();
                        _alias_selected_items();
                        UI::draw_transform(draw_env, grab_transform, 0x2FFF7F7F);
                    }
                    else
                    {
                        // not-grabbing routine (dragging)
                        for (auto& item : selection)
                        {
                            UI::move_to
                            (
                                item->transform, 
                                item->prev_transform.x + (x_delta_units / item->transform.pixels_per_unit.x),
                                item->prev_transform.y + (y_delta_units / item->transform.pixels_per_unit.y)
                            );
                        }
                    }
                }

                if (right_mb_clicked && active_picker)
                {
                    _clear_selected_items();
                    if (active_picker->selection)
                    {
                        UI::AreaRenderItem item = UI::create_object_item(active_picker->selection->object->_id, (u8)active_picker->tileset->type);
                        item.transform.x = mouse_x_units;
                        item.transform.y = mouse_y_units;
                        item.transform.w = 1;
                        item.transform.h = 1;
                        item.prev_transform = item.transform;
                        item.is_selected = true;
                        NSMB::add_item(*area, item);
                    }
                    _alias_selected_items();
                }

                if (right_mb_dragging)
                {
                    // resize drag
                    for (auto& item : selection)
                    {
                        UI::resize_to
                        (
                            item->transform,
                            item->prev_transform.w + (x_delta_units / item->transform.pixels_per_unit.x),
                            item->prev_transform.h + (y_delta_units / item->transform.pixels_per_unit.y)
                        );
                    }
                }
                
                // copy/paste handling

                if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_C))
                {
                    clipboard.clear();
                    // copy selection into clipboard
                    for (auto  item : selection)
                    {
                        clipboard.push_back(*item);
                    }
                }

                if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_V))
                {
                    _clear_selected_items();
                    for (auto copy : clipboard)
                    {
                        copy.prev_transform = copy.transform;
                        copy.is_selected    = true;

                        NSMB::add_item(*area, copy);

                        _alias_selected_items();
                    }
                }

                // delete handling

                if ((ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_X)) || ImGui::IsKeyPressed(ImGuiKey_Delete))
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

                // zoom/pan handling

                if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_Equal))
                {
                    draw_env.scale += 0.25f;
                }

                if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_Minus))
                {
                    draw_env.scale -= 0.25f;
                }

                if (ctrl_mod && ImGui::IsKeyPressed(ImGuiKey_MouseWheelY))
                {
                    draw_env.scale += 0.25f * ImGui::GetIO().MouseWheel;
                }

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
    };
}