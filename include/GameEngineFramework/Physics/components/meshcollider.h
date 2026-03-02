#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <GameEngineFramework/configuration.h>

#include <ReactPhysics3d/ReactPhysics3d.h>
#include <vector>


class ENGINE_API MeshCollider {
public:
    
    /// Height field collider
    rp3d::HeightFieldShape* heightFieldShape;
    float* heightMapBuffer;
    
    /// Concave mesh collider
    rp3d::TriangleMesh* triangleMesh;
    rp3d::TriangleVertexArray* triangleArray;
    
    rp3d::ConcaveMeshShape* concaveShape;
    float* vertexBuffer;
    unsigned int* indexBuffer;
    
    MeshCollider();
    ~MeshCollider();
};

#endif
