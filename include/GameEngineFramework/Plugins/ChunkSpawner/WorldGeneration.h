#ifndef __WORLD_GENERATOR_
#define __WORLD_GENERATOR_

#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Biome.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Structure.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Scenery.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/StaticObject.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/definitions/tree.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/definitions/grass.h>


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
    
    Color staticColorLow;
    Color staticColorHigh;
    
    Color actorColorLow;
    Color actorColorHigh;
    
    std::vector<Biome> biomes;
    
    std::vector<Structure> structures;
    
    std::unordered_map<std::string, DefinitionTypeTree> definitionTree;
    std::unordered_map<std::string, DefinitionTypeGrass> definitionGrass;
    
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


#endif
