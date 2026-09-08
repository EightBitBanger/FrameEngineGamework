#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->behavior.SetDistanceToFocus(15.0f);
    targetActor->behavior.SetDistanceToWalk(10.0f);
    targetActor->behavior.SetDistanceToAttack(5.0f);
    targetActor->behavior.SetDistanceToFlee(12.0f);
    targetActor->behavior.SetDistanceToInflict(0.3f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.healthMax  = 24.0f;
    targetActor->biological.health     = 24.0f;
    targetActor->biological.strength   = 10.0f;
    targetActor->biological.defense    = 1.0f;
    
    targetActor->behavior.SetCooldownAttack(1);
    targetActor->behavior.SetCooldownBreed(220);
    targetActor->behavior.SetCooldownMove(1);
    targetActor->behavior.SetCooldownObserve(2);
    targetActor->behavior.SetCooldownSocial(2);
    
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
