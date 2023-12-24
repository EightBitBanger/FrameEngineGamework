#include <GameEngineFramework/ActorAI/Genetics/Base.h>

#include <sstream>

BaseGene::BaseGene() : 
    x(0),
    y(0),
    z(0)
{
}

BaseGene::BaseGene(float xx, float yy, float zz) : 
    x(xx),
    y(yy),
    z(zz)
{
}

std::string BaseGene::ToString(void) {
    std::stringstream sstream;
    sstream << x << " " << y << " " << z;
    return sstream.str();
}
