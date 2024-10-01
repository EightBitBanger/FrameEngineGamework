#ifndef _GENETICS_GENE__
#define _GENETICS_GENE__

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/ActorAI/Genetics/Base.h>



class ENGINE_API Gene {
    
public:
    
    /// Offset position from center.
    BaseGene offset;
    
    /// Final position.
    BaseGene position;
    
    /// Final rotation.
    BaseGene rotation;
    
    /// Final scale.
    BaseGene scale;
    
    /// Render color.
    BaseGene color;
    
    /// Animation max swing range.
    float animationRange;
    
    /// Animation swing rate multiplier.
    BaseGene animationAxis;
    
    /// Offset gene index to which this gene will attach and generate.
    unsigned int attachmentIndex;
    
    /// Should this gene be expressed.
    bool doExpress;
    
    /// Should this gene be animated.
    bool doAnimationCycle;
    
    /// Should the animation cycle be inverted.
    bool doInverseAnimation;
    
    Gene();
    
};

#endif
