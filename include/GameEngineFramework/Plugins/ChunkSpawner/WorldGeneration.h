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
    
    float waterLevel;
    
    float ambientLight;
    
    Color waterColorLow;
    Color waterColorHigh;
    
    Color staticColorLow;
    Color staticColorHigh;
    
    Color actorColorLow;
    Color actorColorHigh;
    
    std::vector<Biome> biomes;
    
    std::vector<Structure> structures;
    
    std::unordered_map<std::string, DefinitionTypeTree> definitionTree;
    std::unordered_map<std::string, DefinitionTypeGrass> definitionGrass;
    
    WorldGeneration() :
        name("default"),
        
        doGenerateChunks(false),
        doAutoBreeding(true),
        
        snowCapHeight(60.0f),
        snowCapBias(2.0f),
        snowCapColor(0.7f, 0.85f, 1.1f, 1.0f),
        
        waterLevel(-21.0f),
        
        ambientLight(1.0f),
        
        waterColorLow(Colors.black),
        waterColorHigh(Colors.blue),
        
        staticColorLow(Colors.black),
        staticColorHigh(Colors.white),
        
        actorColorLow(Colors.black),
        actorColorHigh(Colors.white)
    {
    }
    
};


#endif
