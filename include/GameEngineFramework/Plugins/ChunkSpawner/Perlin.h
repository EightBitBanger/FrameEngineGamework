#ifndef _PERLIN_LAYER__
#define _PERLIN_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>


class ENGINE_API Perlin {
    
public:
    
    Perlin() : 
        heightMultuplier(0.2),
        heightThreshold(0.0f),
        offsetX(0),
        offsetY(0),
        noiseWidth(0.2),
        noiseHeight(0.2)
    {}
    
    /// Mountain height multiplier
    float heightMultuplier;
    
    /// Height threshold.
    float heightThreshold;
    
    /// Horizontal offset
    float offsetX;
    
    /// Vertical offset
    float offsetY;
    
    /// Horizontal noise
    float noiseWidth;
    
    /// Vertical noise
    float noiseHeight;
    
};

#endif
