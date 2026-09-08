#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

void GeneticPresets::ActorPresets::HumanWhite(Actor* targetActor) {
    Color headColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
    Color limbColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
    Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
    
    bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
    bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
    bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
    bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
    Human(targetActor, limbColor, bodyColor);
}

void GeneticPresets::ActorPresets::HumanRed(Actor* targetActor) {
    Color headColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
    Color limbColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
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
    Color headColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
    Color limbColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
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
    Color headColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
    Color limbColor = (Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f)) * 0.7f;
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
    
    targetActor->physical.SetAdultAge(18000);
    targetActor->physical.SetSeniorAge(60000);
    
    targetActor->physical.SetSpeed(0.6f);
    targetActor->physical.SetSpeedYouth(0.4f);
    targetActor->physical.SetSpeedMultiplier(2.24f);
    
    targetActor->physical.SetYouthScale(0.15f);
    targetActor->physical.SetAdultScale(0.75f);
    
    targetActor->behavior.SetHeightPreferenceMax(40.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToWalk(20.0f);
    targetActor->behavior.SetDistanceToAttack(20.0f);
    targetActor->behavior.SetDistanceToFlee(5.0f);
    targetActor->behavior.SetDistanceToInflict(0.3f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.healthMax  = 90.0f;
    targetActor->biological.health     = 90.0f;
    targetActor->biological.strength   = 30.0f;
    targetActor->biological.defense    = 1.0f;
    
    targetActor->behavior.SetCooldownAttack(1);
    targetActor->behavior.SetCooldownBreed(220);
    targetActor->behavior.SetCooldownMove(2);
    targetActor->behavior.SetCooldownObserve(3);
    targetActor->behavior.SetCooldownSocial(3);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    Color headColor = limb;
    Color limbColor = limb;
    Color bodyColor = body;
    Color beardColor = Colors.gray;
    Color eyeColor   = Colors.black;
    
    // Eye color
    if (Random.Range(0, 100) > 80) {
        if (Random.Range(0, 100) > 80) {eyeColor = Colors.blue   * 0.03;} else 
        if (Random.Range(0, 100) > 80) {eyeColor = Colors.green  * 0.03f;} else 
        if (Random.Range(0, 100) > 70) {eyeColor = Colors.brown  * 0.08f;} else 
        if (Random.Range(0, 100) > 95) {eyeColor = Colors.orange * 0.05f;} else 
        if (Random.Range(0, 100) > 99) {eyeColor = Colors.red    * 0.05f;}
    }
    
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
    geneHead.position  = Codon(0, 1.34, 0);
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
    geneBeard.animationType      = ActorState::Animation::Head;
    
    // Eyes
    Gene geneEyeLeft;
    geneEyeLeft.offset    = Codon(0, 0, 0);
    geneEyeLeft.position  = Codon(0.07f, 1.37f, 0.16f);
    geneEyeLeft.rotation  = Codon(0, 0, 0);
    geneEyeLeft.scale     = Codon(0.03f, 0.045f, 0.03f);
    geneEyeLeft.color.x   = eyeColor.r;
    geneEyeLeft.color.y   = eyeColor.g;
    geneEyeLeft.color.z   = eyeColor.b;
    geneEyeLeft.animationType = ActorState::Animation::Head;
    Gene geneEyeRight         = geneEyeLeft;
    geneEyeRight.position.x   = -geneEyeRight.position.x;
    geneEyeRight.colorIndex   = 4;
    geneEyeLeft.scaleIndex    = 5;
    
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
    geneBreastLeft.animationType    = ActorState::Animation::Body;
    
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
    geneBreastRight.scaleIndex       = 6;
    geneBreastRight.colorIndex       = 1;
    geneBreastRight.expressionFactor = 1.5;
    geneBreastRight.expressionMax    = 1.4;
    geneBreastRight.expressionAge    = 13000;
    geneBreastRight.animationType    = ActorState::Animation::Body;
    
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
    geneShoulderLeft.animationType    = ActorState::Animation::Limb;
    
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
    geneShoulderRight.scaleIndex         = 8;
    geneShoulderRight.form               = ActorState::Genetic::Male;
    geneShoulderRight.expressionFactor = 1.5;
    geneShoulderRight.expressionMax    = 1.1;
    geneShoulderRight.expressionAge    = 13000;
    geneShoulderRight.animationType    = ActorState::Animation::LimbHolding;
    
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
    geneLimbFrontLeft.colorIndex       = 2;
    geneLimbFrontLeft.animationType    = ActorState::Animation::Limb;
    
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
    geneLimbFrontRight.colorIndex         = 2;
    geneLimbFrontRight.animationType      = ActorState::Animation::LimbHolding;
    
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
    geneLimbRearLeft.animationAxis      = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange     = 13;
    geneLimbRearLeft.colorIndex         = 2;
    geneLimbRearLeft.animationType      = ActorState::Animation::Limb;
    
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
    geneLimbRearRight.colorIndex         = 2;
    geneLimbRearRight.animationType      = ActorState::Animation::Limb;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
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
