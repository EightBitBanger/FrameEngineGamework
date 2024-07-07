#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <GameEngineFramework/configuration.h>

#include <ReactPhysics3d/ReactPhysics3d.h>
#include <vector>


class ENGINE_API MeshCollider {
    
public:
    
    /// Pointer to the physics height field map.
    rp3d::HeightFieldShape* heightFieldShape;
    
    /// Height field grid array.
    float* heightMapBuffer;
    
    MeshCollider();
    ~MeshCollider();
    
};

#endif
