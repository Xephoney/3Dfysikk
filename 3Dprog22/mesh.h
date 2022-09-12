#pragma once

#include "visualobject.h"

class Mesh : public VisualObject
{
private:
    bool loadTex = true;
public:
    Mesh();
    ~Mesh();
    void init() override;
    void draw(const glm::mat4& pMat) override;
    void drawcam(const glm::mat4& mat);

    // Assimp importering har jeg tatt fra denne videoen
    // Link : https://www.youtube.com/watch?v=jjlOLKkhckU
    void loadMesh(std::string filePath, bool loadTexture = true);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void loadTextures (aiMaterial* mat,  aiTextureType type);

    std::string lastetMeshPath;
};

