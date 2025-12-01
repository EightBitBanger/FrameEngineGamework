#ifndef _STRUCTURE__
#define _STRUCTURE__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>

struct StructurePart {
    glm::vec3 offset;
    glm::vec3 rotation;
    glm::vec3 scale;
    Color color;
};



class ENGINE_API Structure {
    
public:
    
    std::string name;
    float buildSpread;
    
    unsigned int actorCountMin;
    unsigned int actorCountMax;
    
    float actorSpread;
    
    std::vector<StructurePart> parts;
    
    Structure() : 
        name(""),
        buildSpread(0.0f),
        actorCountMin(0),
        actorCountMax(0),
        actorSpread(0.0f)
    {
    }
    
};

#endif
