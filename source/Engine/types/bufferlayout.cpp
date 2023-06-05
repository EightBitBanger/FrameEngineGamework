#include "bufferlayout.h"

Vertex::Vertex() : 
    x(0), y(0), z(0),
    r(0), g(0), b(0),
    nx(0), ny(0), nz(0),
    u(0), v(0)
{
}

Vertex::Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float nxx, float nyy, float nzz, float uu, float vv) : 
    x(xx), y(yy), z(zz),
    r(rr), g(gg), b(bb),
    nx(nxx), ny(nyy), nz(nzz),
    u(uu), v(vv)
{
}

void Vertex::operator= (const Vertex& vertex) {
    x = vertex.x;    // Position
    y = vertex.y;
    z = vertex.z;
    r = vertex.r;    // Color
    g = vertex.g;
    b = vertex.b;
    nx = vertex.nx;  // Normals
    ny = vertex.ny;
    nz = vertex.nz;
    u = vertex.u;    // Texture UV
    v = vertex.v;
    return;
}

Index::Index(unsigned int value) : 
    
    index(value)
{
}
