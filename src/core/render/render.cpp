#include "render.h"
#include "core/console.h"
#include "texture.h"

// returns a static instance of a TextureMap, use this as a global container of OpenGL textures
Core::TextureMap &Core::GetTextures()
{
    static TextureMap textures;
    return textures;
}

// deletes all OpenGL textures from the TextureMap returned by GetTextures()
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
