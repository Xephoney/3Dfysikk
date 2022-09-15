#ifndef WORLDCOORDINATOR_H
#define WORLDCOORDINATOR_H

#include <glm/glm.hpp>
#include "visualobject.h"
#include "toolbox.h"
#include <fstream>

class World{
public:
    static World& getWorld();

    class TriangleSurface* surface {nullptr};


    glm::vec3 find()
    {

    }
    float distanceToTriangle(glm::vec3 pos, int triangleIndex);
    glm::vec3 getNormal(int TriIndex);
    int getTriangleIndex(const glm::vec3& position);

    glm::vec3 barycentricCoordinates(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 point)
    {
        p1.y=0;
        p2.y=0;
        p3.y=0;
        point.y=0;
        glm::vec3 p12 = p2 - p1;
        glm::vec3 p13 = p3 - p1;

        glm::vec3 Kryss = glm::cross(p13, p12);
        float areal_123 = glm::length(Kryss);

        glm::vec3 returnVec;

        //u
        glm::vec3 p = p2 - point;
        glm::vec3 q = p3 - point;
        Kryss = glm::cross(q, p);
        returnVec.x = Kryss.y / areal_123;
        //qDebug()<<returnVec.x;
        //v
        p = p3 - point;
        q = p1 - point;
        Kryss = glm::cross(q, p);
        returnVec.y = Kryss.y / areal_123;
        //qDebug()<<returnVec.y;
        //w
        p = p1 - point;
        q = p2 - point;
        Kryss = glm::cross(q, p);
        returnVec.z = Kryss.y / areal_123;

        //qDebug()<<returnVec.z;
        return returnVec;
    }
};

inline World& World::getWorld()
{
    static World INSTANCE;
    return INSTANCE;
}

#endif // WORLDCOORDINATOR_H
