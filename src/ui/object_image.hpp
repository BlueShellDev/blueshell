#pragma once

#include "../graphics/render.h"
#include "../graphics/graphics.h"
#include "../nsmb/tileset.h"
#include "../util/util.hpp"

namespace UI
{
    using RefSection = std::vector<std::reference_wrapper<const Graphics::ObjectRow>>;

    static void _draw_tile
    (
        Render::RenderManager*      render_mgr,
        ImDrawList*                 canvas,
        const ImVec2&               draw_at,
        const float                 scale,

        const NSMB::Tileset&        tileset,
        const Graphics::ObjectTile& tile,
        const sz_t                  x,
        const sz_t                  y,
        const sz_t                  w
    )
    {
        const Render::PixelBuffer* buffer = render_mgr->buffer_table.at
        (
            static_cast<Render::TextureSlot_>(6 + (sz_t)tileset.type)
        );

        sz_t tile_x, tile_y;

        sz_t offset;
        switch (tileset.type)
        {
        case NSMB::TilesetType_Main:
            offset = 256;
            break;
        case NSMB::TilesetType_Sub:
            offset = 1024;
            break;
        default:
            offset = 0;
            break;
        }

        Util::i_to_xy
        (
            tile.map16_tile_offset - offset,
            (buffer->width / 16),
            tile_x,
            tile_y
        );

        const ImVec2 tile_uv_0 =
        {
            tile_x / 16.f,
            tile_y / (buffer->height / 16.f)
        };

        const ImVec2 tile_uv_1 =
        {
            (tile_x + 1) / 16.f,
            (tile_y + 1) / (buffer->height / 16.f)
        };

        canvas->AddImage
        (
            (ImTextureID)*buffer->link_to_texture,
            { draw_at.x + (x * 16 * scale),                 draw_at.y + (y * (16 * scale)) },
            { draw_at.x + (x * 16 * scale) + (16 * scale),  draw_at.y + (y * 16 * scale) + (16 * scale) },
            tile_uv_0,
            tile_uv_1
        );

        if (tileset.use_overrides)
        {
            for (auto& entry : render_mgr->overrides_table)
            {
                if (entry.overrides == tile.map16_tile_offset)
                {
                    canvas->AddImage
                    (
                        (ImTextureID)render_mgr->overrides_texture,
                        { draw_at.x + (x * 16 * scale),                 draw_at.y + (y * (16 * scale)) },
                        { draw_at.x + (x * 16 * scale) + (16 * scale),  draw_at.y + (y * 16 * scale) + (16 * scale) },
                        { entry.x_offset / 73.0f, entry.y_offset / 3.0f },
                        { (entry.x_offset + 1) / 73.0f, (entry.y_offset + 1) / 3.0f }
                    );
                }
            }
        }
    }

    static void _draw_row
    (
        Render::RenderManager*      render_mgr,
        ImDrawList*                 canvas,
        const ImVec2&               draw_at,
        const float                 scale,

        const NSMB::Tileset&        tileset,
        const Graphics::ObjectRow&  row,
        const sz_t                  y,
        const sz_t                  w,
        const sz_t                  x_offset = 0
    )
    {
        const
        Render::PixelBuffer* buffer = render_mgr->buffer_table.at
                                    (
                                        static_cast<Render::TextureSlot_>(6 + (sz_t)tileset.type)
                                    );

        sz_t                    tile_x, tile_y;
        ImVec2                  tile_uv_0 = { 0, 0 };
        ImVec2                  tile_uv_1 = { 0, 0 };

        std::vector<Graphics::ObjectTile> before_tiles;
        std::vector<std::reference_wrapper<const Graphics::ObjectTile>> repeat_tiles;
        std::vector<std::reference_wrapper<const Graphics::ObjectTile>> after_tiles;

        for (auto& tile : row.tiles)
        {
            // only check the first tile? sure, ig
            switch (tile.control)
            {
            case Graphics::ObjectTileControlByte::REPEAT_X:
                repeat_tiles.push_back(tile);
                break;

            case Graphics::ObjectTileControlByte::REPEAT_XY:
                repeat_tiles.push_back(tile);
                break;

            default:
                if (repeat_tiles.empty())
                {
                    before_tiles.push_back(tile);
                }
                else
                {
                    after_tiles.push_back(tile);
                }
                break;
            }
        }

        const sz_t before_size = before_tiles.size();
        const sz_t repeat_size = repeat_tiles.size();
        const sz_t  after_size = after_tiles.size();

        for (sz_t x = 0; x < w; x++)
        {
            if (repeat_tiles.empty())
            {
                _draw_tile
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    before_tiles.at(x % before_size), x + x_offset, y, w
                );
            }
            else if (x < before_size)
            {
                _draw_tile
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    before_tiles.at(x), x + x_offset, y, w
                );
            }

            else if (x >= w - after_size)
            {
                _draw_tile
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    after_tiles.at(x - w + after_size), x + x_offset, y, w
                );
            }
            else
            {
                _draw_tile
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    repeat_tiles.at((x - before_size) % repeat_size), x + x_offset, y, w
                );
            }
        }
    }

    static void _draw_section
    (
        Render::RenderManager*      render_mgr,
        ImDrawList*                 canvas,
        const ImVec2&               draw_at,
        const float                 scale,

        const NSMB::Tileset&        tileset,
        const RefSection&           section,   
        const sz_t&                 section_w,
        const sz_t&                 x,
        const sz_t&                 y,
        const sz_t&                 w,
        const sz_t&                 h,
        const bool&                 left,
        const bool&                 down
    )
    {
        // render from bottom up
        sz_t tile_x = x;
        sz_t row_y = y;
        for (auto& row : section | std::ranges::views::reverse )
        {
            if (row_y >= h)
                break;

            tile_x = x;
            if (left)
            {
                for (auto& tile : row.get().tiles | std::ranges::views::reverse)
                {
                    _draw_tile(render_mgr, canvas, draw_at, scale, tileset, tile, tile_x, row_y, section_w);
                    tile_x--;
                }
            }
            else
            {
                for (auto& tile : row.get().tiles)
                {
                    if (tile_x >= w)
                        break;
                    _draw_tile(render_mgr, canvas, draw_at, scale, tileset, tile, tile_x, row_y, section_w);
                    tile_x++;
                }
            }
            row_y--;
        }
    }

    static void draw_slope_object
    (
        Render::RenderManager*      render_mgr,
        ImDrawList*                 canvas,
        const ImVec2&               draw_at,
        const float                 scale,

        const NSMB::Tileset&        tileset,
        const Graphics::Object&     object,
        const sz_t                  w,
        const sz_t                  h
    )
    {
        RefSection main_section;
        RefSection sub_section;
        sz_t main_w = 0, sub_w = 0;

        RefSection* current_section = &main_section;
        sz_t* current_w = &main_w;
        for (auto& row : object.rows)
        {
            switch (row.control)
            {
            case Graphics::ObjectRowControlByte::SLOPE_CONTROL_F:
                current_section = &sub_section;
                current_w = &sub_w;
            default:
                current_section->push_back(row);
                *current_w = (((*current_w) > (row.tiles.size())) ? (*current_w) : (row.tiles.size()));
                break;
            }
        }

        Graphics::ObjectRowControlByte main_control = object.rows.at(0).control;
        
        s8  dx = 0, dy = 0;
        sz_t x = 0, y = 0;

        dx = main_w;
        dy = main_section.size();

        bool down = false;
        bool left = false;
        switch (main_control)
        {
        case Graphics::ObjectRowControlByte::SLOPE_NE:  
            y = h - 1;
            dy = -dy;
            left = false;
            down = false;
            break;
        
        case Graphics::ObjectRowControlByte::SLOPE_NW:
            y = h - 1;
            dy = -dy;
            x = w - 1;
            dx = -dx;
            left = true;
            down = false;
            break;

        case Graphics::ObjectRowControlByte::SLOPE_SE:
            left = false;
            down = true;
            break;

        case Graphics::ObjectRowControlByte::SLOPE_SW:
            x = w - 1;
            dx = -dx;
            left = true;
            down = true;
            break;
        }

        while (x < w && x >= 0 && y < h && y >= 0)
        {
            _draw_section(render_mgr, canvas, draw_at, scale, tileset, main_section, main_w, x, y, w, h, left, down);
            if (!sub_section.empty())
            {
                s8 xb = x + main_w;
                if (left)
                {
                    xb = x - main_w;
                }
                if (!down)
                    _draw_section(render_mgr, canvas, draw_at, scale, tileset, sub_section, sub_w, xb, y, w, h, left, down);
                else
                    _draw_section(render_mgr, canvas, draw_at, scale, tileset, sub_section, sub_w, xb, y - main_section.size() + 1, w, h, left, down);
            }

            x += dx;
            y += dy;
        }

    }

    static void draw_object
    (
        Render::RenderManager*          render_mgr,
        ImDrawList*                     canvas,
        const ImVec2&                   draw_at,
        const float                     scale,

        const NSMB::Tileset&            tileset,
        const sz_t&                     object_id,
        const sz_t&                     w,
        const sz_t&                     h
    )
    {
        const 
        Graphics::Object& object = tileset.objects.at(object_id);

        std::vector<std::reference_wrapper<const Graphics::ObjectRow>> before_rows;
        std::vector<std::reference_wrapper<const Graphics::ObjectRow>> repeat_rows;
        std::vector<std::reference_wrapper<const Graphics::ObjectRow>> after_rows;

        // calculate rows
        for (auto& row : object.rows)
        {
            // check for slope control
            switch (row.control)
            {
            case Graphics::ObjectRowControlByte::SLOPE_CONTROL_NONE:
                // continue with standard object 
                break;
            default:
                draw_slope_object(render_mgr, canvas, draw_at, scale, tileset, object, w, h);
                return;
            }

            // only check the first tile? sure, ig
            const Graphics::ObjectTile& tile = row.tiles.at(0);
            switch (tile.control)
            {
            case Graphics::ObjectTileControlByte::REPEAT_Y:
                repeat_rows.push_back(row);
                break;

            case Graphics::ObjectTileControlByte::REPEAT_XY:
                repeat_rows.push_back(row);
                break;

            default:
                if (repeat_rows.empty())
                {
                    before_rows.push_back(row);
                }
                else
                {
                    after_rows.push_back(row);
                }
                break;
            }
        }

        const sz_t before_size = before_rows.size();
        const sz_t repeat_size = repeat_rows.size();
        const sz_t  after_size = after_rows.size();

        for (sz_t y = 0; y < h; y++)
        {
            if ( repeat_rows.empty() ) 
            {
                _draw_row
                (
                    render_mgr, canvas, draw_at, scale, tileset, 
                    before_rows.at(y % before_size), y, w
                );
            }
            else if ( y < before_size )
            {
                _draw_row
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    before_rows.at(y), y, w
                );
            }

            else if ( y >= h - after_size )
            {
                _draw_row
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    after_rows.at(y - h + after_size), y, w
                );
            }
            else
            {
                _draw_row
                (
                    render_mgr, canvas, draw_at, scale, tileset,
                    repeat_rows.at((y - before_size) % repeat_size), y, w
                );
            }
        }
    }
}