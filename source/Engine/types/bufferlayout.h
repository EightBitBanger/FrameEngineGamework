#ifndef _VERTEXBUFFER_TYPE__
#define _VERTEXBUFFER_TYPE__

struct Vertex {
    
    float x, y, z;     // Position
    float r, g, b;     // Color
    float nx, ny, nz;  // Normals
    float u, v;        // Texture coordinate
    
    Vertex();
    
    Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float nxx, float nyy, float nzz, float uu, float vv);
    
    void operator= (const Vertex& vertex);
    
};


struct Index  {
    
    unsigned int index;
    
    Index(unsigned int value);
    
};


#endif
