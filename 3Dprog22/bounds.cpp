#include "bounds.h"
#include "glm/gtx/norm.hpp"

CollisionPolygon::CollisionPolygon(CollisionType type)
    : type(type)
{

}

CollisionPolygon::CollisionPolygon(glm::vec3 center, float radius)
    :type(CollisionType::SPHERE), center(center), radius(radius)
{

}

CollisionPolygon::CollisionPolygon(glm::vec3 min, glm::vec3 max)
    :type(CollisionType::AABB), min(min), max(max)
{

}


glm::vec3 CollisionPolygon::calculateCenter() const
{
    if(type == CollisionType::AABB)
    {
        return (min + max) / 2.f;
    }
    else if(type == CollisionType::SPHERE)
        return center;

    return glm::vec3(-10000.f);
}

glm::vec3 CollisionPolygon::calculateDimensions() const
{
    if(type == CollisionType::AABB)
        return max - min;
    else
        return glm::vec3(2.f*radius);
}

bool CollisionPolygon::containsPoint(const glm::vec3& point) const
{
    if(type == CollisionType::AABB)
    {
        return (point.x >= min.x) && ( point.x <max.x) &&
               (point.y >= min.y) && ( point.y <max.y) &&
                (point.z >= min.z) && ( point.z <max.z);
    }
    else
    {
        //Hvis distansen fra midten til punktet er mindre enn radius, da er punktet inni regionen
        return glm::distance2(point, center) <= (radius * radius);
    }
}

// Sjekker om "this" omringer "other". Altså at hele "other" er inne i "this"
bool CollisionPolygon::containsRegion(const CollisionPolygon &other) const
{
    if(other.type == CollisionType::AABB)
    {
        // Hvis vi sjekker mot en boks så trenger vi at både min og max må være inni vår region
        // Vi trenger heller ikke sjekke typen til "this".
        // Dette er jo fordi i containsPoint så sjekker vi "this->type".
        return containsPoint(other.min) && containsPoint(other.max);
    }
    else if(type == CollisionType::SPHERE && other.type == CollisionType::SPHERE)
    {
        return glm::length(center - other.center) + other.radius < radius;
    }
    else
    {
        //Dette er tilfellet hvor "other" er en SPHERE og "this" er en boks
        // Hvis ikke midten av "other" er inne i "this", så vet vi at resten av kulen/ballen er utenfor
        //så da kan vi bare terminere, og returnere false.
        if(!containsPoint(other.center))
            return false;

        for(int i = 0; i < 3; i++)
        {
            //Denne if-sjekken ser hver akse.
            //Den sjekker hvis lengdenen fra max til center er mindre en radius.
            //Hvis det stemmer, så vil det si at det er en bit av sirkelen som ikke er inne i kuben,
            //  og vi kan derfor returnere false.
            if(abs(max[i] - other.center[i]) < other.radius ||
               abs(other.center[i] - min[i]) < other.radius)
                    return false;
        }
    }
    //Hvis den siste else biten ikke returnerete false, så returnerer vi true.
    //   Da er "other" inne i "this"
    return true;
}

// Sjekke om to regioner overlapper.
bool CollisionPolygon::intersectCheck(CollisionPolygon &other)
{

    //Her må vi sjekk alle aksene for å se om de overlapper. x,y og z.
    if(type == CollisionType::AABB && other.type == CollisionType::AABB)
    {
        glm::vec3 this_rad = calculateDimensions() / 2.f; // radiusen til boksen? O.o
        glm::vec3 other_rad = other.calculateDimensions() / 2.f;

        //Denne er ikke nødvendig siden jeg setter midten til å være posisjonen til objektet ;)
//        center = calculateCenter();
//        other.center = other.calculateCenter();

        glm::vec3 dist = abs(center - other.center);

        //Hvis en av aksene ikke overlapper, da hopper vi ut.
        for(int i = 0; i < 3; i++)
        {

            if(dist[i] > this_rad[i] + other_rad[i])
            {
                //Her er det ikke en overlapp, vi kan hoppe ut!
                return false;
            }
        }

        // Viser seg at det er overlap! ;)
        return true;
    }
    else if(type == CollisionType::SPHERE && other.type == CollisionType::SPHERE)
    {
        return glm::length(center - other.center) > radius + other.radius;
    }
    else if(type == CollisionType::SPHERE)
    {
        //Vi må finne ut hvor "this" er i forhold til "other"
        // Sjekke over, under osv.
        float distanseKvadrat = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            // Finne ut om vi er over, under, foran, bak, høyre eller venstre for boksen.
            float closestPt = std::max(other.min[i], std::min(center[i], other.max[i]));

            //legge sammen distansen
            distanseKvadrat += (closestPt - center[i]) * (closestPt - center[i]);
        }
        return distanseKvadrat < (radius * radius);
    }

    else
    {
        //vet vi at "this" er boks, og "other" er sphere. Da kan vi kalle intersectCheck på "other"
        // siden allerede har en if-statement som tester SPHERE mot AABB "(type == RegionType::SPHERE)"
        // Da slipper vi å lage en ekstra else if.

        return other.intersectCheck(*this);
    }
}
