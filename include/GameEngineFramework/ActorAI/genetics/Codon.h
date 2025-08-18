#ifndef _GENETICS_BASE_GENE__
#define _GENETICS_BASE_GENE__

#include <glm/glm.hpp>
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
    
    glm::vec3 ToVec3(void);
    
private:
    
};


#endif
