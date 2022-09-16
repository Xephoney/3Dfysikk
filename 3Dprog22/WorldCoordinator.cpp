#include "WorldCoordinator.h"
#include "trianglesurface.h"

float World::distanceToTriangle(glm::vec3 pos, int triangleIndex)
{
    if(triangleIndex == -1)
        return 10000000.f;

    std::vector<glm::vec3> vec;
    surface->trianglePositions(triangleIndex, vec);
    glm::vec3 norm = getNormal(triangleIndex);
    glm::vec3 vector_avgpos_pos = vec[0] - pos;

    float dist = abs(glm::dot(vector_avgpos_pos, norm));
    return dist;
}

glm::vec3 World::getNormal(int TriIndex)
{
    if(!surface)
        return glm::vec3(0.f);

    return surface->triangleNormal(TriIndex);
}

int World::getTriangleIndex(const glm::vec3 &position)
{
    if(!surface)
        return -1;

    bool escape = false;
    int counter = 0;
    while(!escape)
    {
        if(counter > surface->triangleCount - 1)
            return -1;

        std::vector<glm::vec3> triPos;
        surface->trianglePositions(counter,triPos);
        if(validateBarycentricCoordinates(barycentricCoordinates(triPos[0], triPos[1], triPos[2], position)))
        {
            return counter;
        }

        counter++;
    }
    return counter;
}

