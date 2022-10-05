#pragma once
#include "octahedronball.h"
#include "toolbox.h"
#include "kurve.h"

class PhysicsObject : public  OctahedronBall
{
private:
    glm::vec3 m_acceleration { 0 };
    glm::vec3 m_velocity{ 0 };
    const glm::vec3 g {0, -9.81,0 };
    float m_weight {0.5};
     glm::vec3 n; //triangle normal
     kurve* m_curve;

    //Triangle t;
    void calculateAcceleration();
    float TriangleDegree;
    float timeSinceCheck=10.f;
    float interval=2.5f;
public:
    PhysicsObject();
    PhysicsObject(int, std::string shader);
    ~PhysicsObject();
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override;
    void tick(const float& dt) override;
    void reset() override;

    std::vector<glm::vec3> cPoints;

    bool hitGround=false;
};
