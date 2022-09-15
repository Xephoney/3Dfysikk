#include "physicsobject.h"
#include "WorldCoordinator.h"
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

    World world = World::getWorld();
    currTriIndex = world.getTriangleIndex(mPosition);

    if(currTriIndex >= 0)
    {
        n = world.getNormal(currTriIndex);
        if(world.distanceToTriangle(mPosition, currTriIndex) < 1.f)
            qDebug() << "!!!!!!!!!!!! Collision !";
    }

    if(currTriIndex != prevTriIndex && prevTriIndex != -1)
    {
        glm::vec3 n1 = world.getNormal(prevTriIndex);
        glm::vec3 n2 = world.getNormal(currTriIndex);
        n = glm::normalize(n1 + n2);

        qDebug() << "Ball over transition from index " << prevTriIndex << " to " << currTriIndex;
    }

    caluclateAcceleration();

    m_velocity += m_acceleration * dt;
    mPosition +=  m_velocity * dt;
}

void PhysicsObject::caluclateAcceleration()
{   
//    glm::vec3 v12 = t.v2-t.v1;
//    glm::vec3 v13 = t.v3-t.v1;
//    glm::vec3 n = glm::cross(v12,v13); //normal vektoren til planet/vertex'en

    glm::vec3 k = {0,1,0};    //verdens opp vektor
    qDebug() << "normal " << n.x << n.y << n.z;

    float skalar =  glm::dot(n,k);
    qDebug() << "skalar " << skalar;

    //Stor G har ingenting med vinkelen å gjøre
    //glm::vec3 G = glm::vec3(g.x*m_weight  * cos(angle), g.y*m_weight  * cos(angle), g.z*m_weight  * cos(angle));
    float G  = glm::length(g) * m_weight;
    G = abs(G);

//    glm::vec3 G  = g * m_weight;
//    float angle = (glm::dot(n,k))/(glm::length(n)+glm::length(k));

//    glm::vec3 N = n * (glm::length(G) * acos(angle)) ;
    //glm::vec3 N = glm::vec3(G.length()*cos(angle) * n.x, G.length()*cos(angle) * n.y, G.length()*cos(angle) * n.z);

    //float a = m_weight*g.length()*sin(angle); //dette er lengden som akselerasjonen skal ha på dette planet.
    //G *= sin(angle);

    //m_acceleration = n * G * skalar + g;
    m_acceleration = glm::length(g) * glm::vec3(n.x * n.y, pow(n.y,2)-1, n.z*n.y );
    //m_acceleration = 1/m_weight *(N+G);

    //(glm::normalize(G)+n) * glm::length(g) * (float)sin(angle);


    //Kjøre på med samme som over, men med G_y og G
}
