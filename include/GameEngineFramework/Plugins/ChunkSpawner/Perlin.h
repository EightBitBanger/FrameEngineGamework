#ifndef _PERLIN_LAYER__
#define _PERLIN_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>


class Perlin {
    
public:
    
    Perlin() : 
        heightMultuplier(0.2),
        noiseWidth(0.2),
        noiseHeight(0.2),
        equation(0)
    {}
    
    /// Mountain height multiplier
    float heightMultuplier;
    
    /// Perlin noise value horizontal
    float noiseWidth;
    
    /// Perlin noise value vertical
    float noiseHeight;
    
    /// Layer equation
    /// 0 = Additive
    /// 1 = Average
    short int equation;
    
};

#endif
