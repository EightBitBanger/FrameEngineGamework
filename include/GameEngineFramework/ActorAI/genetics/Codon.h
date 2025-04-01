#ifndef _GENETICS_BASE_GENE__
#define _GENETICS_BASE_GENE__

#include <GameEngineFramework/configuration.h>
#include <string>

class ENGINE_API Codon {
    
public:
    
    float x;
    float y;
    float z;
    
    Codon();
    
    Codon(float xx, float yy, float zz);
    
    std::string ToString(void);
    
private:
    
};


#endif
