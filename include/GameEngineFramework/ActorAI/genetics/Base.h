#ifndef _GENETICS_BASE_GENE__
#define _GENETICS_BASE_GENE__

#include <GameEngineFramework/configuration.h>

#include <string>

class ENGINE_API BaseGene {
    
public:
    
    float x;
    float y;
    float z;
    
    BaseGene();
    
    BaseGene(float xx, float yy, float zz);
    
    std::string ToString(void);
    
private:
    
};


#endif
