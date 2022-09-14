#pragma once
#include "octahedronball.h"

class PhysicsObject : public  OctahedronBall
{
private:
    glm::vec3 m_acceleration { 0,-9.810,0 };
    glm::vec3 m_velocity{ 0 };
    const float gravity { 9.81 };


public:
    PhysicsObject();
    PhysicsObject(int, std::string shader);
    ~PhysicsObject();
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override;
    void tick(const float& dt) override;
};
