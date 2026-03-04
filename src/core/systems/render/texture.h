#pragma once
#include <string>
#include <unordered_map>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "core/types.h"

namespace BS
{
    namespace Render
    {
        struct Texture
        {
            u32 id;
            i32 width;
            i32 height;
            enum class Format
            {
                RGBA8888,
                BGR565
            } format{Format::RGBA8888};

            Texture() : id(0), width(0), height(0) {}
            Texture(u32 id, i32 width, i32 height, Format format) : id(id), width(width), height(height), format(format) {}
        };

        inline bool IsTextureValid(const Texture &texture) { return glIsTexture(texture.id); };
        Texture MakeTextureFromFile(const std::string &filePath, Texture::Format format);
        Texture MakeTextureFromBytes(const u8 *data, i32 width, i32 height, Texture::Format format);
    };
}