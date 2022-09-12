#include "Vertex.h"

std::ostream& operator<< (std::ostream& os, const Vertex& v) {
  os << std::fixed;
  os << "(" << v.m_xyz[0] << ", " << v.m_xyz[1] << ", " << v.m_xyz[2] << ") ";
  os << "(" << v.m_normal[0] << ", " << v.m_normal[1] << ", " << v.m_normal[2] << ") ";
  os << "(" << v.m_uv[0] << ", " << v.m_uv[1] << ") ";
  return os;
}
std::istream& operator>> (std::istream& is, Vertex& v) {
// Trenger fire temporære variabler som kun skal lese inn parenteser og komma
  char dum, dum2, dum3, dum4;
  is >> dum >> v.m_xyz[0] >> dum2 >> v.m_xyz[1] >> dum3 >> v.m_xyz[2] >> dum4;
  is >> dum >> v.m_normal[0] >> dum2 >> v.m_normal[1] >> dum3 >> v.m_normal[2] >> dum4;
  is >> dum >> v.m_uv[0] >> dum2 >> v.m_uv[1] >> dum3;
  return is;
}

Vertex::Vertex()
{

}

Vertex::Vertex(const float &x, const float &y, const float &z)
{
    m_xyz[0] = x;
    m_xyz[1] = y;
    m_xyz[2] = z;
}
Vertex::Vertex(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b, const float& u, const float& v)
{
    m_xyz[0] = x;
    m_xyz[1] = y;
    m_xyz[2] = z;
    m_normal[0] = r;
    m_normal[1] = g;
    m_normal[2] = b;
    m_uv[0] = u;
    m_uv[1] = v;
}
Vertex::Vertex(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b)
{
    m_xyz[0]=x;
    m_xyz[1]=y;
    m_xyz[2]=z;
    m_normal[0]=r;
    m_normal[1]=g;
    m_normal[2]=b;
    m_uv[0]=0.f;
    m_uv[1]=0.f;
}

Vertex::Vertex(const float &x, const float &y, const float &z, const float &r, const float &g, const float &b, const float &u, const float &v, const float &fr, const float &fg, const float &fb)
{
    m_xyz[0] = x;
    m_xyz[1] = y;
    m_xyz[2] = z;
    m_normal[0] = r;
    m_normal[1] = g;
    m_normal[2] = b;
    m_uv[0] = u;
    m_uv[1] = v;
    m_color[0] = fr;
    m_color[1] = fg;
    m_color[2] = fb;
}

void Vertex::setVertexColor(const float& a, const float& b, const float& c)
{
    m_color[0] = a;
    m_color[1] = b;
    m_color[2] = c;
}
