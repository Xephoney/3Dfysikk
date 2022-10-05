#pragma once
#include "visuallineobject.h"

class kurve : public VisualObject
{
public:
    kurve();
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override ;
    std::vector<glm::vec3> cPoints;
    std::vector<int> skjoter;
    void update(glm::vec3 inPos);

private:
    float bFunc(int i, int d, float t);
    float wFunc(int i, int d, float t);
    void reCalc();
    int iterator = 1;

};

