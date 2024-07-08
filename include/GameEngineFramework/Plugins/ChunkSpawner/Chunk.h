#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>


class ENGINE_API Chunk {
    
public:
    
    /// Position of the chunk in the world.
    glm::vec2 position;
    
    /// Associated game object.
    GameObject* gameObject;
    
    /// Levels of detail
    Mesh* lodHigh;
    Mesh* lodLow;
    
    /// Associated rigid body.
    rp3d::RigidBody* rigidBody;
    
    /// Associated collider.
    rp3d::Collider* bodyCollider;
    
    /// Height field collision buffer.
    MeshCollider* collider;
    
    /// List of actor objects in this chunk.
    std::vector<GameObject*> actorList;
    
    /// Batched static object container
    GameObject* staticObjects;
    
};


#endif
