#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "Vertex.h"
#include "shader.h"


class Cubemap : public QOpenGLFunctions_4_1_Core
{
public:
    Cubemap();
    ~Cubemap();

    void init();
    void draw(const glm::mat4 &projMatrix, const glm::mat4 &viewMatrix);
    void loadTextures(std::string Path);

protected:
    void CreateBox();
    GLuint mVAO {0};
    GLuint mVBO {0};
    GLuint mIBO {0};

    Shader* mShader {nullptr};
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;

    unsigned int cubemapTextureID;
};

#endif // CUBEMAP_H
