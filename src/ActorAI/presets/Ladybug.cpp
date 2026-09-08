#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

void GeneticPresets::ActorPresets::Ladybug(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.2,0.0,-0.2}, {0.2,0.5,0.2});
    
    targetActor->SetName("LadyBug");
    
    targetActor->physical.SetAdultAge(260);
    targetActor->physical.SetSeniorAge(2800);
    
    targetActor->physical.SetSpeed(0.35f);
    targetActor->physical.SetSpeedYouth(0.30f);
    targetActor->physical.SetSpeedMultiplier(1.6f);
    
    targetActor->physical.SetYouthScale(0.060f);
    targetActor->physical.SetAdultScale(0.180f);
    
    targetActor->behavior.SetHeightPreferenceMax(8.0f);
    targetActor->behavior.SetDistanceToAttack(1.5f);
    targetActor->behavior.SetDistanceToFlee(7.0f);
    targetActor->behavior.SetDistanceToFocus(10.0f);
    targetActor->behavior.SetDistanceToWalk(6.0f);
    targetActor->behavior.SetDistanceToInflict(0.15f);
    
    targetActor->behavior.SetPredatorState(false);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health   = 16.0f;
    targetActor->biological.strength = 1.2f;
    targetActor->biological.defense  = 1.5f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); } else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color headColor = Colors.black;
    Color bodyColor = Colors.red * 0.85f;
    Color limbColor = Colors.black;
    
    // Small black head
    Gene head;
    head.offset   = Codon(0, 0, 0.25f);
    head.position = Codon(0, 0.22f, 0.12f);
    head.scale    = Codon(0.22f, 0.18f, 0.18f);
    head.color.x  = headColor.r;
    head.color.y  = headColor.g;
    head.color.z  = headColor.b;
    head.animationType = ActorState::Animation::Head;
    
    // Red body (shell)
    Gene body;
    body.offset   = Codon(0, 0, 0);
    body.position = Codon(0, 0.14f, 0.0f);
    body.scale    = Codon(0.55f, 0.45f, 0.60f);
    body.color.x  = bodyColor.r;
    body.color.y  = bodyColor.g;
    body.color.z  = bodyColor.b;
    body.animationType = ActorState::Animation::Body;
    
    targetActor->genetics.AddGene(head);
    targetActor->genetics.AddGene(body);
    
    // 6 legs (3 pairs)
    float  xx     = 0.18f;
    float  zz     = 0.22f;
    float  canter = -0.25f;
    bool   invLeft  = true;
    bool   invRight = false;
    
    for (unsigned int i = 0; i < 3; i++) {
        Gene leftLimb  = AI.genomes.build.BuildAddLeg( xx, 0.11f, zz, 0.32f, 0.085f, limbColor, invLeft,  true,  canter, 1.5f, 0.18f);
        Gene rightLimb = AI.genomes.build.BuildAddLeg(-xx, 0.11f, zz, 0.32f, 0.085f, limbColor, invRight, false, canter, 1.5f, 0.18f);
        
        targetActor->genetics.AddGene(leftLimb);
        targetActor->genetics.AddGene(rightLimb);
        
        xx     -= 0.03f;
        zz     -= 0.10f;
        canter += 0.40f;
        
        invLeft  = !invLeft;
        invRight = !invRight;
    }
}
