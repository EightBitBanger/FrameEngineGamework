#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <ReactPhysics3d/ReactPhysics3d.h>


class MeshCollider {
    
public:
    
    MeshCollider();
    
    rp3d::ConcaveMeshShape*    concaveMeshShape;
    rp3d::TriangleVertexArray* triangleArray;
    rp3d::TriangleMesh*        triangleMesh;
    
    std::vector<float> vertexBuffer;
    std::vector<unsigned int> indexBuffer;
    
};

#endif
