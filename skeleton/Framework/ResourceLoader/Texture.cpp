#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture::~Texture()
{
    if(id) glDeleteTextures(1, &id);

    id = 0;
}

Texture::Texture(Texture&& other)
{
    id = other.id;
    other.id = 0;
}

Texture& Texture::operator=(Texture&& other)
{
    if (&other == this) return *this;

    if (id) glDeleteTextures(1, &id);

    id = other.id;
    other.id = 0;

    return *this;
}

Texture::Texture(const std::string& path) {

    stbi_set_flip_vertically_on_load(true);

    int x, y, n;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 3);

    if (!data) {
        std::cout << "No se pudo cargar " << path << ": " << stbi_failure_reason() << '\n';
        return;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
        GL_TEXTURE_2D,      // Target
        0,                  // Mipmap level (0 is base)
        GL_RGB,             // Internal format
        x,                  // Width
        y,                  // Height
        0,                  // Border (must be 0)
        GL_RGB,             // Pixel format (source data format)
        GL_UNSIGNED_BYTE,   // Data type
        data           // Pointer to pixel data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}
