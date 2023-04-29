#ifndef _VERTEXBUFFER_TYPE__
#define _VERTEXBUFFER_TYPE__

class Vertex {
    
public:
    
    float x, y, z;  // Position
    float r, g, b;  // Color
    float u, v;     // Texture coordinate
    
    Vertex();
    
    Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float uu, float vv);
    
    void operator= (const Vertex& vertex);
    
};


class Index  {
    
public:
    
    unsigned int index;
    
    Index(unsigned int value);
    
};


#endif
