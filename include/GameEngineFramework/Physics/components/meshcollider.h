#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <GameEngineFramework/configuration.h>

#include <ReactPhysics3d/ReactPhysics3d.h>
#include <vector>


class ENGINE_API MeshCollider {
    
public:
    
    MeshCollider();
    ~MeshCollider();
    
    // Height field collider
    rp3d::HeightFieldShape* heightFieldShape;
    
    float* heightMapBuffer;
    
    
    // Mesh collider
    rp3d::ConcaveMeshShape*    concaveMeshShape;
    rp3d::TriangleVertexArray* triangleArray;
    rp3d::TriangleMesh*        triangleMesh;
    
    std::vector<float> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    
};

#endif
