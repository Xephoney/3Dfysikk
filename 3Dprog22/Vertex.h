#pragma once
#include <iostream>
#include "glm/glm.hpp"

class Vertex {


private:
    //! Overloaded ostream operator which writes all vertex data on an open textfile stream
    friend std::ostream& operator<< (std::ostream&, const Vertex&);

    //! Overloaded ostream operator which reads all vertex data from an open textfile stream
    friend std::istream& operator>> (std::istream&, Vertex&);
public:
    glm::vec3 m_xyz;
    glm::vec3 m_normal;
    glm::vec2 m_uv;
    glm::vec3 m_color;
    Vertex();
    Vertex(const float& x, const float& y, const float& z);
    Vertex(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b);
    Vertex(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b, const float& u, const float& v);
    Vertex(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b, const float& u, const float& v, const float& fr, const float& fg, const float& fb);
    void setVertexColor(const float& a, const float& b, const float& c);
};
