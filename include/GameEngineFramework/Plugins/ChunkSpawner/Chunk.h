#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>

class ENGINE_API StaticObject {
    
public:
    
    float x;
    float y;
    float z;
    
    float r;
    float g;
    float b;
    
    uint8_t type;
};


class ENGINE_API Chunk {
    
public:
    
    Chunk();
    
    /// Is the chunk active in the scene
    bool isActive;
    
    float x;
    float y;
    
    GameObject* gameObject;
    GameObject* staticObject;
    GameObject* waterObject;
    
    rp3d::RigidBody* rigidBody;
    
    rp3d::Collider* bodyCollider;
    
    MeshCollider* meshCollider;
    
    std::vector<StaticObject> statics;
    
};


#endif
