#pragma once
#ifndef BOUNDS_H
#define BOUNDS_H

// Denne klassen er lagd i følge med en video fra Michael Grieco
// link : https://www.youtube.com/watch?v=enB97Utgpwg&list=PLysLvOneEETPlOI_PI4mJnocqIpr2cSHS&index=25
// Tanken er å bruke det som start-punkt, og deretter bygge på med SAT kollisjon algoritmer senere.

// Hele denne klassen er til alle oppgavene med kollisjon
// Oppgave 7, 8 og 9

#include <glm/glm.hpp>

enum class CollisionType
{
    NONE = 0,
    AABB = 1,
    SPHERE = 2
};

enum class CollisionResponse
{
    GHOST = 0,
    STATIC = 1,
    DYNAMIC = 2
};


class CollisionPolygon
{
public:
    CollisionPolygon() = delete;
    CollisionType type;

    glm::vec3 center;
    float radius;

    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 extent;


    // Basert på hva som blir gitt
    CollisionPolygon(CollisionType type);

    // Sirkel/sphere
    CollisionPolygon(glm::vec3 center, float radius);

    // Boks :)
    CollisionPolygon(glm::vec3 min, glm::vec3 max);

    glm::vec3 calculateCenter() const;

    glm::vec3 calculateDimensions() const;

    // Algoritme funksjoner

    bool containsPoint(const glm::vec3& point) const;

    bool containsRegion(const CollisionPolygon& other) const;

    bool intersectCheck(CollisionPolygon& other);
};

#endif // BOUNDS_H
