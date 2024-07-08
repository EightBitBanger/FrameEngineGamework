#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>

extern NumberGeneration  Random;
extern ColorPreset       Colors;


void GeneticPresets::SheepGene(Actor* targetActor) {
    
    // Personality parameters
    targetActor->SetChanceToChangeDirection(80);
    targetActor->SetChanceToFocusOnActor(200);
    targetActor->SetChanceToStopWalking(100);
    targetActor->SetChanceToWalk(100);
    
    targetActor->SetSpeed(0.7);
    
    // Color variant
    Color headColor;
    Color bodyColor;
    Color limbColor;
    
    headColor = Colors.MakeRandomGrayScale() * Colors.ltgray;
    bodyColor = Colors.MakeRandomGrayScale() * Colors.dkgray;
    limbColor = Colors.MakeRandomGrayScale() * Colors.gray;
    
    // Random off colors
    
    // Brown
    if (Random.Range(0, 10) > 8) 
        bodyColor *= Colors.brown;
    
    // Grays
    if (Random.Range(0, 10) > 9) bodyColor *= Colors.dkgray;
    if (Random.Range(0, 10) > 9) bodyColor *= Colors.ltgray;
    
    if (Random.Range(0, 100) > 97) bodyColor = Colors.purple;
    
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = BaseGene(0, 0, 0);
    geneBody.position  = BaseGene(0, 0.7, 0);
    geneBody.scale     = BaseGene(0.4, 0.4, 0.9);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = BaseGene(0, 1.02, 0.254);
    geneHead.position  = BaseGene(0, 0, 0.3);
    geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
    geneHead.color     = BaseGene(0.4, 0.4, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = BaseGene(0, -0.4, 0);
    geneLimbFrontLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontLeft.color     = BaseGene(0.4, 0.4, 0.4);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = BaseGene(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = BaseGene(0, -0.4, 0);
    geneLimbFrontRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontRight.color     = BaseGene(0.4, 0.4, 0.4);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = BaseGene(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = BaseGene(0, -0.4, 0);
    geneLimbRearLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearLeft.color     = BaseGene(0.4, 0.4, 0.4);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = BaseGene(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = BaseGene(0, -0.4, 0);
    geneLimbRearRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearRight.color     = BaseGene(0.4, 0.4, 0.4);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->AddGene(geneBody);
    targetActor->AddGene(geneHead);
    
    targetActor->AddGene(geneLimbFrontLeft);
    targetActor->AddGene(geneLimbFrontRight);
    targetActor->AddGene(geneLimbRearLeft);
    targetActor->AddGene(geneLimbRearRight);
    
    return;
}

