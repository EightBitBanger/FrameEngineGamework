#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
    
    targetActor->behavior.SetHeightPreferenceMax(25.0f);
    targetActor->behavior.SetHeightPreferenceMin(0.0f);
    
    targetActor->behavior.SetDistanceToFocus(24.0f);
    targetActor->behavior.SetDistanceToWalk(14.0f);
    targetActor->behavior.SetDistanceToAttack(3.5f);
    targetActor->behavior.SetDistanceToFlee(20.0f);
    targetActor->behavior.SetDistanceToInflict(0.22f);
    
    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.healthMax  = 170.0f;
    targetActor->biological.health     = 170.0f;
    targetActor->biological.strength   = 20.0f;
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
