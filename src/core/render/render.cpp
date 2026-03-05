#include "render.h"
#include "core/console.h"
#include "texture.h"

Core::TextureMap &Core::GetTextures()
{
    static TextureMap textures;
    return textures;
}

void Core::DestroyTextures()
{
    TextureMap &textures = GetTextures();
    for (auto &[name, texture] : textures)
    {
        if (IsTextureValid(texture))
            glDeleteTextures(1, &texture.id);
    }
    textures.clear();
}
