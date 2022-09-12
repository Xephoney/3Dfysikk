#pragma once

#include "visualobject.h"

class GridPlane : public VisualObject
{
public:
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override;

    GridPlane() = delete;
    GridPlane(int size);
    ~GridPlane();
protected:
    bool displayVerticalAxis {true};
};

