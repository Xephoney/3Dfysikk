#pragma once
#include "visualobject.h"

class TriangleSurface : public VisualObject
{
public:
   TriangleSurface();
   TriangleSurface(std::string filnavn);
   ~TriangleSurface() override;
   void writeToFile(std::string filNavn);
   void readFile(std::string filnavn);
   virtual void init(GLint matrixUniform) override;
   virtual void draw(const glm::mat4& pMat) override;
   void move(float x, float y, float z) override { mPosition.x = x; mPosition.y = y; mPosition.z = z;};
   void makeQuad(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c, const glm::vec3& d);
   void makeQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3& farge);
   void makeTriangle(const int &indx0, const int &indx1, const int &indx2);
protected:
   void makeTriangle(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c);
   void makeTriangle(const glm::vec3& a,const glm::vec3& b,const glm::vec3& c, const glm::vec3& farge);
private:
   void construct();
   void constructIndices();
   float func(const float x, const float y);
};
