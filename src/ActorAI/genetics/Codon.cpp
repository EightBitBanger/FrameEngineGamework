#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

#include <sstream>

Codon::Codon() : 
    x(0.0f), 
    y(0.0f), 
    z(0.0f) 
{}

Codon::Codon(float xx, float yy, float zz) : 
    x(xx), 
    y(yy), 
    z(zz) 
{}

std::string Codon::ToString(void) const {
    std::ostringstream oss;
    oss << x << " " << y << " " << z;
    return oss.str();
}

glm::vec3 Codon::ToVec3(void) const {
    return glm::vec3(x, y, z);
}

glm::vec3 Codon::operator+(const Codon& rhs) const {
    return glm::vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Codon& Codon::operator+=(const Codon& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

glm::vec3 Codon::operator+(const glm::vec3& rhs) const {
    return glm::vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Codon::operator glm::vec3() const {
    return glm::vec3(x, y, z);
}
