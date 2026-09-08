#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

void GeneticPresets::ActorPresets::Spider(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    targetActor->SetBoundingBox({-0.2,0.0,-0.2}, {0.2,0.5,0.2});
    
    targetActor->SetName("Spider");
    
    targetActor->physical.SetAdultAge(300);
    targetActor->physical.SetSeniorAge(4000);
    
    targetActor->physical.SetSpeed(0.5f);
    targetActor->physical.SetSpeedYouth(0.4f);
    targetActor->physical.SetSpeedMultiplier(2.2f);
    
    targetActor->physical.SetYouthScale(0.087f);
    targetActor->physical.SetAdultScale(0.3f);
    
    targetActor->behavior.SetHeightPreferenceMax(25.0f);
    targetActor->behavior.SetDistanceToAttack(4.0f);
    targetActor->behavior.SetDistanceToFlee(10.0f);
    targetActor->behavior.SetDistanceToFocus(14.0f);
    targetActor->behavior.SetDistanceToWalk(8.0f);
    targetActor->behavior.SetDistanceToInflict(0.2f);
    
    targetActor->behavior.SetPredatorState(true);
    targetActor->behavior.SetPreyState(false);
    
    targetActor->animation.SetAnimationCycleRate(33.0f);
    
    targetActor->biological.health = 40.0f;
    targetActor->biological.strength = 6.0f;
    targetActor->biological.defense  = 2.0f;
    
    if (Random.Range(0, 100) > 55)
        {targetActor->physical.SetSexualOrientation(true);} else
        {targetActor->physical.SetSexualOrientation(false);}
    
    Color headColor = Colors.dkgray * Colors.MakeGrayScale(0.03f);
    Color bodyColor = Colors.black * Colors.MakeGrayScale(0.02f);
    Color limbColor = Colors.dkgray * Colors.MakeGrayScale(0.02f);
    //Color redMark = Colors.red;
    
    if (Random.Range(0, 100) > 70) 
        bodyColor = Colors.brown * 0.03f;
    
    Gene head;
    head.offset   = Codon(0, 0, 0.27f);
    head.position = Codon(0, 0.2f, 0.05f);
    head.scale    = Codon(0.3f, 0.24f, 0.14f);
    head.color.x = headColor.r;
    head.color.y = headColor.g;
    head.color.z = headColor.b;
    head.animationType = ActorState::Animation::Head;
    
    Gene abdomen;
    abdomen.offset   = Codon(0, 0, 0);
    abdomen.position = Codon(0, 0.1f, 0.0f);
    abdomen.scale    = Codon(0.42f, 0.36f, 0.50f);
    abdomen.color.x = bodyColor.r;
    abdomen.color.y = bodyColor.g;
    abdomen.color.z = bodyColor.b;
    abdomen.animationType = ActorState::Animation::Body;
    
    targetActor->genetics.AddGene(head);
    targetActor->genetics.AddGene(abdomen);
    
    float xx = 0.25f;
    float zz = 0.38f;
    float canter = -0.7f;
    bool invLeft  = true;
    bool invRight = false;
    
    for (unsigned int i=0; i < 4; i++) {
        Gene leftLimb  = AI.genomes.build.BuildAddLeg( xx, 0.14f, zz, 0.5f, 0.1f, limbColor, invLeft,  true,  canter, 1.9f, 0.24f);
        Gene rightLimb = AI.genomes.build.BuildAddLeg(-xx, 0.14f, zz, 0.5f, 0.1f, limbColor, invRight, false, canter, 1.9f, 0.24f);
        
        targetActor->genetics.AddGene(leftLimb);
        targetActor->genetics.AddGene(rightLimb);
        
        xx -= 0.05f;
        zz -= 0.12f;
        canter += 0.5f;
        
        invLeft  = !invLeft;
        invRight = !invRight;
    }
}
