#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->biological.healthMax = 180.0f;
    targetActor->biological.health    = 180.0f;
    targetActor->biological.strength  = 72.0f;
    targetActor->biological.defense   = 3.0f;
    
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
