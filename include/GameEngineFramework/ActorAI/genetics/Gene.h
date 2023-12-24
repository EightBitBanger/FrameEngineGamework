#ifndef _GENETICS_GENE__
#define _GENETICS_GENE__

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/ActorAI/Genetics/Base.h>



class ENGINE_API Gene {
    
public:
    
    /// Should this gene be expressed.
    bool doExpress;
    
    /// This gene should be animated.
    bool useAnimation;
    
    // Base genes in this gene.
    
    /// Model position offset.
    BaseGene offset;
    BaseGene position;
    BaseGene rotation;
    BaseGene scale;
    BaseGene color;
    
    Gene();
    
private:
    
};

#endif
