#include "physicsobject.h"

PhysicsObject::PhysicsObject() : OctahedronBall(3,"plainshader")
{

}

PhysicsObject::PhysicsObject(int index, std::string shader) : OctahedronBall(index, shader)
{

}

PhysicsObject::~PhysicsObject()
{

}

void PhysicsObject::init(GLint matrixUniform)
{
    OctahedronBall::init(matrixUniform);
}

void PhysicsObject::draw(const glm::mat4 &pMat)
{
    OctahedronBall::draw(pMat);
}

void PhysicsObject::tick(const float &dt)
{
    VisualObject::tick(dt);
    m_velocity = m_velocity + m_acceleration * dt;
    mPosition = mPosition + m_velocity * dt;
}
