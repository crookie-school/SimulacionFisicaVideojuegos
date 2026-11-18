#pragma once

#include <string>
#include <gl/freeglut.h>

class Texture
{
public:
    Texture() : id(0){}
    ~Texture();
    Texture(Texture& other) = delete;
    Texture& operator= (Texture& other) = delete;
    Texture(Texture&& other);
    Texture& operator= (Texture&& other);

	Texture(const std::string& path);

    void bind() const { glBindTexture(GL_TEXTURE_2D, id); }
    void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

private:
	unsigned int id;
};

