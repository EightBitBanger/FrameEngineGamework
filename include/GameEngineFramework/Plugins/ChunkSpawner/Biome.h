#ifndef _BIOME_LAYER__
#define _BIOME_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>

class ENGINE_API Biome {
    
public:
    
    Biome() : 
        color(1, 1, 1),
        
        heightMultuplier(0),
        noiseWidth(0),
        noiseHeight(0)
    {}
    
    glm::vec3 color;
    
    float heightMultuplier;
    float noiseWidth;
    float noiseHeight;
    
};

#endif
