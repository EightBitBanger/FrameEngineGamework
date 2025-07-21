#ifndef __CHUNK_MANAGER_
#define __CHUNK_MANAGER_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Biome.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>


class ENGINE_API WorldGeneration {
    
public:
    
    std::string name;
    
    bool doGenerateChunks;
    
    bool doAutoBreeding;
    
    float snowCapHeight;
    float snowCapBias;
    Color snowCapColor;
    
    Color waterColorLow;
    Color waterColorHigh;
    
    float waterLevel;
    
    float ambientLight;
    
    Color chunkColorLow;
    Color chunkColorHigh;
    float chunkColorBias;
    
    Color staticColorLow;
    Color staticColorHigh;
    
    Color actorColorLow;
    Color actorColorHigh;
    
    std::vector<DecorationSpecifier> mDecorations;
    
    std::vector<Perlin> mPerlin;
    std::vector<Biome> mBiomes;
    
    std::vector<Structure> mStructures;
    
    // Static plant generation
    
    unsigned int staticDensity;
    float staticHeightCutoff;
    
    // Tree generation
    
    unsigned int treeDensity;
    float treeHeightCutoff;
    float treeHeightLow;
    float treeHeightHigh;
    
    unsigned int numberOfLeaves;
    float leafSpreadArea;
    float leafSpreadHeight;
    float leafHeightOffset;
    
    // Actors
    unsigned int actorDensity;
    
    
    WorldGeneration() :
        name("default"),
        
        doGenerateChunks(false),
        doAutoBreeding(true),
        
        snowCapHeight(60.0f),
        snowCapBias(2.0f),
        snowCapColor(0.7f, 0.85f, 1.1f, 1.0f),
        
        waterColorLow(Colors.black),
        waterColorHigh(Colors.blue),
        
        waterLevel(-21.0f),
        
        ambientLight(1.0f),
        
        chunkColorLow(Colors.black),
        chunkColorHigh(Colors.white),
        
        staticColorLow(Colors.black),
        staticColorHigh(Colors.white),
        
        actorColorLow(Colors.black),
        actorColorHigh(Colors.white),
        
        staticDensity(200),
        staticHeightCutoff(50),
        
        treeDensity(40),
        treeHeightCutoff(50),
        treeHeightLow(5),
        treeHeightHigh(8),
        
        numberOfLeaves(15),
        leafSpreadArea(3.0f),
        leafSpreadHeight(1.4f),
        leafHeightOffset(-0.4f),
        
        actorDensity(10)
    {
    }
    
};



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
    
    bool SaveChunk(Chunk& chunk, bool doClearActors);
    bool LoadChunk(Chunk& chunk);
    
    bool SaveWorld(void);
    bool LoadWorld(void);
    
    // Purge
    
    void ClearWorld(void);
    bool DestroyWorld(std::string worldname);
    
    // Chunks
    
    Chunk CreateChunk(float x, float y);
    bool DestroyChunk(Chunk& chunk);
    
    // World rules
    
    void AddWorldRule(std::string key, std::string value);
    bool RemoveWorldRule(std::string key);
    std::string GetWorldRule(std::string key);
    bool SetWorldRule(std::string key, std::string value);
    bool ApplyWorldRule(std::string key, std::string value);
    
    // World generation functions
    
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
    void GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color low, Color high, float bias);
    
    /// Smooth the terrain height starting at a given height level and moving downward.
    void GenerateWaterTableFromHeightField(float* heightField, unsigned int width, unsigned int height, float tableHeight);
    
    /// Set a layer of perlin noise into a color field. The perlin noise is used to fade from the first color to the second.
    void SetColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseThreshold, Color first, Color second, int offsetX, int offsetZ);
    
    /// Generate a snow cap effect of a given color capColor and starting at the height beginHeight.
    /// The bias will determine how much snow will be added.
    void AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color capColor, float beginHeight, float bias);
    
    /// Generate a water level effect.
    void AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color waterColor, float beginHeight, float bias, float waterTableHeight);
    
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
    
    void AddDecor(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz, float yaw, float pitch, float roll);
    void AddDecorGrass(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorGrassThin(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorGrassThick(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    
    void AddDecorTreeLogs(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorTreeLeaves(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorTree(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz, Decoration treeType);
    
    void Decorate(Chunk& chunk);
    
    std::vector<Chunk> chunks;
    
    // World material batches
    
    Material* waterMaterial;
    Material* worldMaterial;
    Material* staticMaterial;
    
private:
    
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


struct StaticElement {
    
    glm::vec3 position;
    glm::vec2 rotation;
    
    glm::vec3 color;
    
    uint8_t type;
    
};


#endif
