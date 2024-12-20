#ifndef _STRUCTURE__
#define _STRUCTURE__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>


class ENGINE_API DecorationElement {
    
public:
    
    /// Decoration type
    int type;
    
    /// Position relative to the structure center
    glm::vec3 position;
    
    DecorationElement() : 
        type(DECORATION_GRASS),
        position(glm::vec3(0))
    {
    }
};

class ENGINE_API Structure {
    
public:
    
    std::string name;
    
    std::vector<DecorationElement> elements;
    
    Structure() : 
        name("")
    {
    }
    
};

#endif
