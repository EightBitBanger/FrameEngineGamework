#ifndef _DECORATION_LAYER__
#define _DECORATION_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>

struct Decoration {
    
    enum class Grass {
        Short,
        Tall,
        Dead,
        Marsh,
        Dry,
        Crab,
        Fern,
        Flowers,
        Clover
    };
    
    enum class Tree {
        Oak,
        Pine,
        Spruce,
        Birch,
        Maple,
        Palm,
        Willow,
        Cedar,
        Redwood,
        Dead,
        Bonsai,
        Baobab,
        MushroomTree
    };
    
    enum class Actor {
        Human,
        Sheep,
        Bear,
        Dog,
        Bovine,
        Horse
    };
};


enum class DecorationType {
    Grass,
    Tree,
    Structure,
    Actor
};



class ENGINE_API DecorationSpecifier {
    
public:
    
    DecorationType type;
    
    /// Decoration subtype name.
    std::string name;
    
    /// Spawn rate factor.
    unsigned int density;
    
    /// Base generation color.
    glm::vec3 colorLow;
    
    /// Fade to top generation color.
    glm::vec3 colorHigh;
    
    /// Minimum generation height.
    float spawnHeightMinimum;
    
    /// Maximum generation height.
    float spawnHeightMaximum;
    
    /// Threshold to generate decoration from perlin noise.
    float threshold;
    
    /// Perlin noise multiplier.
    float noise;
    
    DecorationSpecifier() : 
        type(DecorationType::Grass),
        name(""),
        density(10),
        
        spawnHeightMinimum(0),
        spawnHeightMaximum(100),
        
        threshold(0.1f),
        noise(0.1f)
    {
    }
    
};

#endif
