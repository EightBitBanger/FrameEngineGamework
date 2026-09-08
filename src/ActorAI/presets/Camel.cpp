#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/ActorSystemInternal.h>

void GeneticPresets::ActorPresets::Camel(Actor* targetActor) {
    AI.genomes.ClearGenes(targetActor);
    
    // Camel configuration matching Elk/Boar pattern
    targetActor->SetBoundingBox({-0.45f, 0.0f, -0.65f}, {0.45f, 2.2f, 0.65f});
    targetActor->SetName("Camel");
    
    targetActor->physical.SetAdultAge(700);
    targetActor->physical.SetSeniorAge(5500);
    
    targetActor->physical.SetSpeed(0.55f);
    targetActor->physical.SetSpeedYouth(0.38f);
    targetActor->physical.SetSpeedMultiplier(1.6f);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.85f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetPreyState(true);
    
    targetActor->biological.health = 150.0f;
    targetActor->biological.strength = 25.0f;
    targetActor->biological.defense = 4.0f;
    
    if (Random.Range(0, 100) > 55)
        { targetActor->physical.SetSexualOrientation(true); }
    else
        { targetActor->physical.SetSexualOrientation(false); }
    
    Color bodyColor = Colors.brown * Colors.Lerp(Colors.white, Colors.yellow, 0.4f) * 0.7f;
    Color headColor = bodyColor * Colors.MakeGrayScale(0.95f);
    Color limbColor = bodyColor * Colors.MakeGrayScale(0.85f);
    
    Color variation = Colors.MakeRandomGrayScale();
    bodyColor *= variation; headColor *= variation; limbColor *= variation;
    
    // Body Gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 1.1f, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.52f, 0.55f, 1.35f);
    geneBody.color.x   = bodyColor.r; geneBody.color.y = bodyColor.g; geneBody.color.z = bodyColor.b;
    geneBody.animationType = ActorState::Animation::Body;
    targetActor->genetics.AddGene(geneBody);
    
    // Long Neck Gene
    Gene geneNeck;
    geneNeck.offset    = Codon(0, 1.3f, 0.55f);
    geneNeck.position  = Codon(0, 0.3f, 0.2f);
    geneNeck.rotation  = Codon(0.45f, 0, 0);
    geneNeck.scale     = Codon(0.24f, 0.65f, 0.26f);
    geneNeck.color.x   = bodyColor.r; geneNeck.color.y = bodyColor.g; geneNeck.color.z = bodyColor.b;
    geneNeck.animationType = ActorState::Animation::Head; 
    targetActor->genetics.AddGene(geneNeck);
    
    // Head Gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.85f, 0.85f);
    geneHead.position  = Codon(0, 0, 0.25f);
    geneHead.rotation  = Codon(0.2f, 0, 0);
    geneHead.scale     = Codon(0.28f, 0.28f, 0.48f);
    geneHead.color.x   = headColor.r; geneHead.color.y = headColor.g; geneHead.color.z = headColor.b;
    geneHead.animationType = ActorState::Animation::Head;
    targetActor->genetics.AddGene(geneHead);
    
    // Hump
    if (Random.Range(0, 100) > 50) {
        // Dromedary: Single hump
        Gene singleHump;
        singleHump.offset   = Codon(0, 1.38f, 0.0f); 
        singleHump.position = Codon(0, 0, 0);
        singleHump.scale    = Codon(0.45f, 0.42f, 0.6f);
        singleHump.color.x  = bodyColor.r; singleHump.color.y = bodyColor.g; singleHump.color.z = bodyColor.b;
        targetActor->genetics.AddGene(singleHump);
    } else {
        // Bactrian: Two humps
        Gene frontHump;
        frontHump.offset   = Codon(0, 1.35f, 0.25f);
        frontHump.position = Codon(0, 0, 0);
        frontHump.scale    = Codon(0.42f, 0.36f, 0.38f);
        frontHump.color.x  = bodyColor.r; frontHump.color.y = bodyColor.g; frontHump.color.z = bodyColor.b;
        targetActor->genetics.AddGene(frontHump);
        
        Gene rearHump = frontHump;
        rearHump.offset.z = -0.25f;
        targetActor->genetics.AddGene(rearHump);
    }
    
    // Long Legs
    Gene FL;
    FL.offset    = Codon(0.2f, 1.0f, 0.45f);
    FL.position  = Codon(0, -0.5f, 0);
    FL.scale     = Codon(0.15f, 1.0f, 0.15f);
    FL.color.x   = limbColor.r; FL.color.y = limbColor.g; FL.color.z = limbColor.b;
    FL.animationType = ActorState::Animation::Limb;
    FL.animationAxis = Codon(1, 0, 0);
    FL.animationRange = 16;
    
    Gene FR = FL; FR.offset.x = -FR.offset.x; FR.doInverseAnimation = true;
    Gene RL = FL; RL.offset.z = -RL.offset.z;
    Gene RR = FR; RR.offset.z = -RR.offset.z;
    
    targetActor->genetics.AddGene(FL);
    targetActor->genetics.AddGene(FR);
    targetActor->genetics.AddGene(RL);
    targetActor->genetics.AddGene(RR);
    
    // Tail
    Gene tail;
    tail.offset   = Codon(0, 1.1f, -0.65f);
    tail.position = Codon(0, -0.25f, -0.05f);
    tail.rotation = Codon(0.2f, 0, 0);
    tail.scale    = Codon(0.08f, 0.45f, 0.08f);
    tail.color.x  = bodyColor.r; tail.color.y = bodyColor.g; tail.color.z = bodyColor.b;
    targetActor->genetics.AddGene(tail);
}
