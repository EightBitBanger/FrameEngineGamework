#ifndef _BIOME_LAYER__
#define _BIOME_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/PerlinDefinition.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/ClassDefinition.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/DecorationSpecifier.h>

class ENGINE_API Biome {
    
public:
    
    /// Main biome color range.
    Color colorLow;
    Color colorHigh;
    
    /// Rate of shift from color low to high.
    float colorBias;
    
    /// Beginning height where the color shifts from high to low.
    float colorHeight;
    
    class GenerationRegion {
        
    public:
        float noiseWidth;
        float noiseHeight;
        
        float offsetX;
        float offsetZ;
        
        GenerationRegion() : 
            noiseWidth(0),
            noiseHeight(0),
            
            offsetX(0),
            offsetZ(0)
        {}
        
    } region;
    
    std::vector<Perlin> perlin;
    
    std::vector<DecorationSpecifier> decorations;
    
    Biome() : 
        colorLow(1, 1, 1),
        colorHigh(1, 1, 1),
        colorBias(0.87f),
        colorHeight(0.0f)
    {}
    
};

#endif
