#pragma once

#include "visualobject.h"

class VisualLineObject : public VisualObject
{
public:
    VisualLineObject();
    ~VisualLineObject();
    //VisualObject overrides
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override;

    virtual glm::vec3 positionOnLine(const float& t)=0;
    void setLineColor(const glm::vec4& col);
};

