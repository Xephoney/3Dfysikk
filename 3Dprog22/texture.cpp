//Følger tutorial fra The Cherno på YouTube


#include "texture.h"

Texture::Texture() {}


Texture::Texture(std::string dir, std::string path, aiTextureType type) : dir(dir), path(path), type(type)
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::load(bool flip, GLint parameter)
{
    stbi_set_flip_vertically_on_load(flip);
    std::string newPath = dir + "/" + path;
    m_LocalBuffer = stbi_load(newPath.c_str(), &m_width, &m_height, &m_BPP,4);

    if(!m_LocalBuffer)
    {
        qDebug() << newPath.c_str() << " | couldn't load";
    }
    //qDebug() << newPath.c_str() << " | w : " << m_width << ", h " << m_height;
    initializeOpenGLFunctions();
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 m_width,
                 m_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 m_LocalBuffer );
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,id);

    if(m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

void Texture::Bind(unsigned int slot)
{
    initializeOpenGLFunctions();
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
    initializeOpenGLFunctions();
    glBindTexture(GL_TEXTURE_2D,0);
}
