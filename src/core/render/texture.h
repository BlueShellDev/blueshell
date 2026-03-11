#pragma once
#include <string>
#include <unordered_map>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "core/types.h"

namespace Core
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

        inline Texture() : id(0), width(0), height(0) {} 
        inline Texture(u32 id, i32 width, i32 height, Format format) : id(id), width(width), height(height), format(format) {}
    };

    // wrapper around glIsTexture, returns false if Texture::id is not a real texture
    inline bool IsTextureValid(const Texture &texture) { return glIsTexture(texture.id); };
    // wrapper around MakeTextureFromBytes, uses stb_image to extract bytes from a typical image format
    Texture MakeTextureFromFile(const std::string &filePath, Texture::Format format);
    // creates an OpenGL texture from specified bytes and format, currently only RGBA8888 & BGR565
    Texture MakeTextureFromBytes(const u8 *data, i32 width, i32 height, Texture::Format format);
};