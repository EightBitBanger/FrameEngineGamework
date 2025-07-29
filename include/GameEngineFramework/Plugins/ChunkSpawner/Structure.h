#ifndef _STRUCTURE__
#define _STRUCTURE__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>


class ENGINE_API DecorationElement {
    
public:
    
    /// Decoration type
    DecorationType type;
    
    /// Position relative to the structure center
    glm::vec3 position;
    
    DecorationElement() : 
        type(DecorationType::Grass),
        position(glm::vec3(0))
    {
    }
    
    DecorationElement(DecorationType newType, glm::vec3 newPosition) : 
        type(newType),
        position(newPosition)
    {
    }
    
};

class ENGINE_API Structure {
    
public:
    
    // Structure name
    std::string name;
    
    /// How rare the structure is in the world
    /// Higher number will increase rarity
    unsigned int rarity;
    
    // Decorations included in this structure
    std::vector<DecorationElement> elements;
    
    Structure() : 
        name(""),
        rarity(10)
    {
    }
    
};

#endif
