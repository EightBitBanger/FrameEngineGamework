#ifndef _CHUNK__
#define _CHUNK__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>
#include <GameEngineFramework/Plugins/ParticleSystem/Emitter.h>

#include <deque>

#define  CHUNK_IS_ACTIVE      0x01
#define  CHUNK_IS_GENERATED   0x02
#define  CHUNK_IS_COMPLETE    0x04
#define  CHUNK_DO_UPDATE      0x08

class ENGINE_API Chunk {
public:
    
    Chunk();
    
    uint8_t flags;
    
    //bool isActive;
    bool isGenerated;
    bool isComplete;
    bool doUpdate;
    
    /// Current seed used to generate this chunk
    int seed;
    
    /// Chunk world position
    float x;
    float y;
    
    /// Fade in effect counter
    float fadeIn;
    
    GameObject* gameObject;
    GameObject* staticObject;
    GameObject* pickupObject;
    
    rp3d::RigidBody* rigidBody;
    
    rp3d::Collider* bodyCollider;
    
    MeshCollider* meshCollider;
    
    /// Base list of all static geometry in this chunk.
    std::vector<StaticObject> statics;
    
    /// Quick lookup list for animated static objects.
    std::vector<StaticAnimation> animatedStatics;
    
    /// Active particle emitters in this chunk.
    std::vector<Emitter*> emitters;
    
    /// List of item pickups in this chunk.
    std::vector<StaticPickup> pickups;
    
    /// Biome region tracking map.
    std::vector<int> biomeMap;
    
    /// Biome blend masks
    std::vector<std::vector<float>> biomeWeights;
    std::vector<float> totalWeights;
    
    /// Generation fields
    float* heightField;
    glm::vec3* colorField;
    glm::vec3* colorAdditive;
    
};


#endif
