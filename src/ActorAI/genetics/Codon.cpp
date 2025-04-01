#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

#include <sstream>

Codon::Codon() : 
    x(0),
    y(0),
    z(0)
{
}

Codon::Codon(float xx, float yy, float zz) : 
    x(xx),
    y(yy),
    z(zz)
{
}

std::string Codon::ToString(void) {
    std::stringstream sstream;
    sstream << x << " " << y << " " << z;
    return sstream.str();
}
