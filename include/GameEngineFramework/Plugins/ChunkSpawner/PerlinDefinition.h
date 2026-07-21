#ifndef _PERLIN_LAYER__
#define _PERLIN_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>


class ENGINE_API Perlin {
public:
    
    Perlin() : 
        heightMultuplier(0.2f),
        offsetX(0),
        offsetY(0),
        noiseWidth(0.2f),
        noiseHeight(0.2f),
        heightMax(100.0f),
        heightMin(-100.0f),
        heightBlowoutHeight(1.0f),
        heightBlowoutMul(1.0f)
    {}
    
    /// Mountain height multiplier
    float heightMultuplier;
    
    /// Horizontal offset
    float offsetX;
    
    /// Vertical offset
    float offsetY;
    
    /// Horizontal noise
    float noiseWidth;
    
    /// Vertical noise
    float noiseHeight;
    
    /// Maximum generation height.
    float heightMax;
    
    /// Minimum generation height.
    float heightMin;
    
    /// Height point where height is multiplied.
    float heightBlowoutHeight;
    
    /// Multiplier to effect blowout rate.
    float heightBlowoutMul;
    
};

#endif
