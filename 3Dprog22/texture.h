#pragma once
#include <string>
#include <assimp/scene.h>
#include <src/vendor/stb_image/stb_image.h>
#include <QOpenGLFunctions_4_1_Core>

// Klassen er bygd på fra denne tutorialen :
// link : https://www.youtube.com/watch?v=jjlOLKkhckU
// Med modifikasjoner gjort som en dir og path. + TextureType fra Assimp

class Texture : protected QOpenGLFunctions_4_1_Core
{
    std::string m_FilePath;
    unsigned char* m_LocalBuffer {nullptr};
    int m_width, m_height, m_BPP;

public:
    Texture();
    Texture(std::string dir, std::string path, aiTextureType type = aiTextureType::aiTextureType_DIFFUSE);
    ~Texture();

    void load(bool flip, GLint parameter = GL_LINEAR_MIPMAP_LINEAR);
    void Bind(unsigned int slot = 0);
    void Unbind();

    unsigned int id {0};
    std::string dir;
    std::string path;
    aiTextureType type;

    inline int getWidth() { return m_width; }
    inline int getHeight() { return m_height; }
};

