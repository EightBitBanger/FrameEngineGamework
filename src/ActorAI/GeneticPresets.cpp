#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>

extern NumberGeneration  Random;
extern ColorPreset       Colors;
extern FloatType         Float;
extern StringType        String;


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
    
    genetics += "#";
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene gene = actorSource->GetGeneFromGenome( i );
        
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
        
        genetics += "#";
        
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


// Mental models
//

void GeneticPresets::PreyBase(Actor* targetActor) {
    
    // Personality parameters
    targetActor->SetChanceToChangeDirection(80);
    targetActor->SetChanceToFocusOnActor(200);
    targetActor->SetChanceToStopWalking(100);
    targetActor->SetChanceToWalk(100);
    
    return;
}



// Genomes
//

void GeneticPresets::Sheep(Actor* targetActor) {
    
    // Identification
    targetActor->SetName("Sheep");
    
    targetActor->SetSpeed(0.7);
    
    targetActor->SetYouthScale(0.1f);
    targetActor->SetAdultScale(0.8f);
    
    
    // Mental model
    PreyBase(targetActor);
    
    // Color variant
    Color headColor;
    Color bodyColor;
    Color limbColor;
    
    headColor = Colors.MakeRandomGrayScale() * Colors.ltgray;
    bodyColor = Colors.MakeRandomGrayScale() * Colors.dkgray;
    limbColor = Colors.MakeRandomGrayScale() * Colors.gray;
    
    // Random off colors
    
    // Brown
    if (Random.Range(0, 100) > 90) 
        bodyColor *= Colors.brown;
    
    // Grays
    if (Random.Range(0, 100) > 90) bodyColor *= Colors.dkgray;
    if (Random.Range(0, 100) > 90) bodyColor *= Colors.ltgray;
    
    if (Random.Range(0, 1000) > 990) bodyColor = Colors.purple;
    
    
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



void GeneticPresets::Bear(Actor* targetActor) {
    
    // Identification
    targetActor->SetName("Bear");
    
    targetActor->SetSpeed(1.1);
    
    targetActor->SetYouthScale(0.4f);
    targetActor->SetAdultScale(1.3f);
    
    // Color variant
    Color headColor;
    Color bodyColor;
    Color limbColor;
    
    // Random off colors
    
    // Brown bears
    if (Random.Range(0, 100) > 40) {
        
        headColor *= Colors.dkgray;
        bodyColor *= Colors.dkgray;
        limbColor *= Colors.brown * Colors.dkgray;
        
        PreyBase(targetActor);
        
    }
    
    // Grizzle grizzle
    if (Random.Range(0, 100) > 70) {
        
        headColor = Colors.brown * Colors.MakeGrayScale(0.02);
        bodyColor = Colors.brown * Colors.MakeGrayScale(0.018);
        limbColor = Colors.brown * Colors.MakeGrayScale(0.03);
        
        PreyBase(targetActor);
        
    }
    
    // Black bear
    if (Random.Range(0, 100) > 80) {
        
        headColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.01);
        bodyColor = Colors.dkgray * Colors.dkgray * Colors.MakeGrayScale(0.018);
        
        PreyBase(targetActor);
        
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


