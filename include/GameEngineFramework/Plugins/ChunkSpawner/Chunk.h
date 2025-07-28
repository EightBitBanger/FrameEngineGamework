#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Scenery.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>

#include <deque>


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
    
    /// Base list of all static geometry in this chunk.
    std::vector<StaticObject> statics;
};


#endif
