#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

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
