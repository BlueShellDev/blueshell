#pragma once
#include <string>
#include <unordered_map>
#include "core/systems/render/texture.h"

namespace BS
{
    namespace Render
    {
        using TextureMap = std::unordered_map<std::string, Texture>;

        TextureMap &GetTextures();
        void DestroyTextures();
    };
}