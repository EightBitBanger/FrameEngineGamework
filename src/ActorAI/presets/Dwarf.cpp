#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->biological.healthMax  = 260.0f;
    targetActor->biological.health     = 130.0f;
    targetActor->biological.strength   = 95.0f;
    targetActor->biological.defense    = 8.0f;
    targetActor->biological.hunger     = 0.5f;
    
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
