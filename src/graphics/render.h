#pragma once

// file: render.h
// purpose: functions for rendering pixel data to opengl textures

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "json.hpp"

#include "../nsmb/tileset.h"
#include "../util/util.hpp"

namespace Render
{
    /* struct to hold basic metadata of a pixel array */
    struct PixelBuffer
    {
        GLuint* link_to_texture;
        
        sz_t width      = 0;
        sz_t height     = 0;
        std::vector<u16> data;
    };

    /* enumeration class to keep track of texture names */
    enum TextureSlot_
    {
        TextureSlot_NCG_J,
        TextureSlot_NCG_M,
        TextureSlot_NCG_S,

        TextureSlot_NCL_J,
        TextureSlot_NCL_M,
        TextureSlot_NCL_S,

        TextureSlot_PNL_J,
        TextureSlot_PNL_M,
        TextureSlot_PNL_S,
    };

    struct RenderManager
    {
        int                                     number_of_textures_to_gen   = 0;
        std::vector<GLuint>                     texture_list                = { 0 };
        std::map<TextureSlot_, PixelBuffer*>    buffer_table;

        /* this character will be used when trying to render a character that doesn't exist */
        Graphics::Character                     null_character;

        nlohmann::json                          overrides_json;
        GLuint                                  overrides_texture = 0;
        GLuint                                  pipes_texture     = 0;
    };

    /* opengl related functions */
    void initialize_opengl_textures(RenderManager* render_mgr);
    void initialize_pixmap_table(RenderManager* render_mgr);
    void bind_buffers_to_gpu(RenderManager* render_mgr);

    void load_image_to_texture(RenderManager* render_mgr, GLuint* render_to, std::string image_path);

    /* pixbuf related functions */
    void render_tileset
    (
        RenderManager* render_mgr, 
        NSMB::Tileset& tileset
    );

    void render_character
    (
        PixelBuffer*                        buffer,
        Graphics::Character&                character,
        Graphics::Palette&                  palette,
        const sz_t&                         at_x,
        const sz_t&                         at_y,
        const bool&                         flip_x,
        const bool&                         flip_y
    );

    void render_characters
    (
        PixelBuffer*                        buffer,
        std::vector<Graphics::Character>&   characters,
        std::vector<Graphics::Palette>&     palettes
    );

    void render_metatiles
    (
        PixelBuffer* buffer,
        const sz_t& char_index_offset,
        const sz_t& pal_index_offset,
        Graphics::Character&                null_character,
        std::vector<Graphics::Character>&   characters,
        std::vector<Graphics::Palette>&     palettes,
        std::vector<Graphics::Map16Tile>&   metatiles
    );
}
