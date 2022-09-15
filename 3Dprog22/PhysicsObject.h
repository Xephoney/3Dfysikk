#pragma once
#include "octahedronball.h"
#include "toolbox.h"

class PhysicsObject : public  OctahedronBall
{
private:
    glm::vec3 m_acceleration { 0 };
    glm::vec3 m_velocity{ 0 };
    const glm::vec3 g {0, -9.81,0 };
    float m_weight {0.5};
     glm::vec3 n; //triangle normal
//    struct Triangle {

//        glm::vec3 v1 = {-8.0, 1.0, -8.0} ;
//        glm::vec3 v2 =  {-8.0, 5.0, 8.0};
//        glm::vec3 v3 = {0.0, 3.0, -8.0};
//    };

    //Triangle t;
    void calculateAcceleration();
    float TriangleDegree;
public:
    PhysicsObject();
    PhysicsObject(int, std::string shader);
    ~PhysicsObject();
    void init(GLint matrixUniform) override;
    void draw(const glm::mat4& pMat) override;
    void tick(const float& dt) override;
};
