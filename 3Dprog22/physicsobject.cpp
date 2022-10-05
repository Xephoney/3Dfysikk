#include "physicsobject.h"
#include "WorldCoordinator.h"
PhysicsObject::PhysicsObject() : OctahedronBall(3,"plainshader")
{

}

PhysicsObject::PhysicsObject(int index, std::string shader) : OctahedronBall(index, shader)
{
    m_curve=new kurve;
}

PhysicsObject::~PhysicsObject()
{

}

void PhysicsObject::init(GLint matrixUniform)
{
    OctahedronBall::init(matrixUniform);
    m_curve->init(matrixUniform);
}

void PhysicsObject::draw(const glm::mat4 &pMat)
{
    OctahedronBall::draw(pMat);
    m_curve->draw(pMat);
}

void PhysicsObject::tick(const float &dt)
{
    VisualObject::tick(dt);

    World world = World::getWorld();
    int tempIndex = world.getTriangleIndex(mPosition);


    if(world.distanceToTriangle(mPosition, tempIndex) < 1.f && tempIndex != -1)
    {
        currTriIndex = tempIndex;
        n = world.getNormal(currTriIndex);
    }
    else
        n = {0, 0, 0};

    if(currTriIndex != prevTriIndex && currTriIndex != -1)
    {
        if(currTriIndex != -1 && prevTriIndex != -1)
        {
            //Trekant til trekant
            glm::vec3 n1 = world.getNormal(prevTriIndex);
            glm::vec3 n2 = world.getNormal(currTriIndex);
            if(glm::angle(n1,n2) > 3.14f)
                n={0,0,0};
            else
                n = glm::normalize(n1 + n2);

            glm::vec3 d = m_velocity;
            m_velocity = d - 2*glm::dot(d, n)*n;
        }
        else
        {
            //ingen trekant til trekant
            glm::vec3 d = m_velocity;
            m_velocity = d - glm::dot(d, n)*n;
            hitGround=true;
        }
    }

    if(hitGround){
        if(timeSinceCheck>interval){
            glm::vec3 cPosition=mPosition;
            cPoints.push_back(cPosition);
            timeSinceCheck=0.f;
            //qDebug()<<"Created cPoint at :"<<cPosition.x<<" "<<cPosition.y<<" "<<cPosition.z;
            m_curve->update(cPosition);
        }
        timeSinceCheck+=dt;

    }

    calculateAcceleration();



    m_velocity += m_acceleration * m_weight* dt;
    mPosition +=  m_velocity * dt;
    prevTriIndex = currTriIndex;
}

void PhysicsObject::reset()
{
    OctahedronBall::reset();
    m_velocity = glm::vec3{0.f};
    currTriIndex = -1;
    prevTriIndex = -1;
}

void PhysicsObject::calculateAcceleration()
{   
    float G  = glm::length(g) * m_weight;
    G = abs(G);
    m_acceleration = glm::length(g) * glm::vec3(n.x * n.y, pow(n.y,2)-1, n.z*n.y );
}
