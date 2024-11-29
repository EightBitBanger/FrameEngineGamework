#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>

class ENGINE_API WorldGeneration {
    
public:
    
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
    
    std::vector<Decoration> mDecorations;
    
    
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
    
    float renderDistance;
    
    int chunkSize;
    
    int worldSeed;
    
    int ChunkCounterX;
    int ChunkCounterZ;
    
    ChunkManager();
    
    void SaveWorld(void);
    
    void LoadWorld(void);
    
    void ClearWorld(void);
    
    
    Chunk CreateChunk(float x, float y);
    
    bool DestroyChunk(Chunk& chunkPtr);
    
    
    void Initiate(void);
    
    void Update(void);
    
    
    void Decorate(Chunk& chunk, int chunkX, int chunkZ, Mesh* staticMesh);
    
    
    std::vector<Decoration> decoration;
    std::vector<Perlin> perlin;
    
    std::vector<Chunk> chunkList;
    
private:
    
    /// World generation meshes.
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    
    SubMesh subMeshGrassHorz;
    SubMesh subMeshGrassVert;
    
    SubMesh subMeshStemHorz;
    SubMesh subMeshStemVert;
    
    SubMesh subMeshTree;
    
    /// Water mesh
    Mesh* watermesh;
    Material* watermaterial;
    
};
