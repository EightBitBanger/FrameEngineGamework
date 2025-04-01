#ifndef _GENETICS_PHEN__
#define _GENETICS_PHEN__

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

class ENGINE_API Phen {
    
public:
    
    /// Final phenotypic expression modifiers
    Codon scale;
    Codon color;
    
    Phen();
    
};

#endif
