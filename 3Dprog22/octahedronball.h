#pragma once
#include "visualobject.h"

class OctahedronBall : public VisualObject {
private:
   int m_iterations;
   int m_indeks;               // brukes i rekursjon, til å bygge m_vertices
   void lagTriangel(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
   void subDivide(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int n);
   void oktaederUnitBall();
public:
   OctahedronBall();
   OctahedronBall(int n=3, const std::string& shader = "plainshader");
   ~OctahedronBall();
   void init(GLint matrixUniform) override;
   void draw(const glm::mat4& pMat) override ;
   void tick(const float& dt) override;
   void move(float dx, float dy, float dz) override {mPosition = {dx,dy,dz};};

   glm::vec3 velocity;
};
