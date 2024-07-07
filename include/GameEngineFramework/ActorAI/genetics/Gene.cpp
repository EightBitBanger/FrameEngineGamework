#include <GameEngineFramework/ActorAI/Genetics/Base.h>
#include <GameEngineFramework/ActorAI/Genetics/Gene.h>

Gene::Gene() : 
    doExpress(true),
    doAnimationCycle(false),
    doInverseAnimation(false),
    attachmentIndex(0),
    offset(0, 0, 0),
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    color(0, 0, 0),
    animationRange(0),
    animationAxis(0, 0, 0)
{
}
