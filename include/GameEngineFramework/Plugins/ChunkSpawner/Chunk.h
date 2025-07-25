#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>

class ENGINE_API StaticObject {
    
public:
    
    glm::vec3 position;
    
    glm::vec3 rotation;
    
    glm::vec3 scale;
    
    glm::vec3 color;
    
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
    
    /// Chunk world position
    float x;
    float y;
    
    /// Fade in effect counter
    float fadeIn;
    
    GameObject* gameObject;
    GameObject* staticObject;
    
    rp3d::RigidBody* rigidBody;
    
    rp3d::Collider* bodyCollider;
    
    MeshCollider* meshCollider;
    
    std::vector<StaticObject> statics;
    
};


#endif
