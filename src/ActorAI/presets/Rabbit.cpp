#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
