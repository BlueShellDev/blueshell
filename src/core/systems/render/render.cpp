#include "render.h"
#include "core/systems/console/console.h"
#include "texture.h"

BS::Render::TextureMap &BS::Render::GetTextures()
{
    static TextureMap textures;
    return textures;
}

void BS::Render::DestroyTextures()
{
    TextureMap &textures = GetTextures();
    for (auto &[name, texture] : textures)
    {
        if (IsTextureValid(texture))
            glDeleteTextures(1, &texture.id);
    }
    textures.clear();
}
