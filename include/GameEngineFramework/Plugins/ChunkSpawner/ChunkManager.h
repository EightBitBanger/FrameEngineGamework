#ifndef __CHUNK_MANAGER_
#define __CHUNK_MANAGER_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/WorldGeneration.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/PerlinDefinition.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Biome.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/HeightMapping.h>

#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>

#include <utility>

extern ParticleSystem Particle;

ENGINE_API glm::vec3 SnapAxes(glm::vec3 p, glm::bvec3 axes, float grid, glm::vec3 origin = glm::vec3(0.0f));

struct DecorationHitInfo {
    bool didHit = false;
    
    std::string type;
    std::string mesh;
    
    glm::vec3 worldPosition;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct NearbyStaticInfo {
    std::string type;
    std::string mesh;
    glm::vec3 worldPosition;
    glm::vec3 rotation;
    glm::vec3 scale;
    float distance;
};

struct NearbyPickupInfo {
    std::string classification;
    glm::vec3 worldPosition;
    float distance;
};

struct PendingRequest {
    std::string classification;
    glm::vec3 position;
    glm::vec3 rotation;
};

class ENGINE_API ChunkManager {
public:
    
    WorldGeneration world;
    
    std::string version;
    
    bool isInitiated;
    bool isChunkGenerationActive;
    
    float renderDistance;
    float staticDistance;
    float actorDistance;
    
    int chunkSize;
    
    int worldSeed;
    
    ChunkManager();
    
    // World rules
    
    void AddWorldRule(std::string key, std::string value);
    bool RemoveWorldRule(std::string key);
    std::string GetWorldRule(std::string key);
    bool SetWorldRule(std::string key, std::string value);
    bool ApplyWorldRule(std::string key, std::string value);
    
    // Raycast / query
    
    /// Queries all static objects within a given radius around a world position.
    std::vector<NearbyStaticInfo> QueryRadius(const glm::vec3& centerPosition, float range);
    
    /// General query over all types of world objects.
    std::string QueryWorld(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold);
    
    // Decorations
    
    bool PlaceDecor(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance, float threshold);
    bool PlaceDecorAt(const std::string& type, const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f));
    
    bool PlaceStructure(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance = 100.0f, float threshold = 0.5f);
    bool PlaceStructureAt(const std::string& name, const glm::vec3& worldPosition);
    
    bool RemoveDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold);
    bool RemoveDecorAt(const glm::vec3& position, float tolerance = 0.1f);
    bool RemoveDecorByIndex(Chunk* chunk, size_t index, bool rebuildMesh = true);
    
    /// Query static decor via raycast.
    DecorationHitInfo QueryDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold);
    
    /// Query static objects in range and return a list of (name, position) pairs.
    std::vector<std::pair<std::string, glm::vec3>> QueryDecorNames(const glm::vec3& centerPosition, float range);
    
    /// Thread-safe method to queue static object placement from background threads
    void QueueDecorAt(const std::string& type, const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f));
    
    // Item pickups
    
    bool PlacePickup(glm::vec3 position, glm::vec3 direction, float maxDistance, const std::string& itemClassification);
    bool PlacePickupAt(const std::string& itemClassification, const glm::vec3& position, const glm::vec3& rotation);
    bool RemovePickup(glm::vec3 position, glm::vec3 direction, float maxDistance, std::string& collectedItem);
    bool RemovePickupAt(const glm::vec3& position, float tolerance = 0.1f, std::string* collectedItem = nullptr);
    bool QueryPickup(glm::vec3 position, glm::vec3 direction, float maxDistance, std::string& queriedItem);
    
    /// Query pickups via raycast.
    std::vector<NearbyPickupInfo> QueryPickupNearest(glm::vec3 position, float maxDistance, size_t count = 1);
    
    /// Query pickups in range and return a list of (name, position) pairs.
    std::vector<std::pair<std::string, glm::vec3>> QueryPickupNames(const glm::vec3& position, float range);
    
    /// Thread-safe method to queue item drops from background threads
    void QueuePickupAt(const std::string& itemClassification, const glm::vec3& position, const glm::vec3& rotation);
    
    // Chunk color manipulation
    
    /// Sets an additive color value at a specific world position.
    bool SetWorldColorAdditive(const glm::vec3& worldPosition, const Color& color);
    
    /// Applies an additive color brush in a radius around a world position (supports snow, tilling, scorch marks).
    bool AddWorldColorAdditiveRadius(const glm::vec3& worldPosition, float radius, const glm::vec3& color, float intensity = 1.0f, bool additive = true);
    
    /// Fades all additive effects within a chunk over time (for melting snow, drying soil).
    void DecayChunkColorAdditive(Chunk* chunk, float decayRate);
    
    /// Refreshes the GPU mesh vertex colors for a chunk without regenerating geometry.
    void RebuildChunkMeshColors(Chunk* chunk);
    
    // Actors
    
    Actor* SummonActor(glm::vec3 position);
    void KillActor(Actor* actor);
    
    // Save / load
    
    unsigned int GetWorldVersion(void);
    
    bool SaveChunk(Chunk* chunk, bool doClearActors);
    bool LoadChunk(Chunk* chunk);
    
    bool SaveWorld(void);
    bool LoadWorld(void);
    
    // Purge
    
    void ClearWorld(void);
    bool DestroyWorld(std::string worldname);
    
    // Chunks
    
    Chunk* FindChunk(int x, int z);
    Chunk* CreateChunk(float x, float y);
    bool DestroyChunk(Chunk* chunk);
    
    void GenerateChunkBiomes(Chunk* chunk);
    void GenerateChunkBlendMasks(Chunk* chunk);
    
    // Internal chunk decoration
    void Decorate(Chunk* chunk);
    bool BuildDecorStructure(Chunk* chunk, glm::vec3 position, const std::string& pattern, const std::string& name, const std::string& mesh);

    void BuildItemMesh(Mesh* targetMesh, const std::string& itemClassification, const glm::vec3& offset = glm::vec3(0.0f));
    unsigned int AddPickupToMesh(Mesh* targetMesh, const std::string& itemClassification, const glm::vec3& position, const glm::vec3& rotation);
    void RebuildPickupMesh(Chunk* chunk);
    
    class ENGINE_API BuildFunctions {
    public:
        
        void StackAtAngle(Structure& structure, glm::vec3 position, glm::vec3 scale, glm::vec3 angle, float stepHeight, int length, Color color);
        
    } build;
    
    void Initiate(void);
    bool InitiateWorldDirectory(void);
    
    void Update(float deltaTime);
    void UpdateStaticObjects(float deltaTime);
    void UpdatePickupObjects(float deltaTime);
    void ProcessPendingRequests();
    
    // World materials
    
    Material* waterMaterial;
    Material* worldMaterial;
    Material* staticMaterial;
    
    // List of chunks to process for generation
    std::vector<Chunk*> generating;
    std::mutex mux;
    Timer threadTimer;
    
    PoolAllocator<Chunk> chunks;
    
private:
    
    // Internal decoration mesh combiner function
    void AddDecor(Chunk* chunk, const std::string& mesh, const std::string& type, const glm::vec3& position, const glm::vec3& rotation, glm::vec3 scale = glm::vec3(0.0f), glm::vec3 color = glm::vec3(-1.0f), int function = -1);
    
    // Biome generation
    void GenerateBiome(glm::vec3* colorField, float* heightField, Chunk* chunk, Biome* biome, float* weightMask, float* totalWeights);
    
    // Translate a color by name to a color value.
    Color GetColorByName(const std::string& name);
    
    HeightMapping generation;
    
    // Chunk generation thread
    std::thread* generationThread;
    
    // World generation functions
    
    void GenerateChunks(const glm::vec3 &playerPosition);
    void DestroyChunks(const glm::vec3 &playerPosition);
    bool IsChunkFound(const glm::vec2 &chunkPosition);
    
    void HandleActorLifeCycle(GameObject* actorObject);
    
    void UpdateFogSettings(const glm::vec3 &playerPosition);
    
    void InitializePlayerHeight(glm::vec3 &playerPosition);
    void KeepPlayerAboveGround(glm::vec3 &playerPosition, float playerHeightOffset);
    
    // List of world rules
    
    std::vector<std::pair<std::string, std::string>> mWorldRules;
    
    // Update index counters
    
    unsigned int mActorIndex;
    unsigned int mChunkIndex;
    
    int mChunkCounterX;
    int mChunkCounterZ;
    
    std::unordered_map<std::string, unsigned int> mStaticMeshToIndex;
    std::unordered_map<unsigned int, std::string> mStaticIndexToMesh;
    
    std::unordered_map<std::string, SubMesh> mStaticMeshes;
    
    Mesh* waterMesh;
    
    // World fog layers
    
    Fog* fogWater;
    
    // Item request queue
    
    std::vector<PendingRequest> mPendingRequests;
    std::mutex mPendingRequestsMutex;
    
    std::vector<PendingRequest> mPendingStaticRequests;
    std::mutex mPendingStaticRequestsMutex;
};

#endif
