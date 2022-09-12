#pragma once

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "Vertex.h"
#include "shader.h"
#include "texture.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "bounds.h"

class VisualObject : public QOpenGLFunctions_4_1_Core
{
public:
    VisualObject();

    VisualObject(std::string, CollisionResponse = CollisionResponse::GHOST);
    ~VisualObject();
    //init og draw er nå "valgfri" å override. Hvis de ikke spesifiserer egen
    // så vil basevesjonen bli kalt
    virtual void init();
    virtual void init(GLint matrixUniform);
    virtual void draw(const glm::mat4& pMat);
    virtual void tick(const float& dt);
    virtual void move(float x, float y, float z){mPosition += glm::vec3(x,y,z);};
    virtual void move(float dt){mPosition.x += dt;};
    virtual void reset();

    //Flytt definisjonen til .cpp
    void setPosition2D(const std::pair<double,double>& p) {mPosition = {p.first,0,p.second};}
    void setPosition (const glm::vec3& pos) { mPosition = pos;}
    void setRotation (const glm::vec3& rot) { mRotation = rot;}
    void Translate   (const glm::vec3& dxyz) {mPosition += dxyz;};
    void Translate   (const float& dx,const float& dy,const float& dz) {mPosition += glm::vec3(dx,dy,dz);};
    void setScale    (const glm::vec3& scale) { mScale = scale; }
    void setScale    (const float& factor) { mScale = {factor,factor,factor}; }
    void setScale    (const int& factor) { mScale = {factor,factor,factor}; }
    void setNavn     (const std::string& str){ mNavn = str; };
    void setColor    (const float& r,const float& g, const float& b) { for(Vertex& v : mVertices){v.setVertexColor(r,g,b);};}
    void setColor    (const float& r,const float& g, const float& b, bool recursive);
    void setShader   (Shader* shader);
    void setShader   (const std::string& shader);
    void normalizeVertexNormals();
    void rotateAroundAxis(const float& degrees, const glm::vec3& axis)
    {
        mRotation += (axis * glm::radians(degrees));
    }

    const std::pair<double,double> getPosition2D() const { return std::pair<double,double>(mPosition.x,mPosition.z);};
    const glm::vec3&    getPosition()   const { return mPosition; };
    const glm::vec3&    getRotation()   const { return mRotation; };
    const glm::vec3&    getScale()      const { return mScale; };
    const std::string&  getNavn()       const { return mNavn; };

    bool bCollectable;
    glm::mat4 mModelMatrix{1.f};
    int shaderID {-1};

    CollisionType ct;
    CollisionPolygon cp;

    CollisionResponse cr;
    std::vector<Texture*> mTextures ;
    std::vector<VisualObject*> mSubObjects;

    void ToggleDebugDisplay(bool);

    bool DebugDisplay = true;
    void findMinMax(glm::vec3& min, glm::vec3& max);
protected:
    Shader* mShader {nullptr};

    std::string mNavn{"objekt"};
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;


    GLuint mVAO {0};
    GLuint mVBO {0};
    GLuint mIBO {0};

    GLint mMatrixUniform{0};

    glm::vec3 mPosition{0,0,0};
    glm::vec3 mRotation{0,0,0};
    glm::vec3 mScale{1,1,1};

    glm::mat4 getTransform();
    void fetchVertices(std::vector<glm::vec3>& vertexPos);
    void findMinMax(glm::vec3& min, glm::vec3& max, std::vector<glm::vec3>& vertexPos);
    void initCollisions();
    float timeSinceCreation {0};
private:
    glm::vec3 startPos  {0,0,0};
    glm::vec3 startRot  {0,0,0};
    glm::vec3 startScale{0,0,0};
    std::vector<glm::vec3> vertexPositions;
};
