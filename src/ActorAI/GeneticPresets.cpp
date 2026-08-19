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

void GeneticPresets::ActorPresets::HumanWhite(Actor* targetActor) {
    Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
    Human(targetActor, limbColor, bodyColor);
}

void GeneticPresets::ActorPresets::HumanRed(Actor* targetActor) {
    Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
    
    headColor *= Colors.Lerp(headColor, Colors.red, 0.2f);
    limbColor *= Colors.Lerp(limbColor, Colors.red, 0.2f);
    headColor *= Colors.Lerp(headColor, Colors.terracotta, 0.2f);
    limbColor *= Colors.Lerp(limbColor, Colors.terracotta, 0.2f);
    Human(targetActor, limbColor, bodyColor);
}

void GeneticPresets::ActorPresets::HumanBrown(Actor* targetActor) {
    Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
    
    headColor = Colors.Lerp(headColor, Colors.brown, 0.2) * 0.02f;
    limbColor = Colors.Lerp(limbColor, Colors.brown, 0.2) * 0.02f;
    headColor = Colors.Lerp(headColor, Colors.red, 0.008);
    limbColor = Colors.Lerp(limbColor, Colors.red, 0.008);
    headColor = Colors.Lerp(headColor, Colors.black, 0.6);
    limbColor = Colors.Lerp(limbColor, Colors.black, 0.6);
    Human(targetActor, limbColor, bodyColor);
}

void GeneticPresets::ActorPresets::HumanBlack(Actor* targetActor) {
    Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
    
    headColor = Colors.Lerp(headColor, Colors.black, 0.998);
    limbColor = Colors.Lerp(limbColor, Colors.black, 0.998);
    bodyColor *= 1.4f;
    Human(targetActor, limbColor, bodyColor);
}


void GeneticPresets::ActorPresets::Human(Actor* targetActor, Color& limb, Color& body) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.8,0.35});
    
    targetActor->SetName("Human");
    
    targetActor->physical.SetAdultAge( 18000 );
    targetActor->physical.SetSeniorAge( 60000 );
    
    targetActor->physical.SetSpeed(0.6f);
    targetActor->physical.SetSpeedYouth(0.4f);
    targetActor->physical.SetSpeedMultiplier(2.24f);
    
    targetActor->physical.SetYouthScale(0.1f);
    targetActor->physical.SetAdultScale(0.75f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->behavior.SetDistanceToInflict(0.3f);
    
    targetActor->behavior.SetCooldownAttack(2);
    targetActor->behavior.SetCooldownBreed(8);
    targetActor->behavior.SetCooldownMove(2);
    targetActor->behavior.SetCooldownObserve(8);
    targetActor->behavior.SetCooldownSocial(3);
    
    targetActor->biological.health = 220;
    targetActor->biological.strength = 80;
    targetActor->biological.defense = 2;
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    Color headColor = limb;
    Color limbColor = limb;
    Color bodyColor = body;
    Color beardColor = Colors.gray;
    Color eyeColor   = Colors.black;
    
    float headSize   = (Random.Range(0, 99) * 0.0001f);
    float breastSize = (Random.Range(0, 99) * 0.00054f);
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.37, 0.5, 0.24);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.36, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.3, 0.3, headSize + 0.3);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Beard
    Gene geneBeard;
    geneBeard.offset    = Codon(0, 0, 0);
    geneBeard.position  = Codon(0, 1.2, 0.08f);
    geneBeard.rotation  = Codon(0, 0, 0);
    geneBeard.scale     = Codon(headSize + 0.17, 0.2, 0.1f);
    geneBeard.color.x   = beardColor.r;
    geneBeard.color.y   = beardColor.g;
    geneBeard.color.z   = beardColor.b;
    geneBeard.form             = ActorState::Genetic::Male;
    geneBeard.expressionAge    = 60000;
    geneBeard.expressionFactor = 1.5;
    geneBeard.expressionMax    = 1.4;
    
    //geneBeard.colorIndex         = 1;
    geneBeard.animationType      = ActorState::Animation::Head;
    
    // Eyes
    Gene geneEyeLeft;
    geneEyeLeft.offset    = Codon(0, 0, 0);
    geneEyeLeft.position  = Codon(0.07f, 1.42f, 0.16f);
    geneEyeLeft.rotation  = Codon(0, 0, 0);
    geneEyeLeft.scale     = Codon(0.03f, 0.03f, 0.03f);
    geneEyeLeft.color.x   = eyeColor.r;
    geneEyeLeft.color.y   = eyeColor.g;
    geneEyeLeft.color.z   = eyeColor.b;
    geneEyeLeft.animationType = ActorState::Animation::Head;
    Gene geneEyeRight = geneEyeLeft;
    geneEyeRight.position.x = -geneEyeRight.position.x;
    
    // Neck
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 0, 0);
    geneNeck.position  = Codon(0, 1.2, 0);
    geneNeck.rotation  = Codon(0, 0, 0);
    geneNeck.scale     = Codon(0.18, 0.1, 0.18);
    geneNeck.color.x   = bodyColor.r;
    geneNeck.color.y   = bodyColor.g;
    geneNeck.color.z   = bodyColor.b;
    geneNeck.colorIndex = 1;
    geneNeck.animationType = ActorState::Animation::Head;
    
    // Left breast
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.112, 1.03, breastSize + 0.06f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.14, 0.14, 0.14);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.5;
    geneBreastLeft.expressionMax    = 1.4;
    geneBreastLeft.expressionAge    = 13000;
    geneBreastLeft.animationType = ActorState::Animation::Body;
    
    // Right breast
    Gene geneBreastRight;
    geneBreastRight.offset    = Codon(-0.112, 1.03, breastSize + 0.06f);
    geneBreastRight.position  = Codon(0, 0, 0);
    geneBreastRight.rotation  = Codon(0, 0, 0);
    geneBreastRight.scale     = Codon(0.14, 0.14, 0.14);
    geneBreastRight.color.x   = bodyColor.r;
    geneBreastRight.color.y   = bodyColor.g;
    geneBreastRight.color.z   = bodyColor.b;
    geneBreastRight.form             = ActorState::Genetic::Female;
    geneBreastRight.scaleIndex       = 7;
    geneBreastRight.colorIndex       = 1;
    geneBreastRight.expressionFactor = 1.5;
    geneBreastRight.expressionMax    = 1.4;
    geneBreastRight.expressionAge    = 13000;
    geneBreastRight.animationType = ActorState::Animation::Body;
    
    // Left shoulder
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 1.09, 0);
    geneShoulderLeft.position  = Codon(0.24, -0.0425, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.12, 0.24, 0.12);
    geneShoulderLeft.color.x   = limbColor.r;
    geneShoulderLeft.color.y   = limbColor.g;
    geneShoulderLeft.color.z   = limbColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 13;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.5;
    geneShoulderLeft.expressionMax    = 1.1;
    geneShoulderLeft.expressionAge    = 13000;
    geneShoulderLeft.animationType = ActorState::Animation::Limb;
    
    // Right shoulder
    Gene geneShoulderRight;
    geneShoulderRight.offset    = Codon(0, 1.09, 0);
    geneShoulderRight.position  = Codon(-0.24, -0.0425, 0);
    geneShoulderRight.rotation  = Codon(0, 0, 0);
    geneShoulderRight.scale     = Codon(0.12, 0.24, 0.12);
    geneShoulderRight.color.x   = limbColor.r;
    geneShoulderRight.color.y   = limbColor.g;
    geneShoulderRight.color.z   = limbColor.b;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationAxis      = Codon(1, 0, 0);
    geneShoulderRight.animationRange     = 13;
    geneShoulderRight.colorIndex         = 2;
    geneShoulderRight.form               = ActorState::Genetic::Male;
    geneShoulderRight.expressionFactor = 1.5;
    geneShoulderRight.expressionMax    = 1.1;
    geneShoulderRight.expressionAge    = 13000;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    // Left arm
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.24, 1.09, 0);
    geneLimbFrontLeft.position  = Codon(0, -0.24, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.1, 0.55, 0.1);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 13;
    geneLimbFrontLeft.colorIndex       = 9;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    
    // Right arm
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.24, 1.09, 0);
    geneLimbFrontRight.position  = Codon(0, -0.24, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.1, 0.55, 0.1);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 13;
    geneLimbFrontRight.colorIndex         = 10;
    geneLimbFrontRight.animationType = ActorState::Animation::LimbHolding;
    
    // Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.12, 0.6, 0);
    geneLimbRearLeft.position  = Codon(0, -0.24, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.18, 0.61, 0.18);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doInverseAnimation = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 13;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    
    // Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.12, 0.6, 0);
    geneLimbRearRight.position  = Codon(0, -0.24 , 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.18, 0.61, 0.18);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 13;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneNeck);
    targetActor->genetics.AddGene(geneBeard);
    targetActor->genetics.AddGene(geneEyeLeft);
    targetActor->genetics.AddGene(geneEyeRight);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
}



void GeneticPresets::ActorPresets::Sheep(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.2,0.35});
    
    targetActor->SetName("Sheep");
    
    targetActor->physical.SetAdultAge( 400 );
    targetActor->physical.SetSeniorAge( 4000 );
    
    targetActor->physical.SetSpeed(0.5);
    targetActor->physical.SetSpeedYouth(0.4);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.6f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 30;
    
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
    geneHead.animationType = ActorState::Animation::Head;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.17, 0.75, 0.425);
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
    geneLimbFrontRight.offset    = Codon(-0.17, 0.75, 0.425);
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
    geneLimbRearLeft.offset    = Codon(0.17, 0.75, -0.425);
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
    geneLimbRearRight.offset    = Codon(-0.17, 0.75, -0.425);
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
}


void GeneticPresets::ActorPresets::Boar(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.2,0.35});
    
    targetActor->SetName("Boar");
    
    targetActor->physical.SetAdultAge(600);
    targetActor->physical.SetSeniorAge(6000);
    
    targetActor->physical.SetSpeed(0.45f);
    targetActor->physical.SetSpeedYouth(0.35f);
    targetActor->physical.SetSpeedMultiplier(1.8f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.65f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetDistanceToAttack(3.5f);
    targetActor->behavior.SetDistanceToFlee(12.0f);
    targetActor->behavior.SetDistanceToFocus(16.0f);
    targetActor->behavior.SetDistanceToWalk(10.0f);
    targetActor->behavior.SetDistanceToInflict(0.35f);
    
    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 110.0f;
    targetActor->biological.strength = 10.0f;
    targetActor->biological.defense  = 7.0f;
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else
        {targetActor->physical.SetSexualOrientation(false);}
    
    Color body = Colors.brown * Colors.MakeRandomGrayScale();
    Color limb = Colors.Range(Colors.gray, Colors.dkgray) * Colors.MakeGrayScale(0.08f);
    Color head = body * Colors.MakeGrayScale(0.95f);
    Color tusk = Colors.MakeGrayScale(0.07f);
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.85f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.55f, 0.50f, 1.0f);
    geneBody.color.x   = body.r; geneBody.color.y = body.g; geneBody.color.z = body.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.0f, 0.42f);
    geneHead.position  = Codon(0, 0.0f, 0.25f);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.40f, 0.36f, 0.36f);
    geneHead.color.x   = head.r; geneHead.color.y = head.g; geneHead.color.z = head.b;
    
    // Tusks (attach to head)
    Gene tuskL;
    tuskL.attachmentIndex = 2;
    tuskL.offset   = Codon(0, 0, 0);
    tuskL.position = Codon(0.14f, -0.05f, 0.58f);
    tuskL.rotation = Codon(0.2f, 0, -0.4f);
    tuskL.scale    = Codon(0.22f, 0.08f, 0.08f);
    tuskL.color.x  = tusk.r; tuskL.color.y = tusk.g; tuskL.color.z = tusk.b;
    
    Gene tuskR = tuskL;
    tuskR.position.x = -tuskR.position.x;
    tuskR.rotation.z = -tuskR.rotation.z;
    
    // Legs
    Gene FL;
    FL.offset    = Codon(0.22f, 0.80f, 0.35f);
    FL.position  = Codon(0, -0.42f, 0);
    FL.scale     = Codon(0.18f, 0.62f, 0.18f);
    FL.color.x   = limb.r; FL.color.y = limb.g; FL.color.z = limb.b;
    FL.animationType = ActorState::Animation::Limb;
    FL.animationAxis = Codon(1, 0, 0);
    FL.animationRange = 14;
    
    Gene FR = FL; FR.offset.x = -FR.offset.x; FR.doInverseAnimation = true;
    Gene RL = FL; RL.offset.z = -RL.offset.z;
    Gene RR = FR; RR.offset.z = -RR.offset.z;
    
    // Tail (stubby)
    Gene tail;
    tail.offset   = Codon(0, 0, 0);
    tail.position = Codon(0, 0.86f, -0.55f);
    tail.scale    = Codon(0.08f, 0.18f, 0.12f);
    tail.color.x  = body.r; tail.color.y = body.g; tail.color.z = body.b;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(tuskL);
    targetActor->genetics.AddGene(tuskR);
    targetActor->genetics.AddGene(FL);
    targetActor->genetics.AddGene(FR);
    targetActor->genetics.AddGene(RL);
    targetActor->genetics.AddGene(RR);
    targetActor->genetics.AddGene(tail);
}


void GeneticPresets::ActorPresets::Elk(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.35f, 0.0f, -0.35f}, {0.35f, 1.3f, 0.35f});
    
    targetActor->SetName("Elk");
    
    targetActor->physical.SetAdultAge(1000);
    targetActor->physical.SetSeniorAge(8000);
    
    targetActor->physical.SetSpeed(0.5f);
    targetActor->physical.SetSpeedYouth(0.47f);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.7f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    targetActor->biological.health = 170.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    // Color variants
    Color headColor  = Colors.dkgray;
    Color bodyColor  = Colors.dkgray;
    Color limbColor  = Colors.dkgray;
    Color antlerColor = Colors.MakeGrayScale(0.037f);
    Color udderColor = Colors.Lerp(Colors.MakeGrayScale(0.1f), Colors.red, 0.2f);
    
    headColor  *= Colors.MakeRandomGrayScale();
    bodyColor  *= Colors.MakeRandomGrayScale();
    limbColor  *= Colors.MakeRandomGrayScale();
    udderColor *= Colors.MakeGrayScale(0.3f);
    
    if (Random.Range(0, 100) > 50) headColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) bodyColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) limbColor *= Colors.brown;
    
    // Slight variation in antler tone
    antlerColor *= Colors.MakeGrayScale(0.85f + Random.Range(0.0f, 0.2f));
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.5f, 0.5f, 1.2f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.1f, 0.45f);
    geneHead.position  = Codon(0, 0, 0.35f);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45f, 0.42f, 0.4f);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Left main beam
    Gene geneAntlerLeftMain;
    geneAntlerLeftMain.attachmentIndex = 2;
    geneAntlerLeftMain.offset    = Codon(0, 0, 0);
    geneAntlerLeftMain.position  = Codon(0.2f, 0.43f, 0.1f);
    geneAntlerLeftMain.rotation  = Codon(-0.10f, 0.3f, 1.1f);
    geneAntlerLeftMain.scale     = Codon(0.48f, 0.1f, 0.08f);
    geneAntlerLeftMain.expressionAge = 1000;
    geneAntlerLeftMain.color.x   = antlerColor.r;
    geneAntlerLeftMain.color.y   = antlerColor.g;
    geneAntlerLeftMain.color.z   = antlerColor.b;
    geneAntlerLeftMain.form = ActorState::Genetic::Male;
    geneAntlerLeftMain.expressionFactor = 1.0f;
    
    // Right main beam
    Gene geneAntlerRightMain;
    geneAntlerRightMain.attachmentIndex = 2;
    geneAntlerRightMain.offset    = Codon(0, 0, 0);
    geneAntlerRightMain.position  = Codon(-0.2f, 0.43f, 0.1f);
    geneAntlerRightMain.rotation  = Codon(-0.10f, -0.3f, -1.1f);
    geneAntlerRightMain.scale     = Codon(0.48f, 0.1f, 0.08f);
    geneAntlerRightMain.expressionAge = 1000;
    geneAntlerRightMain.color.x   = antlerColor.r;
    geneAntlerRightMain.color.y   = antlerColor.g;
    geneAntlerRightMain.color.z   = antlerColor.b;
    geneAntlerRightMain.form = ActorState::Genetic::Male;
    geneAntlerRightMain.expressionFactor = 1.0f;
    
    // Left mid tine lower
    Gene geneAntlerLeftMidA;
    geneAntlerLeftMidA.attachmentIndex = 3;
    geneAntlerLeftMidA.offset    = Codon(0, 0, 0);
    geneAntlerLeftMidA.position  = Codon(0.26f, 0.34f, 0.11f);
    geneAntlerLeftMidA.rotation  = Codon(0.05f, 0.0f, -0.55f);
    geneAntlerLeftMidA.scale     = Codon(0.24f, 0.05f, 0.05f);
    geneAntlerLeftMidA.expressionAge = 1700;
    geneAntlerLeftMidA.color.x   = antlerColor.r;
    geneAntlerLeftMidA.color.y   = antlerColor.g;
    geneAntlerLeftMidA.color.z   = antlerColor.b;
    geneAntlerLeftMidA.form = ActorState::Genetic::Male;
    geneAntlerLeftMidA.expressionFactor = 1.0f;
    
    // Right mid tine lower
    Gene geneAntlerRightMidA;
    geneAntlerRightMidA.attachmentIndex = 4;
    geneAntlerRightMidA.offset    = Codon(0, 0, 0);
    geneAntlerRightMidA.position  = Codon(-0.26f, 0.34f, 0.11f);
    geneAntlerRightMidA.rotation  = Codon(0.05f, 0.0f, 0.55f);
    geneAntlerRightMidA.scale     = Codon(0.24f, 0.05f, 0.05f);
    geneAntlerRightMidA.expressionAge = 1700;
    geneAntlerRightMidA.color.x   = antlerColor.r;
    geneAntlerRightMidA.color.y   = antlerColor.g;
    geneAntlerRightMidA.color.z   = antlerColor.b;
    geneAntlerRightMidA.form = ActorState::Genetic::Male;
    geneAntlerRightMidA.expressionFactor = 1.0f;
    
    // Left crown fork lower
    Gene geneAntlerLeftTopA;
    geneAntlerLeftTopA.attachmentIndex = 3;
    geneAntlerLeftTopA.offset    = Codon(0, 0, 0);
    geneAntlerLeftTopA.position  = Codon(0.34f, 0.41f, 0.06f);
    geneAntlerLeftTopA.rotation  = Codon(-0.12f, 0.0f, -0.10f);
    geneAntlerLeftTopA.scale     = Codon(0.20f, 0.064f, 0.045f);
    geneAntlerLeftTopA.expressionAge = 2000;
    geneAntlerLeftTopA.color.x   = antlerColor.r;
    geneAntlerLeftTopA.color.y   = antlerColor.g;
    geneAntlerLeftTopA.color.z   = antlerColor.b;
    geneAntlerLeftTopA.form = ActorState::Genetic::Male;
    geneAntlerLeftTopA.expressionFactor = 1.0f;
    
    // Right crown fork lower
    Gene geneAntlerRightTopA;
    geneAntlerRightTopA.attachmentIndex = 4;
    geneAntlerRightTopA.offset    = Codon(0, 0, 0);
    geneAntlerRightTopA.position  = Codon(-0.34f, 0.41f, 0.06f);
    geneAntlerRightTopA.rotation  = Codon(-0.12f, 0.0f, 0.10f);
    geneAntlerRightTopA.scale     = Codon(0.20f, 0.064f, 0.045f);
    geneAntlerRightTopA.expressionAge = 2000;
    geneAntlerRightTopA.color.x   = antlerColor.r;
    geneAntlerRightTopA.color.y   = antlerColor.g;
    geneAntlerRightTopA.color.z   = antlerColor.b;
    geneAntlerRightTopA.form = ActorState::Genetic::Male;
    geneAntlerRightTopA.expressionFactor = 1.0f;
    
    // Left crown fork upper
    Gene geneAntlerLeftTopB;
    geneAntlerLeftTopB.attachmentIndex = 3;
    geneAntlerLeftTopB.offset    = Codon(0, 0, 0);
    geneAntlerLeftTopB.position  = Codon(0.39f, 0.57f, 0.01f);
    geneAntlerLeftTopB.rotation  = Codon(-0.18f, 0.0f, 0.10f);
    geneAntlerLeftTopB.scale     = Codon(0.17f, 0.06f, 0.04f);
    geneAntlerLeftTopB.expressionAge = 2200;
    geneAntlerLeftTopB.color.x   = antlerColor.r;
    geneAntlerLeftTopB.color.y   = antlerColor.g;
    geneAntlerLeftTopB.color.z   = antlerColor.b;
    geneAntlerLeftTopB.form = ActorState::Genetic::Male;
    geneAntlerLeftTopB.expressionFactor = 1.0f;
    
    // Right crown fork upper
    Gene geneAntlerRightTopB;
    geneAntlerRightTopB.attachmentIndex = 4;
    geneAntlerRightTopB.offset    = Codon(0, 0, 0);
    geneAntlerRightTopB.position  = Codon(-0.39f, 0.57f, 0.01f);
    geneAntlerRightTopB.rotation  = Codon(-0.18f, 0.0f, -0.10f);
    geneAntlerRightTopB.scale     = Codon(0.17f, 0.06f, 0.04f);
    geneAntlerRightTopB.expressionAge = 2200;
    geneAntlerRightTopB.color.x   = antlerColor.r;
    geneAntlerRightTopB.color.y   = antlerColor.g;
    geneAntlerRightTopB.color.z   = antlerColor.b;
    geneAntlerRightTopB.form = ActorState::Genetic::Male;
    geneAntlerRightTopB.expressionFactor = 1.0f;
    
    // Udders
    Gene geneUdders;
    geneUdders.offset    = Codon(0, 0, 0);
    geneUdders.position  = Codon(0, 0.68f, -0.3f);
    geneUdders.rotation  = Codon(0, 0, 0);
    geneUdders.scale     = Codon(0.2f, 0.3f, 0.24f);
    geneUdders.color.x   = udderColor.r;
    geneUdders.color.y   = udderColor.g;
    geneUdders.color.z   = udderColor.b;
    geneUdders.form = ActorState::Genetic::Female;
    geneUdders.expressionFactor = 1.0f;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22f, 0.75f, 0.4f);
    geneLimbFrontLeft.position  = Codon(0, -0.4f, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2f, 0.7f, 0.2f);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis  = Codon(1.2f, 0, 0);
    geneLimbFrontLeft.animationRange = 17;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22f, 0.75f, 0.4f);
    geneLimbFrontRight.position  = Codon(0, -0.4f, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2f, 0.7f, 0.2f);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.animationType = ActorState::Animation::Limb;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1.2f, 0, 0);
    geneLimbFrontRight.animationRange     = 17;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22f, 0.75f, -0.45f);
    geneLimbRearLeft.position  = Codon(0, -0.4f, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2f, 0.7f, 0.2f);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis  = Codon(1.2f, 0, 0);
    geneLimbRearLeft.animationRange = 17;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22f, 0.75f, -0.45f);
    geneLimbRearRight.position  = Codon(0, -0.4f, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2f, 0.7f, 0.2f);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.animationType = ActorState::Animation::Limb;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1.2f, 0, 0);
    geneLimbRearRight.animationRange     = 17;
    
    // Tail
    Gene geneTail;
    geneTail.offset    = Codon(0, 0.85f, -0.60f);
    geneTail.position  = Codon(0, 0, 0);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.1f, 0.52f, 0.1f);
    geneTail.color.x   = bodyColor.r;
    geneTail.color.y   = bodyColor.g;
    geneTail.color.z   = bodyColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);              // 1
    targetActor->genetics.AddGene(geneHead);              // 2
    
    targetActor->genetics.AddGene(geneAntlerLeftMain);    // 3
    targetActor->genetics.AddGene(geneAntlerRightMain);   // 4
    
    targetActor->genetics.AddGene(geneAntlerLeftMidA);    // 7
    targetActor->genetics.AddGene(geneAntlerRightMidA);   // 8
    targetActor->genetics.AddGene(geneAntlerLeftTopA);    // 11
    targetActor->genetics.AddGene(geneAntlerRightTopA);   // 12
    targetActor->genetics.AddGene(geneAntlerLeftTopB);    // 13
    targetActor->genetics.AddGene(geneAntlerRightTopB);   // 14
    
    targetActor->genetics.AddGene(geneUdders);            // 15
    targetActor->genetics.AddGene(geneLimbFrontLeft);     // 16
    targetActor->genetics.AddGene(geneLimbFrontRight);    // 17
    targetActor->genetics.AddGene(geneLimbRearLeft);      // 18
    targetActor->genetics.AddGene(geneLimbRearRight);     // 19
    targetActor->genetics.AddGene(geneTail);              // 20
}


void GeneticPresets::ActorPresets::Spider(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.2,0.0,-0.2}, {0.2,0.5,0.2});
    
    targetActor->SetName("Spider");
    
    targetActor->physical.SetAdultAge(300);
    targetActor->physical.SetSeniorAge(4000);
    
    targetActor->physical.SetSpeed(0.5f);
    targetActor->physical.SetSpeedYouth(0.4f);
    targetActor->physical.SetSpeedMultiplier(2.2f);
    
    targetActor->physical.SetYouthScale(0.087f);
    targetActor->physical.SetAdultScale(0.3f);
    
    targetActor->behavior.SetHeightPreferenceMax(25.0f);
    targetActor->behavior.SetDistanceToAttack(4.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(14.0f);
    targetActor->behavior.SetDistanceToWalk(8.0f);
    targetActor->behavior.SetDistanceToInflict(0.2f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(false);
    
    targetActor->biological.health = 40.0f;
    targetActor->biological.strength = 6.0f;
    targetActor->biological.defense  = 2.0f;
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else
        {targetActor->physical.SetSexualOrientation(false);}
    
    Color headColor = Colors.dkgray * Colors.MakeGrayScale(0.03f);
    Color bodyColor = Colors.black * Colors.MakeGrayScale(0.02f);
    Color limbColor = Colors.dkgray * Colors.MakeGrayScale(0.02f);
    //Color redMark = Colors.red;
    
    if (Random.Range(0, 100) > 70) 
        bodyColor = Colors.brown * 0.03f;
    
    Gene head;
    head.offset   = Codon(0, 0, 0.27f);
    head.position = Codon(0, 0.2f, 0.05f);
    head.scale    = Codon(0.3f, 0.24f, 0.14f);
    head.color.x = headColor.r;
    head.color.y = headColor.g;
    head.color.z = headColor.b;
    head.animationType = ActorState::Animation::Head;
    
    Gene abdomen;
    abdomen.offset   = Codon(0, 0, 0);
    abdomen.position = Codon(0, 0.1f, 0.0f);
    abdomen.scale    = Codon(0.42f, 0.36f, 0.50f);
    abdomen.color.x = bodyColor.r;
    abdomen.color.y = bodyColor.g;
    abdomen.color.z = bodyColor.b;
    abdomen.animationType = ActorState::Animation::Body;
    
    targetActor->genetics.AddGene(head);
    targetActor->genetics.AddGene(abdomen);
    
    float xx = 0.25f;
    float zz = 0.38f;
    float canter = -0.7f;
    bool invLeft  = true;
    bool invRight = false;
    
    for (unsigned int i=0; i < 4; i++) {
        Gene leftLimb  = AI.genomes.build.BuildAddLeg( xx, 0.14f, zz, 0.5f, 0.1f, limbColor, invLeft,  true,  canter, 1.9f, 0.24f);
        Gene rightLimb = AI.genomes.build.BuildAddLeg(-xx, 0.14f, zz, 0.5f, 0.1f, limbColor, invRight, false, canter, 1.9f, 0.24f);
        
        targetActor->genetics.AddGene(leftLimb);
        targetActor->genetics.AddGene(rightLimb);
        
        xx -= 0.05f;
        zz -= 0.12f;
        canter += 0.5f;
        
        invLeft  = !invLeft;
        invRight = !invRight;
    }
}

void GeneticPresets::ActorPresets::Ladybug(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.2,0.0,-0.2}, {0.2,0.5,0.2});
    
    targetActor->SetName("LadyBug");

    targetActor->physical.SetAdultAge(260);
    targetActor->physical.SetSeniorAge(2800);

    targetActor->physical.SetSpeed(0.35f);
    targetActor->physical.SetSpeedYouth(0.30f);
    targetActor->physical.SetSpeedMultiplier(1.6f);

    targetActor->physical.SetYouthScale(0.060f);
    targetActor->physical.SetAdultScale(0.180f);

    targetActor->behavior.SetHeightPreferenceMax(8.0f);
    targetActor->behavior.SetDistanceToAttack(1.5f);
    targetActor->behavior.SetDistanceToFlee(7.0f);
    targetActor->behavior.SetDistanceToFocus(10.0f);
    targetActor->behavior.SetDistanceToWalk(6.0f);
    targetActor->behavior.SetDistanceToInflict(0.15f);

    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);

    targetActor->biological.health   = 16.0f;
    targetActor->biological.strength = 1.2f;
    targetActor->biological.defense  = 1.5f;

    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); } else
        { targetActor->physical.SetSexualOrientation(false); }

    Color headColor = Colors.black;
    Color bodyColor = Colors.red * 0.85f;
    Color limbColor = Colors.black;

    // Small black head
    Gene head;
    head.offset   = Codon(0, 0, 0.25f);
    head.position = Codon(0, 0.22f, 0.12f);
    head.scale    = Codon(0.22f, 0.18f, 0.18f);
    head.color.x  = headColor.r;
    head.color.y  = headColor.g;
    head.color.z  = headColor.b;
    head.animationType = ActorState::Animation::Head;

    // Red body (shell)
    Gene body;
    body.offset   = Codon(0, 0, 0);
    body.position = Codon(0, 0.14f, 0.0f);
    body.scale    = Codon(0.55f, 0.45f, 0.60f);
    body.color.x  = bodyColor.r;
    body.color.y  = bodyColor.g;
    body.color.z  = bodyColor.b;
    body.animationType = ActorState::Animation::Body;

    targetActor->genetics.AddGene(head);
    targetActor->genetics.AddGene(body);

    // 6 legs (3 pairs)
    float  xx     = 0.18f;
    float  zz     = 0.22f;
    float  canter = -0.25f;
    bool   invLeft  = true;
    bool   invRight = false;

    for (unsigned int i = 0; i < 3; i++) {
        Gene leftLimb  = AI.genomes.build.BuildAddLeg( xx, 0.11f, zz, 0.32f, 0.085f, limbColor, invLeft,  true,  canter, 1.5f, 0.18f);
        Gene rightLimb = AI.genomes.build.BuildAddLeg(-xx, 0.11f, zz, 0.32f, 0.085f, limbColor, invRight, false, canter, 1.5f, 0.18f);

        targetActor->genetics.AddGene(leftLimb);
        targetActor->genetics.AddGene(rightLimb);

        xx     -= 0.03f;
        zz     -= 0.10f;
        canter += 0.40f;

        invLeft  = !invLeft;
        invRight = !invRight;
    }
}


void GeneticPresets::ActorPresets::Bovine(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.2,0.35});
    
    targetActor->SetName("Ox");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(5000);
    
    targetActor->physical.SetSpeed(0.5);
    targetActor->physical.SetSpeedYouth(0.47);
    targetActor->physical.SetSpeedMultiplier(1.2f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.7f);
    
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
    geneHornLeft.form = ActorState::Genetic::Male;
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
    geneHornLeftUpper.form = ActorState::Genetic::Male;
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
    geneHornRight.form = ActorState::Genetic::Male;
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
    geneHornRightUpper.form = ActorState::Genetic::Male;
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
    geneUdders.form = ActorState::Genetic::Female;
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
    geneLimbFrontLeft.animationAxis    = Codon(1.2f, 0, 0);
    geneLimbFrontLeft.animationRange   = 17;
    
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
    geneLimbFrontRight.animationAxis      = Codon(1.2, 0, 0);
    geneLimbFrontRight.animationRange     = 17;
    
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
    geneLimbRearLeft.animationAxis    = Codon(1.2, 0, 0);
    geneLimbRearLeft.animationRange   = 17;
    
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
    geneLimbRearRight.animationAxis      = Codon(1.2, 0, 0);
    geneLimbRearRight.animationRange     = 17;
    
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
}



void GeneticPresets::ActorPresets::Horse(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.38,0.0,-0.38}, {0.38,1.4,0.38});
    
    targetActor->SetName("Horse");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(0.5);
    targetActor->physical.SetSpeedYouth(0.4);
    targetActor->physical.SetSpeedMultiplier(2.4f);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(0.7f);
    
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
    
    if (Random.Range(0, 100) > 60) {
        limbFRColor *= Colors.brown;
        limbFLColor *= Colors.brown;
        limbRRColor *= Colors.brown;
        limbRLColor *= Colors.brown;
    }
    
    Color grayScale = Colors.MakeRandomGrayScale();
    limbFRColor *= grayScale;
    limbFLColor *= grayScale;
    limbRRColor *= grayScale;
    limbRLColor *= grayScale;
    
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.black;
        limbFLColor = Colors.black;
        limbRRColor = Colors.black;
        limbRLColor = Colors.black;
    }
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.white;
        limbFLColor = Colors.white;
        limbRRColor = Colors.white;
        limbRLColor = Colors.white;
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
    geneLimbFrontLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbFrontRight.animationAxis      = Codon(0.8, 0, 0);
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
    geneLimbRearLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbRearRight.animationAxis      = Codon(0.8, 0, 0);
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
}



void GeneticPresets::ActorPresets::Bear(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.38,0.0,-0.38}, {0.38,1.4,0.38});
    
    targetActor->SetName("Bear");
    
    targetActor->physical.SetAdultAge(800);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(0.4f);
    targetActor->physical.SetSpeedYouth(0.3f);
    targetActor->physical.SetSpeedMultiplier(3.4);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.7f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(10.0f);
    
    targetActor->behavior.SetDistanceToAttack(14.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToInflict(0.5f);
    targetActor->behavior.SetPredatorState(true);
    
    targetActor->biological.health = 280;
    targetActor->biological.strength = 24;
    
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
    geneLimbFrontLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbFrontRight.animationAxis      = Codon(0.8, 0, 0);
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
    geneLimbRearLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbRearRight.animationAxis      = Codon(0.8, 0, 0);
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
}



void GeneticPresets::ActorPresets::Dog(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,0.7,0.35});
    
    targetActor->SetName("Dog");
    
    targetActor->physical.SetAdultAge(750);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(0.4f);
    targetActor->physical.SetSpeedYouth(0.3f);
    targetActor->physical.SetSpeedMultiplier(2.7f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.5f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(10.0f);
    
    targetActor->behavior.SetDistanceToAttack(8.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToInflict(0.5f);
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
    geneLimbFrontLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbFrontRight.animationAxis      = Codon(0.8, 0, 0);
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
    geneLimbRearLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbRearRight.animationAxis      = Codon(0.8, 0, 0);
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
}



void GeneticPresets::ActorPresets::Raptor(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.2,0.35});
    
    targetActor->SetName("Raptor");
    
    targetActor->physical.SetAdultAge(700);
    targetActor->physical.SetSeniorAge(12000);
    
    targetActor->physical.SetSpeed(0.6f);
    targetActor->physical.SetSpeedYouth(0.45f);
    targetActor->physical.SetSpeedMultiplier(3.0f);
    
    targetActor->physical.SetYouthScale(0.25f);
    targetActor->physical.SetAdultScale(0.80f);
    
    targetActor->behavior.SetHeightPreferenceMin(5.0f);
    targetActor->behavior.SetHeightPreferenceMax(45.0f);
    
    targetActor->behavior.SetDistanceToAttack(8.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToWalk(12.0f);
    targetActor->behavior.SetDistanceToInflict(0.35f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(false);
    
    targetActor->biological.health = 120.0f;
    targetActor->biological.strength = 14.0f;
    targetActor->biological.defense  = 6.0f;
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else
        {targetActor->physical.SetSexualOrientation(false);}
    
    // Colors: dark, slightly brown/gray randomized
    Color base = Colors.dkgray * Colors.MakeRandomGrayScale();
    if (Random.Range(0, 100) > 40) base *= Colors.brown;
    Color limb = base * Colors.MakeGrayScale(0.9f);
    Color head = base * Colors.MakeGrayScale(0.95f);
    
    // Body (lean torso)
    Gene body;
    body.offset   = Codon(0, 0, 0);
    body.position = Codon(0, 1.0f, 0);
    body.rotation = Codon(0, 0, 0);
    body.scale    = Codon(0.42f, 0.48f, 0.95f);
    body.color.x  = base.r; body.color.y = base.g; body.color.z = base.b;
    body.animationType = ActorState::Animation::Body;
    
    // Neck
    Gene neck;
    neck.offset   = Codon(0, 0, 0);
    neck.position = Codon(0, 1.25f, 0.20f);
    neck.rotation = Codon(0.25f, 0, 0);
    neck.scale    = Codon(0.18f, 0.28f, 0.16f);
    neck.color.x  = base.r; neck.color.y = base.g; neck.color.z = base.b;
    neck.animationType = ActorState::Animation::Head;
    
    // Head (attached to neck)
    Gene headU;
    headU.attachmentIndex = 2; // neck (second gene in list)
    headU.offset   = Codon(0, 0, 0);
    headU.position = Codon(0, 1.48f, 0.50f);
    headU.rotation = Codon(0.28f, 0, 0);
    headU.scale    = Codon(0.26f, 0.24f, 0.32f);
    headU.color.x  = head.r; headU.color.y = head.g; headU.color.z = head.b;
    headU.animationType = ActorState::Animation::Head;
    
    // Jaw (attached to neck too for simple open/close)
    Gene jaw;
    jaw.attachmentIndex = 2;
    jaw.offset   = Codon(0, 0, 0);
    jaw.position = Codon(0, 1.40f, 0.46f);
    jaw.rotation = Codon(0.45f, 0, 0);
    jaw.scale    = Codon(0.22f, 0.18f, 0.28f);
    jaw.color.x  = head.r; jaw.color.y = head.g; jaw.color.z = head.b;
    jaw.animationType = ActorState::Animation::Head;
    
    // Arms (short)
    Gene armL;
    armL.offset   = Codon(0.26f, 1.02f, 0.18f);
    armL.position = Codon(0, -0.22f, 0);
    armL.rotation = Codon(0, 0, 0);
    armL.scale    = Codon(0.09f, 0.34f, 0.09f);
    armL.color.x  = limb.r; armL.color.y = limb.g; armL.color.z = limb.b;
    armL.animationType = ActorState::Animation::Limb;
    armL.animationAxis = Codon(1, 0, 0);
    armL.animationRange = 16;
    
    Gene armR = armL;
    armR.offset.x = -armR.offset.x;
    armR.doInverseAnimation = true;
    
    // Legs (powerful)
    Gene legL;
    legL.offset   = Codon(0.14f, 0.62f, -0.06f);
    legL.position = Codon(0, -0.30f, 0);
    legL.rotation = Codon(0, 0, 0);
    legL.scale    = Codon(0.18f, 0.78f, 0.18f);
    legL.color.x  = limb.r; legL.color.y = limb.g; legL.color.z = limb.b;
    legL.animationType = ActorState::Animation::Limb;
    legL.animationAxis = Codon(1, 0, 0);
    legL.animationRange = 18;
    
    Gene legR = legL;
    legR.offset.x = -legR.offset.x;
    legR.doInverseAnimation = true;
    
    // Tail (long for balance)
    Gene tail;
    tail.offset   = Codon(0, 0, 0);
    tail.position = Codon(0, 0.94f, -0.78f);
    tail.rotation = Codon(0.02f, 0, 0);
    tail.scale    = Codon(0.12f, 0.12f, 1.05f);
    tail.color.x  = base.r; tail.color.y = base.g; tail.color.z = base.b;
    tail.animationType = ActorState::Animation::Body;
    
    targetActor->genetics.AddGene(body);
    targetActor->genetics.AddGene(neck);
    targetActor->genetics.AddGene(headU);
    targetActor->genetics.AddGene(jaw);
    targetActor->genetics.AddGene(armL);
    targetActor->genetics.AddGene(armR);
    targetActor->genetics.AddGene(legL);
    targetActor->genetics.AddGene(legR);
    targetActor->genetics.AddGene(tail);
}

void GeneticPresets::ActorPresets::Creepy(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.0,0.35});
    
    targetActor->SetName("Creepy");
    
    targetActor->physical.SetAge(0);
    targetActor->physical.SetAdultAge(0);
    targetActor->physical.SetSeniorAge(1000000);
    
    targetActor->physical.SetSpeed(0.3);
    targetActor->physical.SetSpeedYouth(0.3);
    targetActor->physical.SetSpeedMultiplier(1.0f);
    
    targetActor->physical.SetYouthScale(0.1f);
    targetActor->physical.SetAdultScale(0.5f);
    
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
    geneLimbFrontLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbFrontRight.animationAxis      = Codon(0.8, 0, 0);
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
    geneLimbRearLeft.animationAxis    = Codon(0.8, 0, 0);
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
    geneLimbRearRight.animationAxis      = Codon(0.8, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
}




Gene GeneticPresets::GeneticBuilder::BuildAddLeg(float xx, float yy, float zz, float length, float width, Color color, bool invert, bool side, float canter, float tilt, float offset) {
    Gene gene;
    gene.position = Codon(0, offset, 0);
    zz -= offset;
    
    gene.offset = Codon(-xx, yy, zz);
    gene.rotation.x -= canter * 0.1f;
    
    if (side) {
        gene.rotation.y = -canter;
        gene.rotation.z = tilt;
    } else {
        gene.rotation.y = canter;
        gene.rotation.z = -tilt;
    }
    
    gene.scale   = Codon(width, length, width);
    gene.color.x = color.r;
    gene.color.y = color.g;
    gene.color.z = color.b;
    gene.animationType = ActorState::Animation::Limb;
    gene.animationAxis = Codon(3.5f, 0, 0);
    gene.animationRange = 15.0f;
    gene.doInverseAnimation = invert;
    return gene;
}


void GeneticPresets::ActorPresets::Dwarf(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35f, 0.0f, -0.35f}, {0.35f, 1.35f, 0.35f});
    
    targetActor->SetName("Dwarf");
    
    targetActor->physical.SetAdultAge(25000);
    targetActor->physical.SetSeniorAge(250000);
    
    targetActor->physical.SetSpeed(0.46f);
    targetActor->physical.SetSpeedYouth(0.34f);
    targetActor->physical.SetSpeedMultiplier(1.85f);
    
    targetActor->physical.SetYouthScale(0.09f);
    targetActor->physical.SetAdultScale(0.5f);
    
    targetActor->behavior.SetHeightPreferenceMax(35.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(16.0f);
    targetActor->behavior.SetDistanceToWalk(8.0f);
    targetActor->behavior.SetDistanceToAttack(3.2f);
    targetActor->behavior.SetDistanceToFlee(8.0f);
    targetActor->behavior.SetDistanceToInflict(0.28f);
    targetActor->behavior.SetCooldownAttack(1);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 260.0f;
    targetActor->biological.strength = 95.0f;
    targetActor->biological.defense  = 8.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color skinColor  = Colors.Lerp(Colors.white, Colors.yellow, 0.45f) * 0.5f + (Colors.red * 0.07f);
    Color bodyColor  = Colors.Lerp(Colors.brown, Colors.dkgray, 0.35f) * Colors.MakeGrayScale(0.22f);
    Color beardColor = Colors.Range(Colors.gray, Colors.dkgray) * Colors.MakeGrayScale(0.12f);
    Color eyeColor   = Colors.black * 0.09f + (Colors.red * 0.01f);
    
    if (Random.Range(0, 100) > 60)
        beardColor = Colors.brown * Colors.MakeGrayScale(0.12f);
    
    float headSize   = (Random.Range(0, 99) * 0.00008f);
    float beardSize  = 0.22f + (Random.Range(0, 99) * 0.0008f);
    float breastSize = (Random.Range(0, 99) * 0.00042f);
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.72f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.44f, 0.46f, 0.30f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.08f, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.32f, 0.28f, headSize + 0.32f);
    geneHead.color.x   = skinColor.r;
    geneHead.color.y   = skinColor.g;
    geneHead.color.z   = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Eyes
    Gene geneEyeLeft;
    geneEyeLeft.offset    = Codon(0, 0, 0);
    geneEyeLeft.position  = Codon(0.07f, 1.13f, 0.179f);
    geneEyeLeft.rotation  = Codon(0, 0, 0);
    geneEyeLeft.scale     = Codon(0.03f, 0.03f, 0.03f);
    geneEyeLeft.color.x   = eyeColor.r;
    geneEyeLeft.color.y   = eyeColor.g;
    geneEyeLeft.color.z   = eyeColor.b;
    geneEyeLeft.animationType = ActorState::Animation::Head;
    Gene geneEyeRight = geneEyeLeft;
    geneEyeRight.position.x = -geneEyeRight.position.x;
    
    // Beard
    Gene geneBeard;
    geneBeard.offset    = Codon(0, -5, 0);
    geneBeard.position  = Codon(0, -2.8f, 0.11f);
    geneBeard.rotation  = Codon(0, 0, 0);
    geneBeard.scale     = Codon(0.16f, beardSize, 0.10f);
    geneBeard.color.x   = beardColor.r;
    geneBeard.color.y   = beardColor.g;
    geneBeard.color.z   = beardColor.b;
    geneBeard.form               = ActorState::Genetic::Male;
    geneBeard.expressionAge      = 100;
    geneBeard.expressionFactor   = 1.8f;
    geneBeard.expressionMax      = 1.7f;
    geneBeard.animationType      = ActorState::Animation::Head;
    
    // Breast
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.11f, 0.82f, breastSize + 0.1f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.14f, 0.14f, 0.14f);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.3f;
    geneBreastLeft.expressionMax    = 1.2f;
    geneBreastLeft.expressionAge    = 16000;
    
    Gene geneBreastRight = geneBreastLeft;
    geneBreastRight.offset.x = -geneBreastRight.offset.x;
    geneBreastRight.scaleIndex = 7;
    
    // Shoulders
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 0.88f, 0);
    geneShoulderLeft.position  = Codon(0.28f, -0.03f, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.14f, 0.22f, 0.14f);
    geneShoulderLeft.color.x   = skinColor.r;
    geneShoulderLeft.color.y   = skinColor.g;
    geneShoulderLeft.color.z   = skinColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 12;
    geneShoulderLeft.colorIndex       = 1;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.3f;
    geneShoulderLeft.expressionMax    = 1.2f;
    geneShoulderLeft.expressionAge    = 16000;
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneShoulderRight = geneShoulderLeft;
    geneShoulderRight.position.x = -geneShoulderRight.position.x;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    // Arms
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.28f, 0.88f, 0);
    geneArmLeft.position  = Codon(0, -0.20f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.12f, 0.42f, 0.12f);
    geneArmLeft.color.x   = skinColor.r;
    geneArmLeft.color.y   = skinColor.g;
    geneArmLeft.color.z   = skinColor.b;
    geneArmLeft.animationAxis    = Codon(1, 0, 0);
    geneArmLeft.animationRange   = 12;
    geneArmLeft.colorIndex       = 2;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.animationType = ActorState::Animation::LimbHolding;
    
    // Legs
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.14f, 0.44f, 0);
    geneLegLeft.position  = Codon(0, -0.18f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.20f, 0.50f, 0.20f);
    geneLegLeft.color.x   = skinColor.r;
    geneLegLeft.color.y   = skinColor.g;
    geneLegLeft.color.z   = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1, 0, 0);
    geneLegLeft.animationRange     = 12;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneBeard);
    targetActor->genetics.AddGene(geneEyeRight);
    targetActor->genetics.AddGene(geneEyeLeft);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
    
}


void GeneticPresets::ActorPresets::Elf(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.30f, 0.0f, -0.30f}, {0.30f, 1.95f, 0.30f});
    
    targetActor->SetName("Elf");
    
    targetActor->physical.SetAdultAge(38000);
    targetActor->physical.SetSeniorAge(260000);
    
    targetActor->physical.SetSpeed(0.68f);
    targetActor->physical.SetSpeedYouth(0.48f);
    targetActor->physical.SetSpeedMultiplier(2.5f);
    
    targetActor->physical.SetYouthScale(0.10f);
    targetActor->physical.SetAdultScale(0.78f);
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToWalk(12.0f);
    targetActor->behavior.SetDistanceToAttack(4.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToInflict(0.28f);
    targetActor->behavior.SetCooldownAttack(1);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 180.0f;
    targetActor->biological.strength = 72.0f;
    targetActor->biological.defense  = 3.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color skinColor  = Colors.Lerp(Colors.white, Colors.yellow, 0.35f) * 0.55f + (Colors.green * 0.01f);
    Color bodyColor  = Colors.Lerp(Colors.green, Colors.dkgray, 0.35f) * Colors.MakeGrayScale(0.18f);
    Color hairColor  = Colors.Range(Colors.gray, Colors.white) * Colors.MakeGrayScale(0.16f);
    
    if (Random.Range(0, 100) > 70)
        hairColor = Colors.brown * Colors.MakeGrayScale(0.12f);
    
    float headSize   = (Random.Range(0, 99) * 0.00006f);
    float breastSize = (Random.Range(0, 99) * 0.0003f);
    
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.96f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.33f, 0.58f, 0.22f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.46f, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.28f, 0.28f, headSize + 0.28f);
    geneHead.color.x   = skinColor.r;
    geneHead.color.y   = skinColor.g;
    geneHead.color.z   = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 0, 0);
    geneNeck.position  = Codon(0, 1.25f, 0);
    geneNeck.rotation  = Codon(0, 0, 0);
    geneNeck.scale     = Codon(0.14f, 0.12f, 0.14f);
    geneNeck.color.x   = bodyColor.r;
    geneNeck.color.y   = bodyColor.g;
    geneNeck.color.z   = bodyColor.b;
    geneNeck.colorIndex = 1;
    geneNeck.animationType = ActorState::Animation::Head;
    
    // Hair
    Gene geneHair;
    geneHair.offset    = Codon(0, 0, -0.05f);
    geneHair.position  = Codon(0, 1.43f, 0);
    geneHair.rotation  = Codon(0, 0, 0);
    geneHair.scale     = Codon(0.30f, 0.40f, 0.18f);
    geneHair.color.x   = hairColor.r;
    geneHair.color.y   = hairColor.g;
    geneHair.color.z   = hairColor.b;
    geneHair.animationType = ActorState::Animation::Head;
    
    // Ears
    Gene geneEarLeft;
    geneEarLeft.offset    = Codon(0, 0, 0);
    geneEarLeft.position  = Codon(0.24f, 1.48f, 0.0f);
    geneEarLeft.rotation  = Codon(0, 0, -0.25f);
    geneEarLeft.scale     = Codon(0.08f, 0.22f, 0.05f);
    geneEarLeft.color.x   = skinColor.r;
    geneEarLeft.color.y   = skinColor.g;
    geneEarLeft.color.z   = skinColor.b;
    geneEarLeft.animationType = ActorState::Animation::Head;
    
    Gene geneEarRight = geneEarLeft;
    geneEarRight.position.x = -geneEarRight.position.x;
    geneEarRight.rotation.z = -geneEarRight.rotation.z;
    
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.10f, 1.06f, breastSize + 0.055f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.12f, 0.12f, 0.12f);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.3f;
    geneBreastLeft.expressionMax    = 1.2f;
    geneBreastLeft.expressionAge    = 22000;
    
    Gene geneBreastRight = geneBreastLeft;
    geneBreastRight.offset.x = -geneBreastRight.offset.x;
    geneBreastRight.scaleIndex = 5;
    
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 1.12f, 0);
    geneShoulderLeft.position  = Codon(0.23f, -0.03f, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.10f, 0.20f, 0.10f);
    geneShoulderLeft.color.x   = skinColor.r;
    geneShoulderLeft.color.y   = skinColor.g;
    geneShoulderLeft.color.z   = skinColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 14;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.2f;
    geneShoulderLeft.expressionMax    = 1.05f;
    geneShoulderLeft.expressionAge    = 22000;
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneShoulderRight = geneShoulderLeft;
    geneShoulderRight.position.x = -geneShoulderRight.position.x;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.23f, 1.12f, 0);
    geneArmLeft.position  = Codon(0, -0.28f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.08f, 0.62f, 0.08f);
    geneArmLeft.color.x   = skinColor.r;
    geneArmLeft.color.y   = skinColor.g;
    geneArmLeft.color.z   = skinColor.b;
    geneArmLeft.animationAxis    = Codon(1, 0, 0);
    geneArmLeft.animationRange   = 14;
    geneArmLeft.colorIndex       = 7;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.colorIndex = 8;
    geneArmRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.11f, 0.62f, 0);
    geneLegLeft.position  = Codon(0, -0.30f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.14f, 0.78f, 0.14f);
    geneLegLeft.color.x   = skinColor.r;
    geneLegLeft.color.y   = skinColor.g;
    geneLegLeft.color.z   = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1, 0, 0);
    geneLegLeft.animationRange     = 14;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneNeck);
    targetActor->genetics.AddGene(geneHair);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
}


void GeneticPresets::ActorPresets::Goblin(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.26f, 0.0f, -0.26f}, {0.26f, 1.15f, 0.26f});
    
    targetActor->SetName("Goblin");
    
    targetActor->physical.SetAdultAge(2000);
    targetActor->physical.SetSeniorAge(14000);
    
    targetActor->physical.SetSpeed(0.62f);
    targetActor->physical.SetSpeedYouth(0.44f);
    targetActor->physical.SetSpeedMultiplier(2.3f);
    
    targetActor->physical.SetYouthScale(0.08f);
    targetActor->physical.SetAdultScale(0.48f);
    
    targetActor->behavior.SetHeightPreferenceMax(25.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(16.0f);
    targetActor->behavior.SetDistanceToWalk(10.0f);
    targetActor->behavior.SetDistanceToAttack(3.5f);
    targetActor->behavior.SetDistanceToFlee(11.0f);
    targetActor->behavior.SetDistanceToInflict(0.22f);
    targetActor->behavior.SetCooldownAttack(1);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 95.0f;
    targetActor->biological.strength = 36.0f;
    targetActor->biological.defense  = 2.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color skinColor = Colors.Lerp(Colors.green, Colors.yellow, 0.2f) * Colors.MakeGrayScale(0.25f);
    Color bodyColor = Colors.Lerp(Colors.brown, Colors.dkgray, 0.3f) * Colors.MakeGrayScale(0.16f);
    Color noseColor = Colors.Lerp(skinColor, Colors.black, 0.2f);
    
    float headSize   = (Random.Range(0, 99) * 0.00009f);
    float breastSize = (Random.Range(0, 99) * 0.00040f);
    
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.64f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.30f, 0.38f, 0.20f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 0.99f, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.32f, 0.26f, headSize + 0.28f);
    geneHead.color.x   = skinColor.r;
    geneHead.color.y   = skinColor.g;
    geneHead.color.z   = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    Gene geneNose;
    geneNose.offset    = Codon(0, 0, 0.14f);
    geneNose.position  = Codon(0, 0.96f, 0);
    geneNose.rotation  = Codon(0.1f, 0, 0);
    geneNose.scale     = Codon(0.09f, 0.16f, 0.08f);
    geneNose.color.x   = noseColor.r;
    geneNose.color.y   = noseColor.g;
    geneNose.color.z   = noseColor.b;
    geneNose.animationType = ActorState::Animation::Head;
    
    Gene geneEarLeft;
    geneEarLeft.offset    = Codon(0, 0, 0);
    geneEarLeft.position  = Codon(0.24f, 1.01f, 0.0f);
    geneEarLeft.rotation  = Codon(0, 0, -0.55f);
    geneEarLeft.scale     = Codon(0.07f, 0.25f, 0.04f);
    geneEarLeft.color.x   = skinColor.r;
    geneEarLeft.color.y   = skinColor.g;
    geneEarLeft.color.z   = skinColor.b;
    geneEarLeft.animationType = ActorState::Animation::Head;
    
    Gene geneEarRight = geneEarLeft;
    geneEarRight.position.x = -geneEarRight.position.x;
    geneEarRight.rotation.z = -geneEarRight.rotation.z;
    
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.09f, 0.74f, breastSize + 0.04f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.10f, 0.10f, 0.10f);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.2f;
    geneBreastLeft.expressionMax    = 1.1f;
    geneBreastLeft.expressionAge    = 1400;
    
    Gene geneBreastRight = geneBreastLeft;
    geneBreastRight.offset.x = -geneBreastRight.offset.x;
    geneBreastRight.scaleIndex = 5;
    
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 0.82f, 0);
    geneShoulderLeft.position  = Codon(0.18f, -0.03f, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.09f, 0.17f, 0.09f);
    geneShoulderLeft.color.x   = skinColor.r;
    geneShoulderLeft.color.y   = skinColor.g;
    geneShoulderLeft.color.z   = skinColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 15;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.3f;
    geneShoulderLeft.expressionMax    = 1.12f;
    geneShoulderLeft.expressionAge    = 1400;
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneShoulderRight = geneShoulderLeft;
    geneShoulderRight.position.x = -geneShoulderRight.position.x;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.18f, 0.82f, 0);
    geneArmLeft.position  = Codon(0, -0.20f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.08f, 0.42f, 0.08f);
    geneArmLeft.color.x   = skinColor.r;
    geneArmLeft.color.y   = skinColor.g;
    geneArmLeft.color.z   = skinColor.b;
    geneArmLeft.animationAxis    = Codon(1, 0, 0);
    geneArmLeft.animationRange   = 15;
    geneArmLeft.colorIndex       = 7;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.colorIndex = 8;
    geneArmRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.10f, 0.40f, 0);
    geneLegLeft.position  = Codon(0, -0.18f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.13f, 0.48f, 0.13f);
    geneLegLeft.color.x   = skinColor.r;
    geneLegLeft.color.y   = skinColor.g;
    geneLegLeft.color.z   = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1, 0, 0);
    geneLegLeft.animationRange     = 15;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneNose);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
}


void GeneticPresets::ActorPresets::Orc(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.40f, 0.0f, -0.40f}, {0.40f, 1.90f, 0.40f});
    
    targetActor->SetName("Orc");
    
    targetActor->physical.SetAdultAge(4500);
    targetActor->physical.SetSeniorAge(24000);
    
    targetActor->physical.SetSpeed(0.56f);
    targetActor->physical.SetSpeedYouth(0.40f);
    targetActor->physical.SetSpeedMultiplier(2.2f);
    
    targetActor->physical.SetYouthScale(0.12f);
    targetActor->physical.SetAdultScale(0.88f);
    
    targetActor->behavior.SetHeightPreferenceMax(35.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(18.0f);
    targetActor->behavior.SetDistanceToWalk(10.0f);
    targetActor->behavior.SetDistanceToAttack(4.0f);
    targetActor->behavior.SetDistanceToFlee(8.0f);
    targetActor->behavior.SetDistanceToInflict(0.34f);
    targetActor->behavior.SetCooldownAttack(1);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 320.0f;
    targetActor->biological.strength = 120.0f;
    targetActor->biological.defense  = 6.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color skinColor = Colors.Lerp(Colors.green, Colors.brown, 0.35f) * Colors.MakeGrayScale(0.22f);
    Color bodyColor = Colors.Lerp(Colors.brown, Colors.black, 0.42f) * Colors.MakeGrayScale(0.18f);
    Color tuskColor = Colors.MakeGrayScale(0.10f);
    
    float headSize   = (Random.Range(0, 99) * 0.00008f);
    float breastSize = (Random.Range(0, 99) * 0.00045f);
    
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.98f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.46f, 0.62f, 0.30f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.47f, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.34f, 0.31f, headSize + 0.31f);
    geneHead.color.x   = skinColor.r;
    geneHead.color.y   = skinColor.g;
    geneHead.color.z   = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    Gene geneJaw;
    geneJaw.offset    = Codon(0, -0.08f, 0.11f);
    geneJaw.position  = Codon(0, 1.33f, 0);
    geneJaw.rotation  = Codon(0.08f, 0, 0);
    geneJaw.scale     = Codon(0.24f, 0.14f, 0.18f);
    geneJaw.color.x   = skinColor.r;
    geneJaw.color.y   = skinColor.g;
    geneJaw.color.z   = skinColor.b;
    geneJaw.animationType = ActorState::Animation::Head;
    
    Gene tuskLeft;
    tuskLeft.offset    = Codon(0.12f, -0.06f, 0.18f);
    tuskLeft.position  = Codon(0, 1.30f, 0);
    tuskLeft.rotation  = Codon(0.2f, 0, -0.2f);
    tuskLeft.scale     = Codon(0.05f, 0.14f, 0.05f);
    tuskLeft.color.x   = tuskColor.r;
    tuskLeft.color.y   = tuskColor.g;
    tuskLeft.color.z   = tuskColor.b;
    tuskLeft.animationType = ActorState::Animation::Head;
    
    Gene tuskRight = tuskLeft;
    tuskRight.offset.x = -tuskRight.offset.x;
    tuskRight.rotation.z = -tuskRight.rotation.z;
    
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.12f, 1.07f, breastSize + 0.06f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.14f, 0.14f, 0.14f);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.2f;
    geneBreastLeft.expressionMax    = 1.15f;
    geneBreastLeft.expressionAge    = 2500;
    
    Gene geneBreastRight = geneBreastLeft;
    geneBreastRight.offset.x = -geneBreastRight.offset.x;
    geneBreastRight.scaleIndex = 5;
    
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 1.15f, 0);
    geneShoulderLeft.position  = Codon(0.30f, -0.03f, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.15f, 0.24f, 0.15f);
    geneShoulderLeft.color.x   = skinColor.r;
    geneShoulderLeft.color.y   = skinColor.g;
    geneShoulderLeft.color.z   = skinColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 12;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.4f;
    geneShoulderLeft.expressionMax    = 1.25f;
    geneShoulderLeft.expressionAge    = 2500;
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneShoulderRight = geneShoulderLeft;
    geneShoulderRight.position.x = -geneShoulderRight.position.x;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.30f, 1.15f, 0);
    geneArmLeft.position  = Codon(0, -0.26f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.11f, 0.62f, 0.11f);
    geneArmLeft.color.x   = skinColor.r;
    geneArmLeft.color.y   = skinColor.g;
    geneArmLeft.color.z   = skinColor.b;
    geneArmLeft.animationAxis    = Codon(1, 0, 0);
    geneArmLeft.animationRange   = 12;
    geneArmLeft.colorIndex       = 7;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.colorIndex = 8;
    geneArmRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.13f, 0.63f, 0);
    geneLegLeft.position  = Codon(0, -0.26f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.18f, 0.76f, 0.18f);
    geneLegLeft.color.x   = skinColor.r;
    geneLegLeft.color.y   = skinColor.g;
    geneLegLeft.color.z   = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1, 0, 0);
    geneLegLeft.animationRange     = 12;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneJaw);
    targetActor->genetics.AddGene(tuskLeft);
    targetActor->genetics.AddGene(tuskRight);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
}


void GeneticPresets::ActorPresets::Halfling(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.28f, 0.0f, -0.28f}, {0.28f, 1.10f, 0.28f});
    
    targetActor->SetName("Halfling");
    
    targetActor->physical.SetAdultAge(12000);
    targetActor->physical.SetSeniorAge(80000);
    
    targetActor->physical.SetSpeed(0.52f);
    targetActor->physical.SetSpeedYouth(0.40f);
    targetActor->physical.SetSpeedMultiplier(1.95f);
    
    targetActor->physical.SetYouthScale(0.09f);
    targetActor->physical.SetAdultScale(0.48f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(14.0f);
    targetActor->behavior.SetDistanceToWalk(8.0f);
    targetActor->behavior.SetDistanceToAttack(2.6f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToInflict(0.22f);
    targetActor->behavior.SetCooldownAttack(1);
    
    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 140.0f;
    targetActor->biological.strength = 42.0f;
    targetActor->biological.defense  = 2.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color skinColor  = Colors.Lerp(Colors.white, Colors.yellow, 0.5f) * 0.5f + (Colors.red * 0.08f);
    Color bodyColor  = Colors.Lerp(Colors.brown, Colors.green, 0.15f) * Colors.MakeGrayScale(0.20f);
    Color hairColor  = Colors.Range(Colors.brown, Colors.dkgray) * Colors.MakeGrayScale(0.12f);
    
    float headSize   = (Random.Range(0, 99) * 0.00008f);
    float breastSize = (Random.Range(0, 99) * 0.00045f);
    
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.62f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.30f, 0.40f, 0.22f);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 0.98f, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(headSize + 0.31f, 0.29f, headSize + 0.31f);
    geneHead.color.x   = skinColor.r;
    geneHead.color.y   = skinColor.g;
    geneHead.color.z   = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    Gene geneHair;
    geneHair.offset    = Codon(0, 0, -0.04f);
    geneHair.position  = Codon(0, 0.96f, 0);
    geneHair.rotation  = Codon(0, 0, 0);
    geneHair.scale     = Codon(0.28f, 0.26f, 0.16f);
    geneHair.color.x   = hairColor.r;
    geneHair.color.y   = hairColor.g;
    geneHair.color.z   = hairColor.b;
    geneHair.animationType = ActorState::Animation::Head;
    
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 0, 0);
    geneNeck.position  = Codon(0, 0.84f, 0);
    geneNeck.rotation  = Codon(0, 0, 0);
    geneNeck.scale     = Codon(0.15f, 0.10f, 0.15f);
    geneNeck.color.x   = bodyColor.r;
    geneNeck.color.y   = bodyColor.g;
    geneNeck.color.z   = bodyColor.b;
    geneNeck.colorIndex = 1;
    geneNeck.animationType = ActorState::Animation::Head;
    
    Gene geneBreastLeft;
    geneBreastLeft.offset    = Codon(0.09f, 0.73f, breastSize + 0.045f);
    geneBreastLeft.position  = Codon(0, 0, 0);
    geneBreastLeft.rotation  = Codon(0, 0, 0);
    geneBreastLeft.scale     = Codon(0.10f, 0.10f, 0.10f);
    geneBreastLeft.color.x   = bodyColor.r;
    geneBreastLeft.color.y   = bodyColor.g;
    geneBreastLeft.color.z   = bodyColor.b;
    geneBreastLeft.form             = ActorState::Genetic::Female;
    geneBreastLeft.colorIndex       = 1;
    geneBreastLeft.expressionFactor = 1.25f;
    geneBreastLeft.expressionMax    = 1.10f;
    geneBreastLeft.expressionAge    = 9000;
    
    Gene geneBreastRight = geneBreastLeft;
    geneBreastRight.offset.x = -geneBreastRight.offset.x;
    geneBreastRight.scaleIndex = 5;
    
    Gene geneShoulderLeft;
    geneShoulderLeft.offset    = Codon(0, 0.77f, 0);
    geneShoulderLeft.position  = Codon(0.18f, -0.03f, 0);
    geneShoulderLeft.rotation  = Codon(0, 0, 0);
    geneShoulderLeft.scale     = Codon(0.09f, 0.16f, 0.09f);
    geneShoulderLeft.color.x   = skinColor.r;
    geneShoulderLeft.color.y   = skinColor.g;
    geneShoulderLeft.color.z   = skinColor.b;
    geneShoulderLeft.animationAxis    = Codon(1, 0, 0);
    geneShoulderLeft.animationRange   = 13;
    geneShoulderLeft.colorIndex       = 2;
    geneShoulderLeft.form             = ActorState::Genetic::Male;
    geneShoulderLeft.expressionFactor = 1.2f;
    geneShoulderLeft.expressionMax    = 1.08f;
    geneShoulderLeft.expressionAge    = 9000;
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneShoulderRight = geneShoulderLeft;
    geneShoulderRight.position.x = -geneShoulderRight.position.x;
    geneShoulderRight.doInverseAnimation = true;
    geneShoulderRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.18f, 0.77f, 0);
    geneArmLeft.position  = Codon(0, -0.18f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.08f, 0.38f, 0.08f);
    geneArmLeft.color.x   = skinColor.r;
    geneArmLeft.color.y   = skinColor.g;
    geneArmLeft.color.z   = skinColor.b;
    geneArmLeft.animationAxis    = Codon(1, 0, 0);
    geneArmLeft.animationRange   = 13;
    geneArmLeft.colorIndex       = 7;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.colorIndex = 8;
    geneArmRight.animationType = ActorState::Animation::LimbHolding;
    
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.10f, 0.36f, 0);
    geneLegLeft.position  = Codon(0, -0.16f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.14f, 0.42f, 0.14f);
    geneLegLeft.color.x   = skinColor.r;
    geneLegLeft.color.y   = skinColor.g;
    geneLegLeft.color.z   = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1, 0, 0);
    geneLegLeft.animationRange     = 13;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneHair);
    targetActor->genetics.AddGene(geneNeck);
    targetActor->genetics.AddGene(geneBreastLeft);
    targetActor->genetics.AddGene(geneBreastRight);
    targetActor->genetics.AddGene(geneShoulderLeft);
    targetActor->genetics.AddGene(geneShoulderRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
}

void GeneticPresets::ActorPresets::Camel(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    // Camel configuration matching Elk/Boar pattern
    targetActor->SetBoundingBox({-0.45f, 0.0f, -0.65f}, {0.45f, 2.2f, 0.65f});
    targetActor->SetName("Camel");
    
    targetActor->physical.SetAdultAge(700);
    targetActor->physical.SetSeniorAge(5500);
    
    targetActor->physical.SetSpeed(0.55f);
    targetActor->physical.SetSpeedYouth(0.38f);
    targetActor->physical.SetSpeedMultiplier(1.6f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.85f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 150.0f;
    targetActor->biological.strength = 25.0f;
    targetActor->biological.defense = 4.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color bodyColor = Colors.brown * Colors.Lerp(Colors.white, Colors.yellow, 0.4f) * 0.7f;
    Color headColor = bodyColor * Colors.MakeGrayScale(0.95f);
    Color limbColor = bodyColor * Colors.MakeGrayScale(0.85f);
    
    Color variation = Colors.MakeRandomGrayScale();
    bodyColor *= variation; headColor *= variation; limbColor *= variation;
    
    // Body Gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 1.1f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.52f, 0.55f, 1.35f);
    geneBody.color.x   = bodyColor.r; geneBody.color.y = bodyColor.g; geneBody.color.z = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    targetActor->genetics.AddGene(geneBody);
    
    // Long Neck Gene
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 1.3f, 0.55f);
    geneNeck.position  = Codon(0, 0.3f, 0.2f);
    geneNeck.rotation  = Codon(0.45f, 0, 0);
    geneNeck.scale     = Codon(0.24f, 0.65f, 0.26f);
    geneNeck.color.x   = bodyColor.r; geneNeck.color.y = bodyColor.g; geneNeck.color.z = bodyColor.b;
    geneNeck.animationType = ActorState::Animation::Head; 
    targetActor->genetics.AddGene(geneNeck);
    
    // Head Gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.85f, 0.85f);
    geneHead.position  = Codon(0, 0, 0.25f);
    geneHead.rotation  = Codon(0.2f, 0, 0);
    geneHead.scale     = Codon(0.28f, 0.28f, 0.48f);
    geneHead.color.x   = headColor.r; geneHead.color.y = headColor.g; geneHead.color.z = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    targetActor->genetics.AddGene(geneHead);
    
    // Hump
    if (Random.Range(0, 100) > 50) {
        // Dromedary: Single hump
        Gene singleHump;
        singleHump.offset   = Codon(0, 1.38f, 0.0f); 
        singleHump.position = Codon(0, 0, 0);
        singleHump.scale    = Codon(0.45f, 0.42f, 0.6f);
        singleHump.color.x  = bodyColor.r; singleHump.color.y = bodyColor.g; singleHump.color.z = bodyColor.b;
        targetActor->genetics.AddGene(singleHump);
    } else {
        // Bactrian: Two humps
        Gene frontHump;
        frontHump.offset   = Codon(0, 1.35f, 0.25f);
        frontHump.position = Codon(0, 0, 0);
        frontHump.scale    = Codon(0.42f, 0.36f, 0.38f);
        frontHump.color.x  = bodyColor.r; frontHump.color.y = bodyColor.g; frontHump.color.z = bodyColor.b;
        targetActor->genetics.AddGene(frontHump);
        
        Gene rearHump = frontHump;
        rearHump.offset.z = -0.25f;
        targetActor->genetics.AddGene(rearHump);
    }
    
    // Long Legs
    Gene FL;
    FL.offset    = Codon(0.2f, 1.0f, 0.45f);
    FL.position  = Codon(0, -0.5f, 0);
    FL.scale     = Codon(0.15f, 1.0f, 0.15f);
    FL.color.x   = limbColor.r; FL.color.y = limbColor.g; FL.color.z = limbColor.b;
    FL.animationType = ActorState::Animation::Limb;
    FL.animationAxis = Codon(1, 0, 0);
    FL.animationRange = 16;
    
    Gene FR = FL; FR.offset.x = -FR.offset.x; FR.doInverseAnimation = true;
    Gene RL = FL; RL.offset.z = -RL.offset.z;
    Gene RR = FR; RR.offset.z = -RR.offset.z;
    
    targetActor->genetics.AddGene(FL);
    targetActor->genetics.AddGene(FR);
    targetActor->genetics.AddGene(RL);
    targetActor->genetics.AddGene(RR);
    
    // Tail
    Gene tail;
    tail.offset   = Codon(0, 1.1f, -0.65f);
    tail.position = Codon(0, -0.25f, -0.05f);
    tail.rotation = Codon(0.2f, 0, 0);
    tail.scale    = Codon(0.08f, 0.45f, 0.08f);
    tail.color.x  = bodyColor.r; tail.color.y = bodyColor.g; tail.color.z = bodyColor.b;
    targetActor->genetics.AddGene(tail);
}

void GeneticPresets::ActorPresets::Cat(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.2f, 0.0f, -0.2f}, {0.2f, 0.5f, 0.2f});
    
    targetActor->SetName("Cat");
    
    targetActor->physical.SetAdultAge(400);
    targetActor->physical.SetSeniorAge(6000);
    
    targetActor->physical.SetSpeed(0.5f);
    targetActor->physical.SetSpeedYouth(0.4f);
    targetActor->physical.SetSpeedMultiplier(2.8f);
    
    targetActor->physical.SetYouthScale(0.15f);
    targetActor->physical.SetAdultScale(0.35f);
    
    targetActor->behavior.SetHeightPreferenceMax(40.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToAttack(5.0f);
    targetActor->behavior.SetDistanceToFlee(12.0f);
    targetActor->behavior.SetDistanceToFocus(15.0f);
    targetActor->behavior.SetDistanceToInflict(0.3f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 35.0f;
    targetActor->biological.strength = 4.0f;
    targetActor->biological.defense = 1.0f;
    
    if (Random.Range(0, 100) > 55) 
        {targetActor->physical.SetSexualOrientation(true);} else 
        {targetActor->physical.SetSexualOrientation(false);}     
    
    // Color variants (Orange, Black, Gray, White)
    Color baseColor = Colors.gray;
    int colorRoll = Random.Range(0, 100);
    
    if (colorRoll > 75) {
        baseColor = Colors.brown * 1.5f; // Orange/Tabby
    } else if (colorRoll > 50) {
        baseColor = Colors.black * 0.8f; // Black cat
    } else if (colorRoll > 25) {
        baseColor = Colors.white * 0.9f; // White cat
    } else {
        baseColor = Colors.gray * Colors.MakeRandomGrayScale(); // Gray cat
    }
    
    Color eyeColor = Colors.green * 1.2f;
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, -0.05f);
    geneBody.position  = Codon(0, 0.25f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.17f, 0.18f, 0.38f);
    geneBody.color.x   = baseColor.r; geneBody.color.y = baseColor.g; geneBody.color.z = baseColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0.0f, 0, 0.19f);
    geneHead.position  = Codon(0, 0.32f, 0.08f);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.18f, 0.16f, 0.18f);
    geneHead.color.x   = baseColor.r; geneHead.color.y = baseColor.g; geneHead.color.z = baseColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.offset    = Codon(0, 0, 0.19f);
    geneEarLeft.position  = Codon(0.08f, 0.42f, 0.05f);
    geneEarLeft.rotation  = Codon(0.24f, 0, 0);
    geneEarLeft.scale     = Codon(0.05f, 0.08f, 0.03f);
    geneEarLeft.color.x   = baseColor.r; geneEarLeft.color.y = baseColor.g; geneEarLeft.color.z = baseColor.b;
    geneEarLeft.animationType = ActorState::Animation::Head;
    
    // Ear right gene
    Gene geneEarRight = geneEarLeft;
    geneEarRight.position.x = -geneEarRight.position.x;
    geneEarRight.rotation.z = -geneEarRight.rotation.z;
    
    // Tail gene
    Gene geneTail;
    geneTail.offset    = Codon(0.0f, 0.0f, 0.0f);
    geneTail.position  = Codon(0.0f, 0.28f, -0.30f);
    geneTail.rotation  = Codon(-0.5f, 0, 0); // Pointing slightly up
    geneTail.scale     = Codon(0.04f, 0.04f, 0.28f);
    geneTail.color.x   = baseColor.r; geneTail.color.y = baseColor.g; geneTail.color.z = baseColor.b;
    
    // Limbs
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.074f, 0.24f, 0.09f);
    geneLimbFrontLeft.position  = Codon(0.0f, -0.12f, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.06f, 0.24f, 0.06f);
    geneLimbFrontLeft.color.x   = baseColor.r; geneLimbFrontLeft.color.y = baseColor.g; geneLimbFrontLeft.color.z = baseColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis = Codon(1.8f, 0, 0);
    geneLimbFrontLeft.animationRange = 18;
    
    Gene geneLimbFrontRight = geneLimbFrontLeft;
    geneLimbFrontRight.offset.x = -geneLimbFrontRight.offset.x;
    geneLimbFrontRight.doInverseAnimation = true;
    
    Gene geneLimbRearLeft = geneLimbFrontLeft;
    geneLimbRearLeft.offset.z = -0.18f;
    
    Gene geneLimbRearRight = geneLimbFrontRight;
    geneLimbRearRight.offset.z = -0.18f;
    
    // Apply genes
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneTail);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
}

void GeneticPresets::ActorPresets::Troll(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.5f, 0.0f, -0.5f}, {0.5f, 2.6f, 0.5f});
    
    targetActor->SetName("Troll");
    
    targetActor->physical.SetAdultAge(10000);
    targetActor->physical.SetSeniorAge(100000);
    
    targetActor->physical.SetSpeed(0.35f); // Slow walker
    targetActor->physical.SetSpeedYouth(0.30f);
    targetActor->physical.SetSpeedMultiplier(2.5f); // Fast charger
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(1.1f); // Massive
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(25.0f);
    targetActor->behavior.SetDistanceToWalk(12.0f);
    targetActor->behavior.SetDistanceToAttack(4.5f);
    targetActor->behavior.SetDistanceToFlee(5.0f); // Rarely flees
    targetActor->behavior.SetDistanceToInflict(0.6f);
    targetActor->behavior.SetCooldownAttack(2);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(false);
    
    targetActor->biological.health   = 550.0f;
    targetActor->biological.strength = 180.0f;
    targetActor->biological.defense  = 15.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); } else
        { targetActor->physical.SetSexualOrientation(false); }
    
    // Muddy, swampy colors
    Color skinColor = Colors.Lerp(Colors.green, Colors.dkgray, 0.6f) * Colors.MakeGrayScale(0.4f);
    Color tuskColor = Colors.MakeGrayScale(0.8f);
    
    // Large, hunched torso
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 1.3f, 0);
    geneBody.rotation  = Codon(0.2f, 0, 0); // Hunched forward
    geneBody.scale     = Codon(0.65f, 0.75f, 0.55f);
    geneBody.color.x   = skinColor.r; geneBody.color.y = skinColor.g; geneBody.color.z = skinColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    // Small head jutting forward
    Gene geneHead;
    geneHead.offset    = Codon(0, 0.0f, 0.25f);
    geneHead.position  = Codon(0, 1.8f, 0.35f);
    geneHead.rotation  = Codon(-0.1f, 0, 0);
    geneHead.scale     = Codon(0.35f, 0.35f, 0.40f);
    geneHead.color.x   = skinColor.r; geneHead.color.y = skinColor.g; geneHead.color.z = skinColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Tusks (lower jaw jutting out)
    Gene tuskLeft;
    tuskLeft.offset    = Codon(0.12f, 1.7f, 0.55f);
    tuskLeft.position  = Codon(0, 0, 0);
    tuskLeft.rotation  = Codon(-0.4f, 0, 0.1f);
    tuskLeft.scale     = Codon(0.06f, 0.22f, 0.06f);
    tuskLeft.color.x   = tuskColor.r; tuskLeft.color.y = tuskColor.g; tuskLeft.color.z = tuskColor.b;
    tuskLeft.animationType = ActorState::Animation::Head;
    
    Gene tuskRight = tuskLeft;
    tuskRight.offset.x = -tuskRight.offset.x;
    tuskRight.rotation.z = -tuskRight.rotation.z;
    
    // Massive, long arms
    Gene geneArmLeft;
    geneArmLeft.offset    = Codon(0.45f, 1.6f, 0.1f);
    geneArmLeft.position  = Codon(0, -0.5f, 0);
    geneArmLeft.rotation  = Codon(0, 0, 0);
    geneArmLeft.scale     = Codon(0.22f, 0.85f, 0.22f);
    geneArmLeft.color.x   = skinColor.r; geneArmLeft.color.y = skinColor.g; geneArmLeft.color.z = skinColor.b;
    geneArmLeft.animationAxis    = Codon(0.8f, 0, 0);
    geneArmLeft.animationRange   = 18;
    geneArmLeft.animationType    = ActorState::Animation::Limb;
    
    Gene geneArmRight = geneArmLeft;
    geneArmRight.offset.x = -geneArmRight.offset.x;
    geneArmRight.doInverseAnimation = true;
    geneArmRight.animationType = ActorState::Animation::LimbHolding; // Ready to hold a club
    
    // Short, stubby legs
    Gene geneLegLeft;
    geneLegLeft.offset    = Codon(0.25f, 0.6f, -0.1f);
    geneLegLeft.position  = Codon(0, -0.3f, 0);
    geneLegLeft.rotation  = Codon(0, 0, 0);
    geneLegLeft.scale     = Codon(0.28f, 0.55f, 0.28f);
    geneLegLeft.color.x   = skinColor.r; geneLegLeft.color.y = skinColor.g; geneLegLeft.color.z = skinColor.b;
    geneLegLeft.doInverseAnimation = true;
    geneLegLeft.animationAxis      = Codon(1.2f, 0, 0);
    geneLegLeft.animationRange     = 14;
    geneLegLeft.animationType      = ActorState::Animation::Limb;
    
    Gene geneLegRight = geneLegLeft;
    geneLegRight.offset.x = -geneLegRight.offset.x;
    geneLegRight.doInverseAnimation = false;
    
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(tuskLeft);
    targetActor->genetics.AddGene(tuskRight);
    targetActor->genetics.AddGene(geneArmLeft);
    targetActor->genetics.AddGene(geneArmRight);
    targetActor->genetics.AddGene(geneLegLeft);
    targetActor->genetics.AddGene(geneLegRight);
}

void GeneticPresets::ActorPresets::Rabbit(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.18f, 0.0f, -0.18f}, {0.18f, 0.45f, 0.18f});
    
    targetActor->SetName("Rabbit");
    
    targetActor->physical.SetAdultAge(300);
    targetActor->physical.SetSeniorAge(3000);
    
    targetActor->physical.SetSpeed(0.55f);
    targetActor->physical.SetSpeedYouth(0.38f);
    targetActor->physical.SetSpeedMultiplier(2.6f); // Fast burst sprinter
    
    targetActor->physical.SetYouthScale(0.10f);
    targetActor->physical.SetAdultScale(0.28f);
    
    targetActor->behavior.SetHeightPreferenceMax(15.0f);
    
    targetActor->behavior.SetDistanceToAttack(1.0f);
    targetActor->behavior.SetDistanceToFlee(12.0f);
    targetActor->behavior.SetDistanceToFocus(15.0f);
    targetActor->behavior.SetDistanceToWalk(6.0f);
    targetActor->behavior.SetDistanceToInflict(0.12f);
    
    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 20.0f;
    targetActor->biological.strength = 1.5f;
    targetActor->biological.defense  = 0.5f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); } else  // Male
        { targetActor->physical.SetSexualOrientation(false); }      // Female
    
    // Color variants (Brown, Gray, Black, White)
    Color baseColor = Colors.gray;
    int colorRoll = Random.Range(0, 100);
    
    if (colorRoll > 75) {
        baseColor = Colors.brown * 0.04f;                       // Brown rabbit
    } else if (colorRoll > 50) {
        baseColor = Colors.gray * Colors.MakeRandomGrayScale(); // Gray/Agouti
    } else if (colorRoll > 25) {
        baseColor = Colors.black * 0.02f;                       // Black rabbit
    } else {
        baseColor = Colors.white * 0.95f;                       // White rabbit
    }
    
    Color headColor = baseColor;
    Color limbColor = baseColor;
    
    // Body Gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.18f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.20f, 0.22f, 0.32f);
    geneBody.color.x   = baseColor.r;
    geneBody.color.y   = baseColor.g;
    geneBody.color.z   = baseColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    
    // Head Gene
    Gene geneHead;
    geneHead.offset    = Codon(0.0f, 0, 0.15f);
    geneHead.position  = Codon(0, 0.26f, 0.08f);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.15f, 0.15f, 0.17f);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    
    // Left Ear (Long upright ears)
    Gene geneEarLeft;
    geneEarLeft.offset    = Codon(0, 0, 0.15f);
    geneEarLeft.position  = Codon(0.05f, 0.38f, 0.06f);
    geneEarLeft.rotation  = Codon(-0.15f, 0, -0.08f);
    geneEarLeft.scale     = Codon(0.04f, 0.22f, 0.03f);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    geneEarLeft.animationType = ActorState::Animation::Head;
    
    // Right Ear
    Gene geneEarRight = geneEarLeft;
    geneEarRight.position.x = -geneEarRight.position.x;
    geneEarRight.rotation.z = -geneEarRight.rotation.z;
    
    // Tail (Small cotton puff)
    Gene geneTail;
    geneTail.offset    = Codon(0.0f, 0.0f, 0.0f);
    geneTail.position  = Codon(0.0f, 0.20f, -0.18f);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.08f, 0.08f, 0.08f);
    geneTail.color.x   = baseColor.r;
    geneTail.color.y   = baseColor.g;
    geneTail.color.z   = baseColor.b;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.07f, 0.16f, 0.10f);
    geneLimbFrontLeft.position  = Codon(0.0f, -0.08f, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.05f, 0.16f, 0.05f);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.animationType = ActorState::Animation::Limb;
    geneLimbFrontLeft.animationAxis = Codon(1.6f, 0, 0);
    geneLimbFrontLeft.animationRange = 16;
    
    // Front Right Leg
    Gene geneLimbFrontRight = geneLimbFrontLeft;
    geneLimbFrontRight.offset.x = -geneLimbFrontRight.offset.x;
    geneLimbFrontRight.doInverseAnimation = true;
    
    // Rear Left Leg (Larger for propulsion)
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.08f, 0.16f, -0.10f);
    geneLimbRearLeft.position  = Codon(0.0f, -0.09f, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.07f, 0.20f, 0.08f);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.animationType = ActorState::Animation::Limb;
    geneLimbRearLeft.animationAxis = Codon(1.8f, 0, 0);
    geneLimbRearLeft.animationRange = 18;
    
    // Rear Right Leg
    Gene geneLimbRearRight = geneLimbRearLeft;
    geneLimbRearRight.offset.x = -geneLimbRearRight.offset.x;
    geneLimbRearRight.doInverseAnimation = true;
    
    // Apply genes to actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneTail);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
}
