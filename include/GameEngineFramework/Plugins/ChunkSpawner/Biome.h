#ifndef _BIOME_LAYER__
#define _BIOME_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>

class ENGINE_API Biome {
    
public:
    
    glm::vec3 color;
    
    float noiseWidth;
    float noiseHeight;
    
    float offsetX;
    float offsetZ;
    
    std::vector<Perlin> perlin;
    
    std::vector<DecorationSpecifier> decorations;
    
    Biome() : 
        color(1, 1, 1),
        noiseWidth(0),
        noiseHeight(0),
        offsetX(0),
        offsetZ(0)
    {}
    
};

#endif
