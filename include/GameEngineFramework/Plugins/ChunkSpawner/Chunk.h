#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>

class ENGINE_API StaticObject {
    
public:
    
    /// Position
    float x;
    float y;
    float z;
    
    /// Color
    float r;
    float g;
    float b;
    
    /// Type of decoration
    uint8_t type;
    
    /// Collision body representing the object in the world
    rp3d::CollisionBody* collisionBody;
    
    StaticObject();
    
};


class ENGINE_API Chunk {
    
public:
    
    Chunk();
    
    /// Is the chunk active in the scene
    bool isActive;
    
    /// Current seed used to generate this chunk
    int seed;
    
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
