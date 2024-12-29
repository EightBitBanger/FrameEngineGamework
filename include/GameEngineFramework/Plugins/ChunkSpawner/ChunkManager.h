#ifndef __CHUNK_MANAGER_
#define __CHUNK_MANAGER_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>

class ENGINE_API WorldGeneration {
    
public:
    
    std::string name;
    
    bool doGenerateChunks;
    
    bool doAutoBreeding;
    
    float snowCapHeight;
    
    Color waterColorLow;
    Color waterColorHigh;
    
    float waterLevel;
    
    float ambientLight;
    
    Color chunkColorLow;
    Color staticColorLow;
    Color actorColorLow;
    
    Color chunkColorHigh;
    Color staticColorHigh;
    Color actorColorHigh;
    
    std::vector<DecorationSpecifier> mDecorations;
    
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
    float actorHeightCutoff;
    
    
    WorldGeneration() :
        name("default"),
        
        doGenerateChunks(false),
        doAutoBreeding(true),
        
        snowCapHeight(60.0f),
        
        waterColorLow(Colors.black),
        waterColorHigh(Colors.blue),
        
        waterLevel(-21.0f),
        
        ambientLight(1.0f),
        
        chunkColorLow(Colors.black),
        staticColorLow(Colors.black),
        actorColorLow(Colors.black),
        
        chunkColorHigh(Colors.white),
        staticColorHigh(Colors.white),
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
        
        actorDensity(10),
        
        actorHeightCutoff(20.0f)
        
    {
    }
    
};



class ChunkManager {
    
public:
    
    WorldGeneration world;
    
    bool isInitiated;
    
    float renderDistance;
    float staticDistance;
    
    int chunkSize;
    
    int worldSeed;
    
    ChunkManager();
    
    Chunk* FindChunk(int x, int z);
    
    void InitiateWorld(void);
    
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
    
    // Actors
    
    GameObject* SpawnActor(float x, float y, float z);
    
    bool KillActor(GameObject* actorObject);
    
    // World rules
    
    void AddWorldRule(std::string key, std::string value);
    
    bool RemoveWorldRule(std::string key);
    
    std::string GetWorldRule(std::string key);
    
    bool SetWorldRule(std::string key, std::string value);
    
    bool ApplyWorldRule(std::string key, std::string value);
    
    
    void Initiate(void);
    
    void Update(void);
    
    
    void AddDecorGrass(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorGrassThin(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorGrassThick(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    
    void AddDecorTreeLogs(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorTreeLeaves(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz);
    void AddDecorTree(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz, Decoration treeType);
    
    void Decorate(Chunk& chunk);
    
    
    unsigned int numberOfActiveActors;
    
    std::vector<Decoration> decoration;
    std::vector<Perlin> perlin;
    
    std::vector<Chunk> chunks;
    
    std::vector<GameObject*> actors;
    
    // World material batches
    
    Material* waterMaterial;
    Material* worldMaterial;
    Material* staticMaterial;
    
private:
    
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
    
};

#endif
