#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.healthMax  = 95.0f;
    targetActor->biological.health     = 95.0f;
    targetActor->biological.strength   = 36.0f;
    targetActor->biological.defense    = 2.0f;
    
    targetActor->behavior.SetCooldownAttack(1);
    targetActor->behavior.SetCooldownBreed(220);
    targetActor->behavior.SetCooldownMove(2);
    targetActor->behavior.SetCooldownObserve(3);
    targetActor->behavior.SetCooldownSocial(3);
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else 
        {targetActor->physical.SetSexualOrientation(false);}
    
    // Color variants
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
