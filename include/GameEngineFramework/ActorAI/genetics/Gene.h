#ifndef _GENETICS_GENE__
#define _GENETICS_GENE__

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

#define EXPRESSION_TYPE_BASE    0
#define EXPRESSION_TYPE_MALE    1
#define EXPRESSION_TYPE_FEMALE  2


class ENGINE_API Gene {
    
public:
    
    /// Translation, rotation and scale.
    Codon position;
    Codon rotation;
    Codon scale;
    
    /// Surface base color
    Codon color;
    
    /// Position offset for animation and orientation.
    Codon offset;
    
    /// Offset gene index to which this gene will attach and generate.
    /// Zero will cause the gene to inherit from no gene effectively making it a base gene.
    unsigned int attachmentIndex;
    
    /// Color index from which this gene should inherit.
    /// Zero will use the genes current color value.
    unsigned int  colorIndex;
    
    /// Scale index from which this gene should inherit.
    /// Zero will use the genes current scale values.
    unsigned int  scaleIndex;
    
    /// When this gene should be expressed.
    /// 0 = Base gene, always expressed
    /// 1 = Male gene
    /// 2 = Female gene
    unsigned int type;
    
    /// Rate to which this gene will express.
    float expressionFactor;
    
    /// Maximum scale to which the gene will express.
    float expressionMax;
    
    /// Age at which this gene will be expressed.
    unsigned int expressionAge;
    
    /// Should this gene be expressed.
    bool doExpress;
    
    /// Should this gene be animated.
    bool doAnimationCycle;
    
    /// Should the animation cycle be inverted.
    bool doInverseAnimation;
    
    /// Animation max swing range.
    float animationRange;
    
    /// Animation swing direction vector.
    Codon animationAxis;
    
    Gene();
    
};

#endif
