#ifndef WORLDCOORDINATOR_H
#define WORLDCOORDINATOR_H

#include <glm/glm.hpp>
#include "visualobject.h"

class World{
public:
    static World& getWorld();

    float find(VisualObject* p){
        //TODO: make this lmao
        return 0;
    }
    glm::vec3 barycentricCoordinates(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 p3, const glm::vec2 point)
    {
        glm::vec2 p12 = p2 - p1;
            glm::vec2 p13 = p3 - p1;

            glm::vec3 Kryss = glm::cross(glm::vec3{p13.x,0,p13.y}, glm::vec3{p12.x,0,p12.y});
            float areal_123 = glm::length(Kryss);

            glm::vec3 returnVec;

            //u
            glm::vec2 p = p2 - point;
            glm::vec2 q = p3 - point;
            Kryss = glm::cross(glm::vec3{q.x, 0, q.y},glm::vec3{p.x, 0, p.y});
            returnVec.x = Kryss.y / areal_123;
    //qDebug()<<returnVec.x;
            //v
            p = p3 - point;
            q = p1 - point;
            Kryss = glm::cross(glm::vec3{q.x, 0, q.y},glm::vec3{p.x, 0, p.y});
            returnVec.y = Kryss.y / areal_123;
    //qDebug()<<returnVec.y;
            //w
            p = p1 - point;
            q = p2 - point;
            Kryss = glm::cross(glm::vec3{q.x,0,q.y}, glm::vec3{p.x, 0, p.y});
            returnVec.z = Kryss.y / areal_123;
    //qDebug()<<returnVec.z;
    return returnVec;
    }

};

#endif // WORLDCOORDINATOR_H
