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

void PhysicsObject::caluclateAcceleration()
{
//    if(t.v1.y==t.v2.y && t.v2.y==t.v3.y)
//        TriangleDegree = 0;
//    else if (t.v1.y == t.v2.y)
//    {
//        if(t.v1.y > t.v3.y)
//            TriangleDegree = 1;
//    }

    //finn normalvektor till planet

    //prosjekter normalvektor på verdens oppvektor
     //g [nx · nz, ny · nz, n2 z − 1]
    glm::vec3 v12 = t.v1-t.v2;
    glm::vec3 v13 = t.v1-t.v3;
    glm::vec3 n = glm::cross(v12,v13);

    glm::vec3 k;  //Vektor mellom triangle til ballens sendtrum
    glm::normalize(k);
    glm::normalize(n);

     float angle =  acos(glm::dot(k,n));
     glm::vec3 G = glm::vec3(g.x*m_weight  * cos(angle),g.y*m_weight  * cos(angle),g.z*m_weight  * cos(angle));
     glm::vec3 N = glm::vec3(G.length()*cos(angle) * n.x, G.length()*cos(angle) * n.y, G.length()*cos(angle) * n.z);

    float a = m_weight*g.length()*sin(angle); //dette er lengden som akselerasjonen skal ha på dette planet.

   m_acceleration = G+n;



    //Kjøre på med samme som over, men med G_y og G

    //m_acceleration= gravity*(glm::vec3(n.x*n.z,n.y*n.z,pow(n.z,2)-1));
}
