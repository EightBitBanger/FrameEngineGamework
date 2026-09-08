#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

void GeneticPresets::ActorPresets::Raptor(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetBoundingBox({-0.35,0.0,-0.35}, {0.35,1.2,0.35});
    
    targetActor->SetName("Raptor");
    
    targetActor->physical.SetAdultAge(700);
    targetActor->physical.SetSeniorAge(12000);
    
    targetActor->physical.SetSpeed(0.6f);
    targetActor->physical.SetSpeedYouth(0.45f);
    targetActor->physical.SetSpeedMultiplier(3.0f);
    
    targetActor->physical.SetYouthScale(0.25f);
    targetActor->physical.SetAdultScale(0.80f);
    
    targetActor->behavior.SetHeightPreferenceMin(5.0f);
    targetActor->behavior.SetHeightPreferenceMax(45.0f);
    
    targetActor->behavior.SetDistanceToAttack(8.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(20.0f);
    targetActor->behavior.SetDistanceToWalk(12.0f);
    targetActor->behavior.SetDistanceToInflict(0.35f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(false);
    
    targetActor->biological.health = 120.0f;
    targetActor->biological.strength = 14.0f;
    targetActor->biological.defense  = 6.0f;
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else
        {targetActor->physical.SetSexualOrientation(false);}
    
    // Colors: dark, slightly brown/gray randomized
    Color base = Colors.dkgray * Colors.MakeRandomGrayScale();
    if (Random.Range(0, 100) > 40) base *= Colors.brown;
    Color limb = base * Colors.MakeGrayScale(0.9f);
    Color head = base * Colors.MakeGrayScale(0.95f);
    
    // Body (lean torso)
    Gene body;
    body.offset   = Codon(0, 0, 0);
    body.position = Codon(0, 1.0f, 0);
    body.rotation = Codon(0, 0, 0);
    body.scale    = Codon(0.42f, 0.48f, 0.95f);
    body.color.x  = base.r; body.color.y = base.g; body.color.z = base.b;
    body.animationType = ActorState::Animation::Body;
    
    // Neck
    Gene neck;
    neck.offset   = Codon(0, 0, 0);
    neck.position = Codon(0, 1.25f, 0.20f);
    neck.rotation = Codon(0.25f, 0, 0);
    neck.scale    = Codon(0.18f, 0.28f, 0.16f);
    neck.color.x  = base.r; neck.color.y = base.g; neck.color.z = base.b;
    neck.animationType = ActorState::Animation::Head;
    
    // Head (attached to neck)
    Gene headU;
    headU.attachmentIndex = 2; // neck (second gene in list)
    headU.offset   = Codon(0, 0, 0);
    headU.position = Codon(0, 1.48f, 0.50f);
    headU.rotation = Codon(0.28f, 0, 0);
    headU.scale    = Codon(0.26f, 0.24f, 0.32f);
    headU.color.x  = head.r; headU.color.y = head.g; headU.color.z = head.b;
    headU.animationType = ActorState::Animation::Head;
    
    // Jaw (attached to neck too for simple open/close)
    Gene jaw;
    jaw.attachmentIndex = 2;
    jaw.offset   = Codon(0, 0, 0);
    jaw.position = Codon(0, 1.40f, 0.46f);
    jaw.rotation = Codon(0.45f, 0, 0);
    jaw.scale    = Codon(0.22f, 0.18f, 0.28f);
    jaw.color.x  = head.r; jaw.color.y = head.g; jaw.color.z = head.b;
    jaw.animationType = ActorState::Animation::Head;
    
    // Arms (short)
    Gene armL;
    armL.offset   = Codon(0.26f, 1.02f, 0.18f);
    armL.position = Codon(0, -0.22f, 0);
    armL.rotation = Codon(0, 0, 0);
    armL.scale    = Codon(0.09f, 0.34f, 0.09f);
    armL.color.x  = limb.r; armL.color.y = limb.g; armL.color.z = limb.b;
    armL.animationType = ActorState::Animation::Limb;
    armL.animationAxis = Codon(1, 0, 0);
    armL.animationRange = 16;
    
    Gene armR = armL;
    armR.offset.x = -armR.offset.x;
    armR.doInverseAnimation = true;
    
    // Legs (powerful)
    Gene legL;
    legL.offset   = Codon(0.14f, 0.62f, -0.06f);
    legL.position = Codon(0, -0.30f, 0);
    legL.rotation = Codon(0, 0, 0);
    legL.scale    = Codon(0.18f, 0.78f, 0.18f);
    legL.color.x  = limb.r; legL.color.y = limb.g; legL.color.z = limb.b;
    legL.animationType = ActorState::Animation::Limb;
    legL.animationAxis = Codon(1, 0, 0);
    legL.animationRange = 18;
    
    Gene legR = legL;
    legR.offset.x = -legR.offset.x;
    legR.doInverseAnimation = true;
    
    // Tail (long for balance)
    Gene tail;
    tail.offset   = Codon(0, 0, 0);
    tail.position = Codon(0, 0.94f, -0.78f);
    tail.rotation = Codon(0.02f, 0, 0);
    tail.scale    = Codon(0.12f, 0.12f, 1.05f);
    tail.color.x  = base.r; tail.color.y = base.g; tail.color.z = base.b;
    tail.animationType = ActorState::Animation::Body;
    
    targetActor->genetics.AddGene(body);
    targetActor->genetics.AddGene(neck);
    targetActor->genetics.AddGene(headU);
    targetActor->genetics.AddGene(jaw);
    targetActor->genetics.AddGene(armL);
    targetActor->genetics.AddGene(armR);
    targetActor->genetics.AddGene(legL);
    targetActor->genetics.AddGene(legR);
    targetActor->genetics.AddGene(tail);
}
