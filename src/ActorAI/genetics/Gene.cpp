#include <GameEngineFramework/ActorAI/Genetics/Gene.h>

Gene::Gene() : 
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    color(0, 0, 0),
    offset(0, 0, 0),
    attachmentIndex(0),
    colorIndex(0),
    scaleIndex(0),
    type(EXPRESSION_TYPE_BASE),
    expressionFactor(0.1f),
    expressionMax(1.0f),
    expressionAge(0),
    doExpress(true),
    doAnimateAsHead(false),
    doAnimationCycle(false),
    doInverseAnimation(false),
    animationRange(15),
    animationAxis(1, 0, 0)
{
}
