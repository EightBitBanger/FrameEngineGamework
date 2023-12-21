#ifndef _GENETICS_GENE__
#define _GENETICS_GENE__

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/ActorAI/Genetics/Base.h>



class ENGINE_API Gene {
    
public:
    
    /// Should this gene be expressed.
    bool doExpress;
    
    // 0-Offset
    // 1-Position
    // 2-Rotation
    // 3-Scale
    // 4-Color
    
    /// Base genes within this gene.
    BaseGene mBaseGenes[5];
    
    Gene();
    
private:
    
};

#endif
