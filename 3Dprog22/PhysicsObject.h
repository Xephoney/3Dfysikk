#pragma once

#include "octahedronball.h"

class PhysicsObject : public OctahedronBall {

private:
    void physicsTick();

    float m_acceleration { 0 };
    float m_speed{ 0 };

    const float gravity { 9.81 };
public:
   PhysicsObject();
   PhysicsObject(std::string &shader);
   ~PhysicsObject();
//   void init(GLint matrixUniform) override;
//   void draw(const glm::mat4& pMat) override;
};
