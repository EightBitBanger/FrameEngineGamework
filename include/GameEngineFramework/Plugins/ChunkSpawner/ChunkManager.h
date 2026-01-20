#ifndef __CHUNK_MANAGER_
#define __CHUNK_MANAGER_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/WorldGeneration.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/PerlinDefinition.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Biome.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>

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
    
    Actor* SummonActor(glm::vec3 position);
    void KillActor(Actor* actor);
    
    Chunk* FindChunk(int x, int z);
    
    bool WorldDirectoryInitiate(void);
    
    // Save / load
    
    bool SaveChunk(Chunk* chunk, bool doClearActors);
    bool LoadChunk(Chunk* chunk);
    
    bool SaveWorld(void);
    bool LoadWorld(void);
    
    // Purge
    
    void ClearWorld(void);
    bool DestroyWorld(std::string worldname);
    
    // Chunks
    
    Chunk* CreateChunk(float x, float y);
    bool DestroyChunk(Chunk* chunk);
    
    void GenerateChunkBiomes(Chunk* chunk);
    void GenerateChunkBlendMasks(Chunk* chunk);
    
    // World rules
    
    void AddWorldRule(std::string key, std::string value);
    bool RemoveWorldRule(std::string key);
    std::string GetWorldRule(std::string key);
    bool SetWorldRule(std::string key, std::string value);
    bool ApplyWorldRule(std::string key, std::string value);
    
    // Biome generation
    void GenerateBiome(glm::vec3* colorField, float* heightField, Chunk* chunk, Biome* biome, float* weightMask, float* totalWeights);
    
    
    /// Set a layer of perlin noise into a color field.
    void AddColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, Color color, int offsetX, int offsetZ);
    
    // World generation
    
    /// Initiates a height field grid array of points and set them to zero.
    void SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value);
    
    /// Add a layer of perlin noise into a height field. The minimum 
    /// height value will be returned.
    float AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseMul, int offsetX, int offsetZ, float heightThreshold, int seed);
    
    // Color field
    
    /// Initiates a color field grid array of colors and set them to zero.
    void SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color, float noise);
    
    /// Generate a color field containing a color range from from low to high. The bias will determine the fade 
    /// from the low color to the high color based on the height field values.
    void GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color low, Color high, float bias, float beginHeight);
    
    /// Smooth the terrain height starting at a given height level and moving downward.
    void GenerateWaterTableFromHeightField(float* heightField, unsigned int width, unsigned int height, float tableHeight);
    
    /// Generate a snow cap effect of a given color capColor and starting at the height beginHeight.
    /// The bias will determine how much snow will be added.
    void AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color capColor, float beginHeight, float bias);
    
    /// Generate a water level effect.
    void AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color waterColor, float beginHeight, float bias, float waterTableHeight);
    
    /// Translate a color by name to a color value.
    Color GetColorByName(const std::string& name);
    
    // Mapping to a mesh
    
    /// Apply the height field values to a mesh.
    void AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int subTessX=1.0f, unsigned int subTessZ=1.0f);
    
    /// Apply a reduced quality version of the height field values to a mesh. This function will
    /// reduce the mesh by one half of the original size.
    void AddHeightFieldToMeshSimplified(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int simplifyFactor);
    
    void AddHeightFieldToMeshLOD(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int lodFactor);
    
    /// Apply the height field values to the mesh using a quality resolution value.
    void AddHeightFieldToMeshReduced(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int resolution);
    
    /// Generate a height field mesh from perlin noise.
    Mesh* CreateMeshFromHeightField(float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ);
    
    /// Apply a height stepping effect to the mesh.
    void AddHeightStepToMesh(float* heightField, unsigned int width, unsigned int height);
    
    void Initiate(void);
    
    void Update(void);
    
    // Decoration functions
    void AddDecor(Chunk* chunk, const std::string& mesh, const std::string& type, const glm::vec3& position, const glm::vec3& rotation);
    
    DecorationHitInfo QueryDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold);
    bool PlaceDecor(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance, float threshold);
    bool RemoveDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold);
    
    
    //void DecodeGenome(std::string name, Actor* actorPtr);
    
    void Decorate(Chunk* chunk);
    
    // Build functions
    class ENGINE_API BuildFunctions {
    public:
        
        void StackAtAngle(Structure& structure, glm::vec3 position, glm::vec3 scale, glm::vec3 angle, float stepHeight, int length, Color color);
        
    } build;
    
    bool BuildDecorStructure(Chunk* chunk, glm::vec3 position, const std::string& pattern, const std::string& name, const std::string& mesh);
    
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
    
    // Chunk generation thread
    std::thread* generationThread;
    
    // World generation functions
    
    void GenerateChunks(const glm::vec3 &playerPosition);
    void DestroyChunks(const glm::vec3 &playerPosition);
    bool IsChunkFound(const glm::vec2 &chunkPosition);
    
    void HandleActorLifeCycle(GameObject* actorObject);
    
    void UpdateFogSettings(const glm::vec3 &playerPosition);
    
    void InitializePlayerHeight(glm::vec3 &playerPosition);
    
    // List of world rules
    
    std::vector<std::pair<std::string, std::string>> mWorldRules;
    
    // Update index counters
    
    unsigned int mActorIndex;
    unsigned int mChunkIndex;
    
    int mChunkCounterX;
    int mChunkCounterZ;
    
    // Cool down counters
    
    unsigned int mBreedingCoolDown;
    unsigned int mDeathCoolDown;
    
    // Mesh cache
public:
    
    std::unordered_map<std::string, unsigned int> mStaticMeshToIndex;
    std::unordered_map<unsigned int, std::string> mStaticIndexToMesh;
    
    std::unordered_map<std::string, SubMesh> mStaticMeshes;
    
private:
    
    Mesh* waterMesh;
    
    // World fogs
    
    Fog* fogWater;
    
};

#endif
