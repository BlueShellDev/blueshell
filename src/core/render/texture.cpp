#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "core/console.h"

// wrapper around MakeTextureFromBytes, uses stb_image to extract bytes from a typical image format
Core::Texture Core::MakeTextureFromFile(const std::string &filePath, Texture::Format format)
{
    Texture newTexture{};
    u8 *data = stbi_load(filePath.c_str(), &newTexture.width, &newTexture.height, nullptr, 4);
    if (data == nullptr)
    {
        Console::Global().Err("Failed to load texture from file: {}", filePath);
        return newTexture;
    }
    Console::Global().Log("Loading texture from file: {} ({}x{})...", filePath, newTexture.width, newTexture.height);
    stbi_image_free(data);
    return MakeTextureFromBytes(data, newTexture.width, newTexture.height, format);
}

// creates an OpenGL texture from specified bytes and format, currently only RGBA8888 & BGR565
Core::Texture Core::MakeTextureFromBytes(const u8 *data, i32 width, i32 height, Texture::Format format)
{
    Texture newTexture{};
    newTexture.width = width;
    newTexture.height = height;
    newTexture.format = format;

    glGenTextures(1, &newTexture.id);
    glBindTexture(GL_TEXTURE_2D, newTexture.id);
    switch (format)
    {
    case Texture::Format::RGBA8888:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        break;
    case Texture::Format::BGR565:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV, data);
        break;
    default:
        Console::Global().Err("Unsupported texture format specified! Didn't bind any texture.");
        return Texture{};
    }
    Console::Global().Log("Created OpenGL texture (ID: {}) with dimensions {}x{} and format {}", newTexture.id, width, height, format == Texture::Format::RGBA8888 ? "RGBA8888" : "BGR565");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return newTexture;
}