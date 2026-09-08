#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
