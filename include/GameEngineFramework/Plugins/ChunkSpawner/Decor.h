#ifndef _DECOR_LAYER__
#define _DECOR_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>

#define DECORATION_GRASS_THIN   0
#define DECORATION_GRASS        1
#define DECORATION_GRASS_THICK  2
#define DECORATION_TREE         3
#define DECORATION_ACTOR_SHEEP  4

class Decoration {
    
public:
    
    Decoration() : 
        type(0),
        density(10),
        
        spawnHeightMinimum(0),
        spawnHeightMaximum(100)
        
    {}
    
    /// Decoration type
    /// 0  = Thin grass
    /// 1  = Grass
    /// 2  = Thick grass
    /// 3  = Tree
    /// 4  = Actor - sheep
    int type;
    
    /// Spawn rate factor.
    unsigned int density;
    
    /// Minimum spawn height.
    float spawnHeightMinimum;
    
    /// Maximum spawn height.
    float spawnHeightMaximum;
    
    
};

#endif
