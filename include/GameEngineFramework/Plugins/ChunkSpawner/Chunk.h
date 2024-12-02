#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>


class ENGINE_API Chunk {
    
public:
    
    Chunk();
    
    int32_t x;
    int32_t y;
    
    GameObject* gameObject;
    GameObject* staticObject;
    GameObject* waterObject;
    
    rp3d::RigidBody* rigidBody;
    
    rp3d::Collider* bodyCollider;
    
    MeshCollider* meshCollider;
    
};


#endif
