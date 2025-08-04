#ifndef __CHUNK_MANAGER_
#define __CHUNK_MANAGER_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/WorldGeneration.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Biome.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Scenery.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/definitions/tree.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/definitions/grass.h>


class ENGINE_API ChunkManager {
    
public:
    
    WorldGeneration world;
    
    bool isInitiated;
    
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
    void SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color);
    
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
    Color GetColorByName(std::string name);
    
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
    
    // Base decoration function
    unsigned int AddDecor(Chunk* chunk, unsigned int index, Mesh* staticMesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color);
    
    void RemoveDecor(glm::vec3 position, glm::vec3 direction);
    
    // Decoration
    void AddDecorGrass(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name);
    void AddDecorTree(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name);
    void AddDecorStructure(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name);
    void AddDecoreActor(glm::vec3 position, std::string name);
    
    void DecodeGenome(std::string name, Actor* actorPtr);
    
    void Decorate(Chunk* chunk);
    
    PoolAllocator<Chunk> chunks;
    
    // World material batches
    
    Material* waterMaterial;
    Material* worldMaterial;
    Material* staticMaterial;
    
    // List of chunks to process for generation
    std::vector<Chunk*> generating;
    std::mutex mux;
    Timer threadTimer;
    
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
    
    // Mesh generation
    
    /// World generation meshes
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    
    SubMesh subMeshGrassHorz;
    SubMesh subMeshGrassVert;
    
    SubMesh subMeshStemHorz;
    SubMesh subMeshStemVert;
    
    SubMesh subMeshTree;
    
    Mesh* waterMesh;
    
    // World fogs
    
    Fog* fogWater;
    
};

#endif
