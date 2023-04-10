#include "bufferlayout.h"

Vertex::Vertex() {
    
    x=0; y=0; z=0; r=0; g=0; b=0; u=0; v=0;
    
}

Vertex::Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float uu, float vv) {
    x=xx; y=yy; z=zz; r=rr; g=gg; b=bb; u=uu; v=vv;
}

void Vertex::operator= (const Vertex& vertex) {
    x = vertex.x;
    y = vertex.y;
    z = vertex.z;
    r = vertex.r;
    g = vertex.g;
    b = vertex.b;
    u = vertex.u;
    v = vertex.v;
    return;
}


Index::Index(unsigned int value) {
    
    index = value;
    
}

