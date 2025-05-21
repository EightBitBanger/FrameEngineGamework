#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>

extern NumberGeneration  Random;
extern ColorPreset       Colors;
extern FloatType         Float;
extern IntType           Int;
extern UIntType          UInt;
extern StringType        String;
extern RenderSystem      Renderer;
extern ActorSystem       AI;


void GeneticPresets::ActorPresets::Human(Actor* targetActor) {
    
    return;
}



void GeneticPresets::ActorPresets::Sheep(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Sheep");
    
    targetActor->physical.SetAdultAge( 400 );
    targetActor->physical.SetSeniorAge( 4000 );
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(0.9);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.5f);
    targetActor->physical.SetAdultScale(1.1f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 20;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.dkgray;
    Color bodyColor = Colors.dkgray;
    Color limbColor = Colors.dkgray;
    
    headColor = Colors.Range(Colors.gray, Colors.dkgray) * (Random.Range(1, 100) * 0.008f);
    bodyColor = Colors.Range(Colors.gray, Colors.black)  * (Random.Range(1, 100) * 0.008f);
    limbColor = Colors.Range(Colors.gray, Colors.dkgray) * (Random.Range(1, 100) * 0.008f);
    
    bool selected = false;
    
    // Brown
    if ((!selected) & (Random.Range(0, 100) > 80)) {
        bodyColor = Colors.brown * 0.04f;
        selected = true;
    }
    
    // Black
    if ((!selected) & (Random.Range(0, 100) > 90)) {
        headColor = Colors.Range(Colors.gray, Colors.dkgray) * 0.08;
        bodyColor = Colors.black;
        limbColor = Colors.Range(Colors.gray, Colors.dkgray) * 0.05;
        selected = true;
    }
    
    // Rare - pink
    if ((!selected) & (Random.Range(0, 1000000) > 999990)) {
        bodyColor = Colors.purple * 0.7f;
        selected = true;
    }
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.7, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.4, 0.4, 0.9);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.02, 0.254);
    geneHead.position  = Codon(0, 0, 0.3);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    
    return;
}



void GeneticPresets::ActorPresets::Bovine(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Ox");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(5000);
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(0.9);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.2f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->biological.health = 170;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor  = Colors.dkgray;
    Color bodyColor  = Colors.dkgray;
    Color limbColor  = Colors.dkgray;
    Color hornColor  = Colors.MakeGrayScale(0.037);
    Color udderColor = Colors.Lerp(Colors.MakeGrayScale(0.1), Colors.red, 0.2);
    
    headColor *= Colors.MakeRandomGrayScale();
    bodyColor *= Colors.MakeRandomGrayScale();
    limbColor *= Colors.MakeRandomGrayScale();
    udderColor *= Colors.MakeGrayScale(0.3);
    
    if (Random.Range(0, 100) > 50) headColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) bodyColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) limbColor *= Colors.brown;
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.5, 0.5, 1.2);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.1, 0.45);
    geneHead.position  = Codon(0, 0, 0.35);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45, 0.42, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Horns
    Gene geneHornLeft;
    geneHornLeft.attachmentIndex = 2;
    geneHornLeft.offset    = Codon(0, 0, 0);
    geneHornLeft.position  = Codon(0.25, 0.2, 0.22);
    geneHornLeft.rotation  = Codon(0, 0, -0.3);
    geneHornLeft.scale     = Codon(0.4, 0.1, 0.1);
    geneHornLeft.color.x   = hornColor.r;
    geneHornLeft.color.y   = hornColor.g;
    geneHornLeft.color.z   = hornColor.b;
    geneHornLeft.type = EXPRESSION_TYPE_MALE;
    geneHornLeft.expressionFactor = 1.0f;
    
    Gene geneHornLeftUpper;
    geneHornLeftUpper.attachmentIndex = 2;
    geneHornLeftUpper.offset    = Codon(0, 0, 0);
    geneHornLeftUpper.position  = Codon(0.5, 0.2, 0.22);
    geneHornLeftUpper.rotation  = Codon(0, 0, 0.75);
    geneHornLeftUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornLeftUpper.color.x   = hornColor.r;
    geneHornLeftUpper.color.y   = hornColor.g;
    geneHornLeftUpper.color.z   = hornColor.b;
    geneHornLeftUpper.type = EXPRESSION_TYPE_MALE;
    geneHornLeftUpper.expressionFactor = 1.0f;
    
    Gene geneHornRight;
    geneHornRight.attachmentIndex = 2;
    geneHornRight.offset    = Codon(0, 0, 0);
    geneHornRight.position  = Codon(-0.25, 0.2, 0.22);
    geneHornRight.rotation  = Codon(0, 0, 0.3);
    geneHornRight.scale     = Codon(0.4, 0.1, 0.1);
    geneHornRight.color.x   = hornColor.r;
    geneHornRight.color.y   = hornColor.g;
    geneHornRight.color.z   = hornColor.b;
    geneHornRight.type = EXPRESSION_TYPE_MALE;
    geneHornRight.expressionFactor = 1.0f;
    
    Gene geneHornRightUpper;
    geneHornRightUpper.attachmentIndex = 2;
    geneHornRightUpper.offset    = Codon(0, 0, 0);
    geneHornRightUpper.position  = Codon(-0.5, 0.2, 0.22);
    geneHornRightUpper.rotation  = Codon(0, 0, -0.75);
    geneHornRightUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornRightUpper.color.x   = hornColor.r;
    geneHornRightUpper.color.y   = hornColor.g;
    geneHornRightUpper.color.z   = hornColor.b;
    geneHornRightUpper.type = EXPRESSION_TYPE_MALE;
    geneHornRightUpper.expressionFactor = 1.0f;
    
    // Udders
    Gene geneUdders;
    geneUdders.offset    = Codon(0, 0, 0);
    geneUdders.position  = Codon(0, 0.68, -0.3);
    geneUdders.rotation  = Codon(0, 0, 0);
    geneUdders.scale     = Codon(0.2, 0.3, 0.24);
    geneUdders.color.x   = udderColor.r;
    geneUdders.color.y   = udderColor.g;
    geneUdders.color.z   = udderColor.b;
    geneUdders.type = EXPRESSION_TYPE_FEMALE;
    geneUdders.expressionFactor = 1.0f;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 10;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 10;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22, 0.75, -0.45);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 10;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22, 0.75, -0.45);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Tail
    Gene geneTail;
    geneTail.offset    = Codon(0, 0.85, -0.60);
    geneTail.position  = Codon(0, 0, 0);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.1, 0.52, 0.1);
    geneTail.color.x   = bodyColor.r;
    geneTail.color.y   = bodyColor.g;
    geneTail.color.z   = bodyColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneHornLeft);
    targetActor->genetics.AddGene(geneHornLeftUpper);
    targetActor->genetics.AddGene(geneHornRight);
    targetActor->genetics.AddGene(geneHornRightUpper);
    targetActor->genetics.AddGene(geneUdders);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
    return;
}



void GeneticPresets::ActorPresets::Horse(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Horse");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(1.1);
    targetActor->physical.SetSpeedYouth(1.1);
    targetActor->physical.SetSpeedMultiplier(2.4f);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.2f);
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    
    targetActor->biological.health = 150;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color Variants (Brown, Black, or White)
    Color headColor = Colors.dkgray;
    Color maneColor = Colors.dkgray;
    Color neckColor = Colors.dkgray;
    Color bodyColor = Colors.dkgray;
    Color tailColor = Colors.dkgray;
    
    if (Random.Range(0, 100) > 30) headColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) maneColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) neckColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) bodyColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) tailColor *= Colors.brown;
    
    headColor *= Colors.MakeRandomGrayScale();
    maneColor *= Colors.MakeRandomGrayScale();
    neckColor *= Colors.MakeRandomGrayScale();
    bodyColor *= Colors.MakeRandomGrayScale();
    tailColor *= Colors.MakeRandomGrayScale();
    
    // Limb colors
    Color limbFRColor = Colors.gray;
    Color limbFLColor = Colors.gray;
    Color limbRRColor = Colors.gray;
    Color limbRLColor = Colors.gray;
    
    if (Random.Range(0, 100) > 30) limbFRColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbFLColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbRRColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbRLColor *= Colors.brown;
    
    limbFRColor *= Colors.MakeRandomGrayScale();
    limbFLColor *= Colors.MakeRandomGrayScale();
    limbRRColor *= Colors.MakeRandomGrayScale();
    limbRLColor *= Colors.MakeRandomGrayScale();
    
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.black; limbFLColor = Colors.black;
        limbRRColor = Colors.black; limbRLColor = Colors.black;
    }
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.white; limbFLColor = Colors.white;
        limbRRColor = Colors.white; limbRLColor = Colors.white;
    }
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 1.0, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.5, 0.5, 1.5);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Neck
    Gene geneNeck;
    geneNeck.offset    = Codon( 0.0, 0.0, 0.0);
    geneNeck.position  = Codon( 0.0, 1.3, 0.72);
    geneNeck.rotation  = Codon( 0.5, 0.0, 0.0);
    geneNeck.scale     = Codon( 0.32,  1.0, 0.3);
    geneNeck.color.x   = neckColor.r;
    geneNeck.color.y   = neckColor.g;
    geneNeck.color.z   = neckColor.b;
    
    // Head upper
    Gene geneHead;
    geneHead.attachmentIndex = 2;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.45, 1.2);
    geneHead.rotation  = Codon(0.3, 0, 0);
    geneHead.scale     = Codon(0.27, 0.25, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Head lower
    Gene geneHeadLower;
    geneHeadLower.attachmentIndex = 2;
    geneHeadLower.offset    = Codon(0, 0, 0);
    geneHeadLower.position  = Codon(0, 1.41, 1.13);
    geneHeadLower.rotation  = Codon(0.4, 0, 0);
    geneHeadLower.scale     = Codon(0.22, 0.29, 0.43);
    geneHeadLower.color.x   = headColor.r;
    geneHeadLower.color.y   = headColor.g;
    geneHeadLower.color.z   = headColor.b;
    
    // Mane
    Gene geneMane;
    geneMane.attachmentIndex = 2;
    geneMane.offset    = Codon( 0.0, 0.0, 0.0);
    geneMane.position  = Codon( 0.0, 1.34, 0.52);
    geneMane.rotation  = Codon( 0.54, 0.0, 0.0);
    geneMane.scale     = Codon( 0.1,  0.9, 0.2);
    geneMane.color.x   = maneColor.r;
    geneMane.color.y   = maneColor.g;
    geneMane.color.z   = maneColor.b;
    
    // Ear left
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = Codon( 0.0, 0.0, 0.0);
    geneEarLeft.position  = Codon( 0.1, 1.8, 0.84);
    geneEarLeft.rotation  = Codon( 0.2, 0.0, 0.0);
    geneEarLeft.scale     = Codon( 0.08, 0.2, 0.05);
    geneEarLeft.color.x   = maneColor.r;
    geneEarLeft.color.y   = maneColor.g;
    geneEarLeft.color.z   = maneColor.b;
    
    // Ear right
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = Codon( 0.0, 0.0, 0.0);
    geneEarRight.position  = Codon(-0.1, 1.8, 0.84);
    geneEarRight.rotation  = Codon( 0.2, 0.0, 0.0);
    geneEarRight.scale     = Codon( 0.08, 0.2, 0.05);
    geneEarRight.color.x   = maneColor.r;
    geneEarRight.color.y   = maneColor.g;
    geneEarRight.color.z   = maneColor.b;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.2, 1.0, 0.6);
    geneLimbFrontLeft.position  = Codon(0, -0.6, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.9, 0.2);
    geneLimbFrontLeft.color.x   = limbFLColor.r;
    geneLimbFrontLeft.color.y   = limbFLColor.g;
    geneLimbFrontLeft.color.z   = limbFLColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 12;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.2, 1.0, 0.6);
    geneLimbFrontRight.position  = Codon(0, -0.6, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.9, 0.2);
    geneLimbFrontRight.color.x   = limbFRColor.r;
    geneLimbFrontRight.color.y   = limbFRColor.g;
    geneLimbFrontRight.color.z   = limbFRColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 12;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.2, 1.0, -0.6);
    geneLimbRearLeft.position  = Codon(0, -0.6, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.9, 0.2);
    geneLimbRearLeft.color.x   = limbRLColor.r;
    geneLimbRearLeft.color.y   = limbRLColor.g;
    geneLimbRearLeft.color.z   = limbRLColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 12;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.2, 1.0, -0.6);
    geneLimbRearRight.position  = Codon(0, -0.6, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.9, 0.2);
    geneLimbRearRight.color.x   = limbRRColor.r;
    geneLimbRearRight.color.y   = limbRRColor.g;
    geneLimbRearRight.color.z   = limbRRColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 12;
    
    // Tail
    Gene geneTail;
    geneTail.offset    = Codon(0, 0, 0);
    geneTail.position  = Codon(0, 0.87, -0.8);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.1, 0.6, 0.1);
    geneTail.color.x   = tailColor.r;
    geneTail.color.y   = tailColor.g;
    geneTail.color.z   = tailColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneNeck);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneHeadLower);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneMane);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
    return;
}



void GeneticPresets::ActorPresets::Bear(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Bear");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(1.2f);
    targetActor->physical.SetSpeedYouth(1.4f);
    targetActor->physical.SetSpeedMultiplier(3.4);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.3f);
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    targetActor->behavior.SetHeightPreferenceMin(20.0f);
    
    targetActor->behavior.SetPredatorState(true);
    
    targetActor->biological.health = 280;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    bool selected = false;
    
    // Default brown bear
    Color headColor = Colors.brown;
    Color bodyColor = Colors.brown;
    Color limbColor = Colors.brown;
    
    headColor *= Colors.MakeGrayScale(0.02);
    bodyColor *= Colors.MakeGrayScale(0.018);
    limbColor *= Colors.MakeGrayScale(0.03);
    
    // Grizzly bear
    if ((!selected) & (Random.Range(0, 100) > 70)) {
        
        headColor = Colors.brown * Colors.MakeGrayScale(0.02);
        bodyColor = Colors.brown * Colors.MakeGrayScale(0.02);
        limbColor = Colors.brown * Colors.MakeGrayScale(0.03);
        
        selected = true;
    }
    
    // Black bear
    if ((!selected) & (Random.Range(0, 100) > 80)) {
        
        headColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.01);
        bodyColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.001);
        
        selected = true;
    }
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.68, 0.65, 1.1);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Body abdomen
    Gene geneAbdomin;
    geneAbdomin.offset    = Codon(0, 0, 0);
    geneAbdomin.position  = Codon(0, 0.9, -0.3);
    geneAbdomin.rotation  = Codon(0, 0, 0);
    geneAbdomin.scale     = Codon(0.78, 0.75, 0.8);
    geneAbdomin.color.x   = bodyColor.r;
    geneAbdomin.color.y   = bodyColor.g;
    geneAbdomin.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.05, 0.33);
    geneHead.position  = Codon(0, -0.1, 0.35);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 2;
    geneMuzzle.offset    = Codon(0.0, 0.0, 0.0);
    geneMuzzle.position  = Codon(0.0, -0.14, 0.6);
    geneMuzzle.rotation  = Codon(0, 0, 0);
    geneMuzzle.scale     = Codon(0.25, 0.2, 0.2);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = Codon(0.0, 0.0, 0.0);
    geneEarLeft.position  = Codon(0.1, 0.13, 0.34);
    geneEarLeft.rotation  = Codon(0, 0, 0);
    geneEarLeft.scale     = Codon(0.1, 0.1, 0.1);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = Codon(0.0, 0.0, 0.0);
    geneEarRight.position  = Codon(-0.1, 0.13, 0.34);
    geneEarRight.rotation  = Codon(0, 0, 0);
    geneEarRight.scale     = Codon(0.1, 0.1, 0.1);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0.02, -0.35, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.24, 0.5, 0.25);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(-0.02, -0.35, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.24, 0.5, 0.25);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = Codon(0.07, -0.35, -0.04);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.25, 0.65, 0.35);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = Codon(-0.07, -0.35, -0.04);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.25, 0.65, 0.35);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneMuzzle);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneAbdomin);
    
    return;
}



void GeneticPresets::ActorPresets::Dog(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Dog");
    
    targetActor->physical.SetAdultAge(750);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(1.2f);
    targetActor->physical.SetSpeedYouth(0.9f);
    targetActor->physical.SetSpeedMultiplier(2.0f);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(10.0f);
    
    targetActor->behavior.SetDistanceToAttack(8.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetPredatorState(true);
    
    targetActor->biological.health = 80;
    targetActor->biological.strength = 8;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.MakeRandomGrayScale();
    Color bodyColor = Colors.MakeRandomGrayScale();
    Color limbColor = Colors.MakeRandomGrayScale();
    Color tailColor = Colors.MakeRandomGrayScale();
    
    headColor *= 0.7f;
    bodyColor *= 0.5f;
    limbColor *= 0.3f;
    tailColor *= 0.2f;
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, -0.1);
    geneBody.position  = Codon(0, 0.34, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.25, 0.25, 0.4);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0.0, 0, 0.3);
    geneHead.position  = Codon(0, 0.43, 0.08);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.23, 0.23, 0.18);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Upper body gene
    Gene geneBodyUpper;
    geneBodyUpper.attachmentIndex = 1;
    geneBodyUpper.offset    = Codon(0, 0, 0);
    geneBodyUpper.position  = Codon(0, 0.35, 0.3);
    geneBodyUpper.rotation  = Codon(0, 0, 0);
    geneBodyUpper.scale     = Codon(0.29, 0.29, 0.22);
    geneBodyUpper.color.x   = bodyColor.r;
    geneBodyUpper.color.y   = bodyColor.g;
    geneBodyUpper.color.z   = bodyColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.offset    = Codon(0, 0, 0.3);
    geneMuzzle.position  = Codon(0, 0.4, 0.18);
    geneMuzzle.rotation  = Codon(0, 0, 0);
    geneMuzzle.scale     = Codon(0.15, 0.15, 0.14);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    geneMuzzle.animationType = ActorState::Animation::Head;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.offset    = Codon(0, 0, 0.3);
    geneEarLeft.position  = Codon(0.07, 0.555, 0.13);
    geneEarLeft.rotation  = Codon(0.1, 0, -0.01);
    geneEarLeft.scale     = Codon(0.07, 0.1, 0.04);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    geneEarLeft.animationType = ActorState::Animation::Head;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.offset    = Codon(0, 0, 0.3);
    geneEarRight.position  = Codon(-0.07, 0.555, 0.13);
    geneEarRight.rotation  = Codon(0.1, 0, 0.01);
    geneEarRight.scale     = Codon(0.07, 0.1, 0.04);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    geneEarRight.animationType = ActorState::Animation::Head;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.11, 0.35, 0.21);
    geneLimbFrontLeft.position  = Codon(0.0, -0.2, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.13, 0.4, 0.13);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.11, 0.35, 0.21);
    geneLimbFrontRight.position  = Codon(0.0, -0.2, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.13, 0.4, 0.13);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.08, 0.3, -0.18);
    geneLimbRearLeft.position  = Codon(0.0, -0.2, -0.04);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.13, 0.35, 0.13);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.08, 0.3, -0.18);
    geneLimbRearRight.position  = Codon(-0.01, -0.2, -0.04);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.13, 0.35, 0.13);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Tail gene
    Gene geneTail;
    geneTail.offset    = Codon(0.0, 0.0, 0.0);
    geneTail.position  = Codon(0.0, 0.32, -0.33);
    geneTail.rotation  = Codon(-1.2, 0, 0);
    geneTail.scale     = Codon(0.07, 0.07, 0.3);
    geneTail.color.x   = tailColor.r;
    geneTail.color.y   = tailColor.g;
    geneTail.color.z   = tailColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneBodyUpper);
    targetActor->genetics.AddGene(geneMuzzle);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
    return;
}



void GeneticPresets::ActorPresets::Creepy(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Creepy");
    
    targetActor->physical.SetAge(0);
    targetActor->physical.SetAdultAge(0);
    targetActor->physical.SetSeniorAge(1000000);
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(1.0);
    targetActor->physical.SetSpeedMultiplier(1.0f);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->behavior.SetPredatorState(true);
    
    targetActor->biological.health = 50;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.green;
    Color bodyColor = Colors.green;
    Color limbColor = Colors.green;
    
    headColor *= Colors.MakeRandomGrayScale() * 0.3;
    bodyColor *= Colors.MakeRandomGrayScale() * 0.3;
    limbColor *= Colors.MakeRandomGrayScale() * 0.3;
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.8, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.4, 0.8, 0.4);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.45, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45, 0.45, 0.45);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22, 0.6, 0.22);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 10;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22, 0.6, 0.22);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 10;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22, 0.6, -0.22);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 10;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22, 0.6, -0.22);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    
    return;
}

