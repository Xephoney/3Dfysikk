#pragma once
#include "visualobject.h"

class TriangleSurface : public VisualObject
{
public:
    TriangleSurface();
    TriangleSurface(std::string filnavn);
    ~TriangleSurface() override;
    void writeToFile(std::string filNavn);
    void readFile(std::string filnavn);
    void readCompressed(std::string filnavn);
    virtual void init() override;
    virtual void draw(const glm::mat4& pMat) override;

    glm::vec3 triangleNormal(int triangleIndex);
    void trianglePositions(int triangleIndex, std::vector<glm::vec3>& vec);


    void move(float x, float y, float z) override { mPosition.x = x; mPosition.y = y; mPosition.z = z;};
    void makeQuad(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c, const glm::vec3& d);
    void makeQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3& farge);
    void makeTriangle(const int &indx0, const int &indx1, const int &indx2);
    int triangleCount = 0;
protected:
    void makeTriangle(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c);
    void makeTriangle(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c, const glm::vec3& farge);
    bool tooClose(Vertex a, Vertex b);

private:
    void construct();
    void constructIndices();
    float func(const float x, const float y);
    float max[3]={-FLT_MAX,-FLT_MAX,-FLT_MAX}; // holder største XYZ verdier i innlest data
    float min[3]={FLT_MAX,FLT_MAX,FLT_MAX}; // holder minste XYZ verdier i innlest data
    float minDist=50.f; // Minimum distanse mellom 2 datapunkter. brukt for å optimalisere data
    float resolution=1.f;
};
