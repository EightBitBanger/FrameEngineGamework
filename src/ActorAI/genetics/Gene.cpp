#include <GameEngineFramework/ActorAI/Genetics/Base.h>
#include <GameEngineFramework/ActorAI/Genetics/Gene.h>

Gene::Gene() : 
    offset(0, 0, 0),
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    color(0, 0, 0),
    animationRange(15),
    animationAxis(1, 0, 0),
    attachmentIndex(0),
    doExpress(true),
    doAnimationCycle(false),
    doInverseAnimation(false)
{
}
