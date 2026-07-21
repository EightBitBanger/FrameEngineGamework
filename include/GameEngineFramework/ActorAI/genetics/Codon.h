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
    
    std::string ToString(void) const;
    
    glm::vec3 ToVec3(void) const;
    
    glm::vec3 operator+(const Codon& rhs) const;
    
    Codon& operator+=(const Codon& rhs);
    
    glm::vec3 operator+(const glm::vec3& rhs) const;
    
    operator glm::vec3() const;
};

inline glm::vec3 operator+(const glm::vec3& lhs, const Codon& rhs) {
    return lhs + rhs.ToVec3();
}

#endif
