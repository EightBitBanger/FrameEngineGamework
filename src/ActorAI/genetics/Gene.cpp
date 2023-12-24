#include <GameEngineFramework/ActorAI/Genetics/Base.h>
#include <GameEngineFramework/ActorAI/Genetics/Gene.h>

Gene::Gene() : 
    doExpress(true),
    offset(0, 0, 0),
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    color(0, 0, 0)
{
}

