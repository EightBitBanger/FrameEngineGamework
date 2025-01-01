#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <GameEngineFramework/configuration.h>

#include <ReactPhysics3d/ReactPhysics3d.h>
#include <vector>


class ENGINE_API MeshCollider {
    
public:
    
    /// Height field collider.
    rp3d::HeightFieldShape* heightFieldShape;
    float* heightMapBuffer;
    
    /// Triangle collider.
    rp3d::TriangleMesh* triangleMesh;
    float* vertexBuffer;
    unsigned int* indexBuffer;
    
    MeshCollider();
    ~MeshCollider();
    
};

#endif
