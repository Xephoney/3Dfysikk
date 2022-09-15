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
    caluclateAcceleration();
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
    glm::vec3 v12 = t.v2-t.v1;
    glm::vec3 v13 = t.v3-t.v1;
    glm::vec3 n = glm::cross(v12,v13); //normal vektoren til planet/vertex'en

    glm::vec3 p = {0,1,0};  //verdens opp vektor
    n =glm::normalize(n);
    qDebug() << "normal " << n.x << n.y << n.z;

    float angle =  acos(glm::dot(p,n));
    qDebug() << "angle " << angle;

    //Stor G har ingenting med vinkelen å gjøre
    //glm::vec3 G = glm::vec3(g.x*m_weight  * cos(angle), g.y*m_weight  * cos(angle), g.z*m_weight  * cos(angle));
    glm::vec3 G  = g;

    //glm::vec3 N = glm::vec3(G.length()*cos(angle) * n.x, G.length()*cos(angle) * n.y, G.length()*cos(angle) * n.z);

    //float a = m_weight*g.length()*sin(angle); //dette er lengden som akselerasjonen skal ha på dette planet.
    //G *= sin(angle);
    m_acceleration = G+n;


    //Kjøre på med samme som over, men med G_y og G

    //m_acceleration= gravity*(glm::vec3(n.x*n.z,n.y*n.z,pow(n.z,2)-1));
}
