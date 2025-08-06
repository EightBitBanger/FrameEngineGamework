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
    
    /// Is this chunk finished generating
    bool isGenerated;
    
    /// Is the chunk fully generated
    bool isComplete;
    
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
    
    /// List of the location of any structures on this chunk.
    std::vector<glm::vec3> structureLocations;
    
    /// Biome region tracking map.
    std::vector<int> biomeMap;
    
    /// Biome blend masks
    std::vector<std::vector<float>> biomeWeights;
    std::vector<float> totalWeights;
    
    /// Generation fields
    float* heightField;
    glm::vec3* colorField;
    
};


#endif
