#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>

extern NumberGeneration  Random;
extern ColorPreset       Colors;
extern FloatType         Float;
extern StringType        String;
extern RenderSystem      Renderer;
extern ActorSystem       AI;


std::string GeneticPresets::ExtractGenome(Actor* actorSource) {
    
    // Extract actor idiosyncrasies from the genome
    std::string genetics;
    
    unsigned int numberOfGenes = actorSource->GetNumberOfGenes();
    
    genetics += actorSource->GetName() + ":";
    
    genetics += Float.ToString( actorSource->GetSpeed() ) + ":";
    genetics += Float.ToString( actorSource->GetSpeedMultiplier() ) + ":";
    genetics += Float.ToString( actorSource->GetSpeedYouth() ) + ":";
    
    genetics += Float.ToString( actorSource->GetYouthScale() ) + ":";
    genetics += Float.ToString( actorSource->GetAdultScale() ) + ":";
    
    // Personality
    genetics += Float.ToString( actorSource->GetChanceToChangeDirection() ) + ":";
    genetics += Float.ToString( actorSource->GetChanceToFocusOnActor() ) + ":";
    genetics += Float.ToString( actorSource->GetChanceToStopWalking() ) + ":";
    genetics += Float.ToString( actorSource->GetChanceToWalk() ) + ":";
    
    genetics += Float.ToString( actorSource->GetDistanceToWalk() ) + ":";
    genetics += Float.ToString( actorSource->GetDistanceToAttack() ) + ":";
    genetics += Float.ToString( actorSource->GetDistanceToFlee() ) + ":";
    genetics += Float.ToString( actorSource->GetHeightPreferenceMin() ) + ":";
    genetics += Float.ToString( actorSource->GetHeightPreferenceMax() ) + ":";
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene gene = actorSource->GetGeneFromGenome( i );
        
        genetics += "#";
        
        genetics += Float.ToString( gene.position.x ) + ",";
        genetics += Float.ToString( gene.position.y ) + ",";
        genetics += Float.ToString( gene.position.z ) + "|";
        
        genetics += Float.ToString( gene.rotation.x ) + ",";
        genetics += Float.ToString( gene.rotation.y ) + ",";
        genetics += Float.ToString( gene.rotation.z ) + "|";
        
        genetics += Float.ToString( gene.scale.x ) + ",";
        genetics += Float.ToString( gene.scale.y ) + ",";
        genetics += Float.ToString( gene.scale.z ) + "|";
        
        genetics += Float.ToString( gene.offset.x ) + ",";
        genetics += Float.ToString( gene.offset.y ) + ",";
        genetics += Float.ToString( gene.offset.z ) + "|";
        
        genetics += Float.ToString( gene.color.x ) + ",";
        genetics += Float.ToString( gene.color.y ) + ",";
        genetics += Float.ToString( gene.color.z ) + "|";
        
        genetics += Float.ToString( gene.doInverseAnimation ) + ",";
        genetics += Float.ToString( gene.doAnimationCycle ) + "|";
        
        continue;
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* actorSource, std::string genome) {
    
    // Inject actor idiosyncrasies into the genome
    std::vector<std::string> traits = String.Explode( genome, ':' );
    
    actorSource->SetName(traits[0]);
    
    actorSource->SetSpeed( String.ToFloat(traits[1]) );
    actorSource->SetSpeedMultiplier( String.ToFloat(traits[2]) );
    actorSource->SetSpeedYouth( String.ToFloat(traits[3]) );
    
    actorSource->SetYouthScale( String.ToFloat(traits[4]) );
    actorSource->SetAdultScale( String.ToFloat(traits[5]) );
    
    // Personality
    actorSource->SetChanceToChangeDirection( String.ToFloat(traits[6]) );
    actorSource->SetChanceToFocusOnActor( String.ToFloat(traits[7]) );
    actorSource->SetChanceToStopWalking( String.ToFloat(traits[8]) );
    actorSource->SetChanceToWalk( String.ToFloat(traits[9]) );
    
    actorSource->SetDistanceToWalk( String.ToFloat(traits[10]) );
    actorSource->SetDistanceToAttack( String.ToFloat(traits[11]) );
    actorSource->SetDistanceToFlee( String.ToFloat(traits[12]) );
    actorSource->SetHeightPreferenceMin( String.ToFloat(traits[13]) );
    actorSource->SetHeightPreferenceMax( String.ToFloat(traits[14]) );
    
    // Extract genes from the genome
    std::vector<std::string> genes = String.Explode( genome, '#' );
    
    unsigned int numberOfGenes = genes.size();
    for (unsigned int i=1; i < numberOfGenes; i++) {
        
        // Extract sub genes
        std::vector<std::string> subGenes = String.Explode( (genes[i]), '|' );
        std::vector<std::string> baseGene;
        
        Gene gene;
        
        baseGene = String.Explode( subGenes[0], ',' );
        gene.position.x = String.ToFloat( baseGene[0] );
        gene.position.y = String.ToFloat( baseGene[1] );
        gene.position.z = String.ToFloat( baseGene[2] );
        
        baseGene = String.Explode( subGenes[1], ',' );
        gene.rotation.x = String.ToFloat( baseGene[0] );
        gene.rotation.y = String.ToFloat( baseGene[1] );
        gene.rotation.z = String.ToFloat( baseGene[2] );
        
        baseGene = String.Explode( subGenes[2], ',' );
        gene.scale.x = String.ToFloat( baseGene[0] );
        gene.scale.y = String.ToFloat( baseGene[1] );
        gene.scale.z = String.ToFloat( baseGene[2] );
        
        baseGene = String.Explode( subGenes[3], ',' );
        gene.offset.x = String.ToFloat( baseGene[0] );
        gene.offset.y = String.ToFloat( baseGene[1] );
        gene.offset.z = String.ToFloat( baseGene[2] );
        
        baseGene = String.Explode( subGenes[4], ',' );
        gene.color.x = String.ToFloat( baseGene[0] );
        gene.color.y = String.ToFloat( baseGene[1] );
        gene.color.z = String.ToFloat( baseGene[2] );
        
        baseGene = String.Explode( subGenes[5], ',' );
        if (baseGene[0] == "1") {gene.doInverseAnimation = 1;}
        if (baseGene[1] == "1") {gene.doAnimationCycle   = 1;}
        
        actorSource->AddGene( gene );
        
    }
    
    
    return true;
}

bool GeneticPresets::ConjugateGenome(Actor* actorA, Actor* actorB, Actor* targetActor) {
    
    unsigned int numberOfGenesA = actorA->mGenes.size();
    unsigned int numberOfGenesB = actorB->mGenes.size();
    
    // Check genetic incompatibility
    if (numberOfGenesA != numberOfGenesB) 
        return false;
    
    targetActor->mGenes.clear();
    
    for (unsigned int i=0; i < numberOfGenesA; i++) {
        
        Gene variant;
        
        if (Random.Range(0, 100) >= 50) {
            
            variant = Lerp(actorA->mGenes[i], actorB->mGenes[i], 0.9999f);
            
        } else {
            
            variant = Lerp(actorA->mGenes[i], actorB->mGenes[i], 0.0001f);
        }
        
        targetActor->mGenes.push_back(variant);
        
        // Targeting and motion parameters
        
        float chanceToChangeDirectionA = actorA->GetChanceToChangeDirection();
        float chanceToFocusOnActorA    = actorA->GetChanceToFocusOnActor();
        float chanceToStopWalkingA     = actorA->GetChanceToStopWalking();
        float chanceToWalkA            = actorA->GetChanceToWalk();
        
        float chanceToChangeDirectionB = actorB->GetChanceToChangeDirection();
        float chanceToFocusOnActorB    = actorB->GetChanceToFocusOnActor();
        float chanceToStopWalkingB     = actorB->GetChanceToStopWalking();
        float chanceToWalkB            = actorB->GetChanceToWalk();
        
        float chanceToChangeDirection = Float.Lerp(chanceToChangeDirectionA, chanceToChangeDirectionB, 0.5f);
        float chanceToFocusOnActor    = Float.Lerp(chanceToFocusOnActorA, chanceToFocusOnActorB, 0.5f);
        float chanceToStopWalking     = Float.Lerp(chanceToStopWalkingA, chanceToStopWalkingB, 0.5f);
        float chanceToWalk            = Float.Lerp(chanceToWalkA, chanceToWalkB, 0.5f);
        
        targetActor->SetChanceToChangeDirection(chanceToChangeDirection);
        targetActor->SetChanceToFocusOnActor(chanceToFocusOnActor);
        targetActor->SetChanceToStopWalking(chanceToStopWalking);
        targetActor->SetChanceToWalk(chanceToWalk);
        
        // Distance parameters
        
        float DistanceToAttackA = actorA->GetDistanceToAttack();
        float DistanceToFleeA   = actorA->GetDistanceToFlee();
        float DistanceToWalkA   = actorA->GetDistanceToWalk();
        
        float DistanceToAttackB = actorB->GetDistanceToAttack();
        float DistanceToFleeB   = actorB->GetDistanceToFlee();
        float DistanceToWalkB   = actorB->GetDistanceToWalk();
        
        float DistanceToAttack  = Float.Lerp(DistanceToAttackA, DistanceToAttackB, 0.5f);
        float DistanceToFlee    = Float.Lerp(DistanceToFleeA, DistanceToFleeB, 0.5f);
        float DistanceToWalk    = Float.Lerp(DistanceToWalkA, DistanceToWalkB, 0.5f);
        
        targetActor->SetDistanceToAttack(DistanceToAttack);
        targetActor->SetDistanceToFlee(DistanceToFlee);
        targetActor->SetDistanceToWalk(DistanceToWalk);
        
        // Increment generation
        unsigned int generation = actorA->GetGeneration();
        
        targetActor->SetGeneration( generation );
        
        continue;
    }
    
    return true;
}

void GeneticPresets::ClearGenes(Actor* actorPtr) {
    
    actorPtr->mGenes.clear();
    
    return;
}

void GeneticPresets::ExposeToRadiation(Actor* actorPtr, float radiationAmount) {
    
    float radiation = Random.Range(0.0f, radiationAmount) - Random.Range(0.0f, radiationAmount);
    
    unsigned int numberOfGenes = actorPtr->mGenes.size();
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene& gene = actorPtr->mGenes[i];
        
        gene.offset.x += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        gene.offset.y += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        gene.offset.z += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        
        gene.position.x += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.position.y += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.position.z += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        
        gene.rotation.x += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.rotation.y += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.rotation.z += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        
        gene.scale.x += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.scale.y += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        gene.scale.z += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100)) * 0.001f) * radiation;
        
        gene.color.x += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        gene.color.y += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        gene.color.z += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100)) * 0.0001f) * radiation;
        
    }
    
    return;
}

Gene GeneticPresets::Lerp(Gene geneA, Gene geneB, float bias) {
    
    Gene gene;
    
    gene.offset.x   = Float.Lerp(geneA.offset.x, geneB.offset.x, bias);
    gene.offset.y   = Float.Lerp(geneA.offset.y, geneB.offset.y, bias);
    gene.offset.z   = Float.Lerp(geneA.offset.z, geneB.offset.z, bias);
    
    gene.position.x = Float.Lerp(geneA.position.x, geneB.position.x, bias);
    gene.position.y = Float.Lerp(geneA.position.y, geneB.position.y, bias);
    gene.position.z = Float.Lerp(geneA.position.z, geneB.position.z, bias);
    
    gene.rotation.x = Float.Lerp(geneA.rotation.x, geneB.rotation.x, bias);
    gene.rotation.y = Float.Lerp(geneA.rotation.y, geneB.rotation.y, bias);
    gene.rotation.z = Float.Lerp(geneA.rotation.z, geneB.rotation.z, bias);
    
    gene.scale.x    = Float.Lerp(geneA.scale.x, geneB.scale.x, bias);
    gene.scale.y    = Float.Lerp(geneA.scale.y, geneB.scale.y, bias);
    gene.scale.z    = Float.Lerp(geneA.scale.z, geneB.scale.z, bias);
    
    gene.color.x    = Float.Lerp(geneA.color.x, geneB.color.x, bias);
    gene.color.y    = Float.Lerp(geneA.color.y, geneB.color.y, bias);
    gene.color.z    = Float.Lerp(geneA.color.z, geneB.color.z, bias);
    
    if ((geneA.doInverseAnimation) | (geneB.doInverseAnimation)) {
        gene.doInverseAnimation = true;
    } else {gene.doInverseAnimation = false;}
    
    if ((geneA.doAnimationCycle) | (geneB.doAnimationCycle)) {
        gene.doAnimationCycle = true;
    } else {gene.doAnimationCycle = false;}
    
    if (geneA.attachmentIndex != geneB.attachmentIndex) {
        
        if (Random.Range(0, 100) > 50) {
            gene.attachmentIndex = geneA.attachmentIndex;
        } else {
            gene.attachmentIndex = geneB.attachmentIndex;
        }
        
    }
    
    gene.animationRange = Float.Lerp(geneA.animationRange, geneB.animationRange, bias);
    
    gene.animationAxis.x = Float.Lerp(geneA.animationAxis.x, geneB.animationAxis.x, bias);
    gene.animationAxis.y = Float.Lerp(geneA.animationAxis.y, geneB.animationAxis.y, bias);
    gene.animationAxis.z = Float.Lerp(geneA.animationAxis.z, geneB.animationAxis.z, bias);
    
    if ((geneA.doExpress) & (geneB.doExpress)) {
        gene.doExpress = true;
    } else {
        gene.doExpress = false;
    }
    
    return gene;
}

// Mental models
//

void GeneticPresets::PsychologicalPresets::PreyBase(Actor* targetActor) {
    
    // Personality parameters
    targetActor->SetChanceToChangeDirection(80);
    targetActor->SetChanceToFocusOnActor(200);
    targetActor->SetChanceToStopWalking(100);
    targetActor->SetChanceToWalk(100);
    
    return;
}

void GeneticPresets::PsychologicalPresets::PredatorBase(Actor* targetActor) {
    
    // Personality parameters
    targetActor->SetChanceToChangeDirection(80);
    targetActor->SetChanceToFocusOnActor(200);
    targetActor->SetChanceToStopWalking(100);
    targetActor->SetChanceToWalk(100);
    
    return;
}


// Genomes
//

void GeneticPresets::ActorPresets::Sheep(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Sheep");
    
    targetActor->SetSpeed(0.7);
    targetActor->SetSpeedYouth(0.6);
    
    targetActor->SetYouthScale(0.2f);
    targetActor->SetAdultScale(0.8f);
    
    targetActor->SetHeightPreferenceMax(20.0f);
    
    AI.genomes.mental.PreyBase(targetActor);
    
    // Color variants
    Color headColor = Colors.dkgray;
    Color bodyColor = Colors.dkgray;
    Color limbColor = Colors.dkgray;
    
    headColor = Colors.Range(Colors.gray, Colors.dkgray) * (Random.Range(1, 100) * 0.008f);
    bodyColor = Colors.Range(Colors.gray, Colors.black)  * (Random.Range(1, 100) * 0.008f);
    limbColor = Colors.Range(Colors.gray, Colors.dkgray) * (Random.Range(1, 100) * 0.008f);
    
    bool selected = false;
    
    // Brown
    if ((!selected) & (Random.Range(0, 100) > 80)) {
        bodyColor = Colors.brown * 0.04f;
        selected = true;
    }
    
    // Black
    if ((!selected) & (Random.Range(0, 100) > 90)) {
        headColor = Colors.Range(Colors.gray, Colors.dkgray) * 0.08;
        bodyColor = Colors.black;
        limbColor = Colors.Range(Colors.gray, Colors.dkgray) * 0.05;
        selected = true;
    }
    
    // Rare - pink
    if ((!selected) & (Random.Range(0, 1000000) > 999990)) {
        bodyColor = Colors.purple * 0.7f;
        selected = true;
    }
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = BaseGene(0, 0, 0);
    geneBody.position  = BaseGene(0, 0.7, 0);
    geneBody.rotation  = BaseGene(0, 0, 0);
    geneBody.scale     = BaseGene(0.4, 0.4, 0.9);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = BaseGene(0, 1.02, 0.254);
    geneHead.position  = BaseGene(0, 0, 0.3);
    geneHead.rotation  = BaseGene(0, 0, 0);
    geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = BaseGene(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = BaseGene(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = BaseGene(0, -0.4, 0);
    geneLimbFrontRight.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = BaseGene(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = BaseGene(0, -0.4, 0);
    geneLimbRearLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbRearLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = BaseGene(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = BaseGene(0, -0.4, 0);
    geneLimbRearRight.rotation  = BaseGene(0, 0, 0);
    geneLimbRearRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->AddGene(geneBody);
    targetActor->AddGene(geneHead);
    
    targetActor->AddGene(geneLimbFrontLeft);
    targetActor->AddGene(geneLimbFrontRight);
    targetActor->AddGene(geneLimbRearLeft);
    targetActor->AddGene(geneLimbRearRight);
    
    return;
}



void GeneticPresets::ActorPresets::Bear(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Bear");
    
    targetActor->SetSpeed(1.1f);
    targetActor->SetSpeedYouth(0.9f);
    
    targetActor->SetYouthScale(0.4f);
    targetActor->SetAdultScale(1.3f);
    
    targetActor->SetHeightPreferenceMax(50.0f);
    targetActor->SetHeightPreferenceMin(20.0f);
    
    AI.genomes.mental.PredatorBase(targetActor);
    
    
    // Color variants
    bool selected = false;
    
    // Default brown bear
    Color headColor = Colors.brown;
    Color bodyColor = Colors.brown;
    Color limbColor = Colors.brown;
    
    headColor *= Colors.MakeGrayScale(0.02);
    bodyColor *= Colors.MakeGrayScale(0.018);
    limbColor *= Colors.MakeGrayScale(0.03);
    
    // Grizzly bear
    if ((!selected) & (Random.Range(0, 100) > 70)) {
        
        headColor = Colors.brown * Colors.MakeGrayScale(0.02);
        bodyColor = Colors.brown * Colors.MakeGrayScale(0.02);
        limbColor = Colors.brown * Colors.MakeGrayScale(0.03);
        
        AI.genomes.mental.PredatorBase(targetActor);
        
        selected = true;
    }
    
    // Black bear
    if ((!selected) & (Random.Range(0, 100) > 80)) {
        
        headColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.01);
        bodyColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.001);
        
        AI.genomes.mental.PredatorBase(targetActor);
        
        selected = true;
    }
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = BaseGene(0, 0, 0);
    geneBody.position  = BaseGene(0, 0.9, 0);
    geneBody.rotation  = BaseGene(0, 0, 0);
    geneBody.scale     = BaseGene(0.68, 0.65, 1.1);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Body abdomen
    Gene geneAbdomin;
    geneAbdomin.offset    = BaseGene(0, 0, 0);
    geneAbdomin.position  = BaseGene(0, 0.9, -0.3);
    geneAbdomin.rotation  = BaseGene(0, 0, 0);
    geneAbdomin.scale     = BaseGene(0.78, 0.75, 0.8);
    geneAbdomin.color.x   = bodyColor.r;
    geneAbdomin.color.y   = bodyColor.g;
    geneAbdomin.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = BaseGene(0, 1.05, 0.33);
    geneHead.position  = BaseGene(0, -0.1, 0.35);
    geneHead.rotation  = BaseGene(0, 0, 0);
    geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 2;
    geneMuzzle.offset    = BaseGene(0.0, 0.0, 0.0);
    geneMuzzle.position  = BaseGene(0.0, -0.14, 0.6);
    geneMuzzle.rotation  = BaseGene(0, 0, 0);
    geneMuzzle.scale     = BaseGene(0.25, 0.2, 0.2);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = BaseGene(0.0, 0.0, 0.0);
    geneEarLeft.position  = BaseGene(0.1, 0.13, 0.34);
    geneEarLeft.rotation  = BaseGene(0, 0, 0);
    geneEarLeft.scale     = BaseGene(0.1, 0.1, 0.1);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = BaseGene(0.0, 0.0, 0.0);
    geneEarRight.position  = BaseGene(-0.1, 0.13, 0.34);
    geneEarRight.rotation  = BaseGene(0, 0, 0);
    geneEarRight.scale     = BaseGene(0.1, 0.1, 0.1);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = BaseGene(0.02, -0.35, 0);
    geneLimbFrontLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontLeft.scale     = BaseGene(0.24, 0.5, 0.25);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = BaseGene(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = BaseGene(-0.02, -0.35, 0);
    geneLimbFrontRight.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontRight.scale     = BaseGene(0.24, 0.5, 0.25);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = BaseGene(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = BaseGene(0.07, -0.35, -0.04);
    geneLimbRearLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbRearLeft.scale     = BaseGene(0.25, 0.65, 0.35);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = BaseGene(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = BaseGene(-0.07, -0.35, -0.04);
    geneLimbRearRight.rotation  = BaseGene(0, 0, 0);
    geneLimbRearRight.scale     = BaseGene(0.25, 0.65, 0.35);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->AddGene(geneBody);
    targetActor->AddGene(geneHead);
    
    targetActor->AddGene(geneLimbFrontLeft);
    targetActor->AddGene(geneLimbFrontRight);
    targetActor->AddGene(geneLimbRearLeft);
    targetActor->AddGene(geneLimbRearRight);
    
    targetActor->AddGene(geneMuzzle);
    targetActor->AddGene(geneEarLeft);
    targetActor->AddGene(geneEarRight);
    
    targetActor->AddGene(geneAbdomin);
    
    return;
}


void GeneticPresets::ActorPresets::Dog(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Dog");
    
    targetActor->SetSpeed(1.0f);
    targetActor->SetSpeedYouth(0.8f);
    
    targetActor->SetYouthScale(0.3f);
    targetActor->SetAdultScale(1.0f);
    
    targetActor->SetHeightPreferenceMax(30.0f);
    targetActor->SetHeightPreferenceMin(10.0f);
    
    AI.genomes.mental.PredatorBase(targetActor);
    
    // Color variants
    Color headColor = Colors.gray;
    Color bodyColor = Colors.gray;
    Color limbColor = Colors.gray;
    Color collarColor = Colors.red;
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = BaseGene(0, 0, 0);
    geneBody.position  = BaseGene(0, 0.8, 0);
    geneBody.rotation  = BaseGene(0, 0, 0);
    geneBody.scale     = BaseGene(0.6, 0.6, 1.2);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = BaseGene(0, 1.0, 0.4);
    geneHead.position  = BaseGene(0, -0.1, 0.5);
    geneHead.rotation  = BaseGene(0, 0, 0);
    geneHead.scale     = BaseGene(0.3, 0.3, 0.3);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 2;
    geneMuzzle.offset    = BaseGene(0.0, 0.0, 0.0);
    geneMuzzle.position  = BaseGene(0.0, -0.1, 0.5);
    geneMuzzle.rotation  = BaseGene(0, 0, 0);
    geneMuzzle.scale     = BaseGene(0.2, 0.2, 0.2);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = BaseGene(0.0, 0.0, 0.0);
    geneEarLeft.position  = BaseGene(0.1, 0.2, 0.3);
    geneEarLeft.rotation  = BaseGene(0, 0, 0);
    geneEarLeft.scale     = BaseGene(0.1, 0.1, 0.1);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = BaseGene(0.0, 0.0, 0.0);
    geneEarRight.position  = BaseGene(-0.1, 0.2, 0.3);
    geneEarRight.rotation  = BaseGene(0, 0, 0);
    geneEarRight.scale     = BaseGene(0.1, 0.1, 0.1);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = BaseGene(0.2, 0.6, 0.5);
    geneLimbFrontLeft.position  = BaseGene(0.02, -0.4, 0);
    geneLimbFrontLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontLeft.scale     = BaseGene(0.2, 0.6, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = BaseGene(-0.2, 0.6, 0.5);
    geneLimbFrontRight.position  = BaseGene(-0.02, -0.4, 0);
    geneLimbFrontRight.rotation  = BaseGene(0, 0, 0);
    geneLimbFrontRight.scale     = BaseGene(0.2, 0.6, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = BaseGene(0.2, 0.6, -0.4);
    geneLimbRearLeft.position  = BaseGene(0.07, -0.4, -0.04);
    geneLimbRearLeft.rotation  = BaseGene(0, 0, 0);
    geneLimbRearLeft.scale     = BaseGene(0.2, 0.6, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = BaseGene(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = BaseGene(-0.2, 0.6, -0.4);
    geneLimbRearRight.position  = BaseGene(-0.07, -0.4, -0.04);
    geneLimbRearRight.rotation  = BaseGene(0, 0, 0);
    geneLimbRearRight.scale     = BaseGene(0.2, 0.6, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = BaseGene(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Collar gene
    Gene geneCollar;
    geneCollar.offset    = BaseGene(0, 1.0, 0);
    geneCollar.position  = BaseGene(0, 0, 0);
    geneCollar.rotation  = BaseGene(0, 0, 0);
    geneCollar.scale     = BaseGene(0.65, 0.1, 0.65);
    geneCollar.color.x   = collarColor.r;
    geneCollar.color.y   = collarColor.g;
    geneCollar.color.z   = collarColor.b;
    
    // Apply genes to the actor
    targetActor->AddGene(geneBody);
    targetActor->AddGene(geneHead);
    targetActor->AddGene(geneMuzzle);
    targetActor->AddGene(geneEarLeft);
    targetActor->AddGene(geneEarRight);
    targetActor->AddGene(geneLimbFrontLeft);
    targetActor->AddGene(geneLimbFrontRight);
    targetActor->AddGene(geneLimbRearLeft);
    targetActor->AddGene(geneLimbRearRight);
    targetActor->AddGene(geneCollar);
    
    return;
}


