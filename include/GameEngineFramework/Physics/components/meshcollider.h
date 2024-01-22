#ifndef _MESH_COLLIDER_
#define _MESH_COLLIDER_

#include <GameEngineFramework/configuration.h>

#include <ReactPhysics3d/ReactPhysics3d.h>
#include <vector>


class ENGINE_API MeshCollider {
    
public:
    
    rp3d::HeightFieldShape* heightFieldShape;
    
    float* heightMapBuffer;
    
    MeshCollider();
    ~MeshCollider();
    
};

#endif
