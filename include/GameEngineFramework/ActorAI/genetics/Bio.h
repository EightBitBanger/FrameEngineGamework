#ifndef _GENETICS_BIOLOGICAL_MODIFIER__
#define _GENETICS_BIOLOGICAL_MODIFIER__

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

class ENGINE_API Bio {
    
public:
    
    /// Phenotypic expression modifiers
    float inflammation;   // Uniform scale effect
    float health;         // Health level of this biologic expression
    
    Codon tint;           // Additive color effect
    
    Bio();
    
};

#endif
