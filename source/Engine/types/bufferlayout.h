#ifndef _VERTEXBUFFER_TYPE__
#define _VERTEXBUFFER_TYPE__

struct Vertex {
    
    /// Position
    float x, y, z;
    
    /// Color
    float r, g, b;
    
    /// Normal
    float nx, ny, nz;
    
    /// Texture coordinates
    float u, v;
    
    Vertex();
    
    Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float nxx, float nyy, float nzz, float uu, float vv);
    
    void operator= (const Vertex& vertex);
    
};


struct Index  {
    
    unsigned int index;
    
    Index(unsigned int value);
    
};


#endif
