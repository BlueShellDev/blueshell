#include "render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Render::initialize_opengl_textures(Render::RenderManager* render_mgr)
{
    // size texture list accordingly
    render_mgr->texture_list.resize
    (
        render_mgr->number_of_textures_to_gen
    );

    glGenTextures
    (
        render_mgr->number_of_textures_to_gen, 
        render_mgr->texture_list.data()
    );

    // confiugure paramaters for each texture
    for (GLuint& name : render_mgr->texture_list)
    {
        glBindTexture(GL_TEXTURE_2D, name);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return;
}

void Render::initialize_pixmap_table(Render::RenderManager* render_mgr)
{
    // loop through all the textures in the list and create table entries for them
    for (std::size_t i = 0; i < render_mgr->texture_list.size(); i++)
    {
        // alais index to enum
        Render::TextureSlot_ current_slot = static_cast<Render::TextureSlot_>(i);

        // insert a blank pixel buffer at each slot
        render_mgr->buffer_table.insert
        (
            { current_slot, new PixelBuffer() }
        );

        // define the alias to the texture in the pixbuf
        render_mgr->buffer_table.at(current_slot)->link_to_texture 
            = &render_mgr->texture_list.at(i);
    }

    return;
}

/* call this AFTER you have rendered */
void Render::bind_buffers_to_gpu(Render::RenderManager* render_mgr)
{
    for (const std::pair entry : render_mgr->buffer_table)
    {
        glBindTexture(GL_TEXTURE_2D, *entry.second->link_to_texture);
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGB5_A1,
            entry.second->width,
            entry.second->height,
            0,
            GL_RGBA,
            GL_UNSIGNED_SHORT_1_5_5_5_REV,
            entry.second->data.data()
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return;
}

void Render::load_image_to_texture(Render::RenderManager* render_mgr, GLuint* render_to, std::string image_path)
{
    int width = 0, height = 0;

    u8* image_data = stbi_load(image_path.c_str(), &width, &height, NULL, 4);

    glGenTextures(1, render_to);
    glBindTexture(GL_TEXTURE_2D, *render_to);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
}

void Render::render_tileset(Render::RenderManager* render_mgr, NSMB::Tileset& tileset)
{
    PixelBuffer* ncg_buffer = nullptr;
    PixelBuffer* ncl_buffer = nullptr;
    PixelBuffer* pnl_buffer = nullptr;

    // determine what buffer to place these graphics into

    switch (tileset.type)
    {
    case NSMB::TilesetType_Jyotyu:
        ncg_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCG_J);
        ncl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCL_J);
        pnl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_PNL_J);

        render_characters
        (
            ncg_buffer,
            tileset.characters,
            tileset.palettes
        );

        render_metatiles
        (
            pnl_buffer,
            0, 0,
            render_mgr->null_character,
            tileset.characters,
            tileset.palettes,
            tileset.metatiles
        );
        break;

    case NSMB::TilesetType_Main:
        ncg_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCG_M);
        ncl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCL_M);
        pnl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_PNL_M);

        render_characters
        (
            ncg_buffer,
            tileset.characters,
            tileset.palettes
        );

        render_metatiles
        (
            pnl_buffer,
            192, 2,
            render_mgr->null_character,
            tileset.characters,
            tileset.palettes,
            tileset.metatiles
        );
        break;

    case NSMB::TilesetType_Sub:
        ncg_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCG_S);
        ncl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_NCL_S);
        pnl_buffer = render_mgr->buffer_table.at(Render::TextureSlot_PNL_S);
        
        render_characters
        (
            ncg_buffer,
            tileset.characters,
            tileset.palettes
        );

        render_metatiles
        (
            pnl_buffer,
            640, 6,
            render_mgr->null_character,
            tileset.characters,
            tileset.palettes,
            tileset.metatiles
        );
        break;

    default:
        throw std::out_of_range("trying to render an unknown tileset type");
        break;
    }
    // actually render now 
    // todo: (figure out logistics come back later)
    return;
}

void Render::render_character
(
    Render::PixelBuffer*                buffer,
    Graphics::Character&                character,
    Graphics::Palette&                  palette,
    const sz_t&                         at_x,
    const sz_t&                         at_y,
    const bool&                         flip_x,
    const bool&                         flip_y
)
{
    sz_t buffer_iter = 0;
    for (sz_t y = 0; y < 8; y++)
    {
        for (sz_t x = 0; x < 8; x++)
        {
            buffer_iter = Util::xy_to_i
            (
                x + at_x * 8,
                y + at_y * 8,
                buffer->width
            );

            buffer->data.at(buffer_iter) = palette.colors.at
            (
                character.pixels
                [flip_y ? (7 - y) : (y)]
                [flip_x ? (7 - x) : (x)]
            );
        }
    }
}

void Render::render_characters
(
    Render::PixelBuffer*                buffer, 
    std::vector<Graphics::Character>&   characters,
    std::vector<Graphics::Palette>&     palettes
)
{
    buffer->width   = 32 * 8;

    sz_t    draw_at_x       = 0, 
            draw_at_y       = 0,
            buffer_iter     = 0,
            bitmap_height   = (characters.size() / 32) * 8,
            full_height = bitmap_height * palettes.size();

    buffer->height = full_height;
    buffer->data.resize(buffer->width * buffer->height);

    for (auto& palette : palettes)
    {
        for (auto& character : characters)
        {
            Util::i_to_xy
            (
                character._id + (characters.size() * palette._id),
                32,
                draw_at_x,
                draw_at_y
            );

            render_character
            (
                buffer,
                character,
                palette,
                draw_at_x,
                draw_at_y,
                false,
                false
            );
        }
    }
}

void Render::render_metatiles
(
    PixelBuffer* buffer, 
    const sz_t& char_index_offset,
    const sz_t& pal_index_offset,
    Graphics::Character&                null_character,
    std::vector<Graphics::Character>&   characters, 
    std::vector<Graphics::Palette>&     palettes, 
    std::vector<Graphics::Map16Tile>&   metatiles
)
{
    buffer->width = 16 * 16;
    buffer->height = (metatiles.size() / 16) * 16;
    buffer->data.resize(buffer->width * buffer->height);

    sz_t    draw_at_x   = 0,
            draw_at_y   = 0,
            quarter_x   = 0,
            quarter_y   = 0,
            buffer_iter = 0;

    for (auto& metatile : metatiles)
    {
        Util::i_to_xy
        (
            metatile._id,
            16,
            draw_at_x,
            draw_at_y
        );

        // handle case for drawing nsbtx instead of 4x4 chars

        for (auto& quarter : metatile.quarters)
        {
            sz_t real_character_id  = quarter.character_id  - char_index_offset;
            sz_t real_palette_id    = quarter.palette_id    - pal_index_offset;

            if (real_palette_id >= palettes.size())
                real_palette_id = 0;

            Graphics::Character& character = (real_character_id < characters.size()) ? 
                characters.at(real_character_id % characters.size()) : 
                (null_character);

            Graphics::Palette& palette = palettes.at(real_palette_id);

            Util::i_to_xy
            (
                quarter._id,
                2,
                quarter_x,
                quarter_y
            );

            Render::render_character
            (
                buffer,
                character,
                palette,
                (draw_at_x * 2) + quarter_x,
                (draw_at_y * 2) + quarter_y,
                quarter.h_flip,
                quarter.v_flip
            );
        }
    }
}
