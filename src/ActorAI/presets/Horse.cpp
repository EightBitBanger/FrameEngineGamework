#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->behavior.SetHeightPreferenceMax(40.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(15.0f);
    targetActor->behavior.SetDistanceToWalk(10.0f);
    targetActor->behavior.SetDistanceToAttack(5.0f);
    targetActor->behavior.SetDistanceToFlee(12.0f);
    targetActor->behavior.SetDistanceToInflict(0.3f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.healthMax  = 150.0f;
    targetActor->biological.health     = 150.0f;
    targetActor->biological.strength   = 100.0f;
    targetActor->biological.defense    = 1.0f;
    
    targetActor->behavior.SetCooldownAttack(1);
    targetActor->behavior.SetCooldownBreed(220);
    targetActor->behavior.SetCooldownMove(2);
    targetActor->behavior.SetCooldownObserve(3);
    targetActor->behavior.SetCooldownSocial(3);
    
    if (Random.Range(0, 100) > 55) 
        {targetActor->physical.SetSexualOrientation(true);} else 
        {targetActor->physical.SetSexualOrientation(false);}
    
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
