#pragma once
#include <string>
#include <QOpenGLFunctions_4_1_Core>

class Texture : protected QOpenGLFunctions_4_1_Core
{
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_width, m_height, m_BPP;

    unsinged int m_RenderID{0};
public:
    Texture(const std::string& filePath);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int getWidth() { return m_width; }
    inline int getHeight() { return m_height; }
};
