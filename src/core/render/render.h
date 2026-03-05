#pragma once
#include <string>
#include <unordered_map>
#include "core/render/texture.h"

namespace Core
{
    using TextureMap = std::unordered_map<std::string, Texture>;

    TextureMap &GetTextures();
    void DestroyTextures();
}