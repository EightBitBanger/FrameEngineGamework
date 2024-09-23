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
    
    // Extract genetics
    std::string genetics;
    
    unsigned int numberOfGenes = actorSource->GetNumberOfGenes();
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
        genetics += Float.ToString( gene.doAnimationCycle ) + "|#";
        
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* actorSource, std::string genome) {
    
    // Extract genes from the genome
    std::vector<std::string> genes = String.Explode( genome, '#' );
    
    unsigned int numberOfGenes = genes.size();
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
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

void GeneticPresets::SheepGene(Actor* targetActor) {
    
    // Identification
    targetActor->SetName("Sheep");
    
    targetActor->SetSpeed(0.7);
    
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
    if (Random.Range(0, 10) > 8) 
        bodyColor *= Colors.brown;
    
    // Grays
    if (Random.Range(0, 10) > 9) bodyColor *= Colors.dkgray;
    if (Random.Range(0, 10) > 9) bodyColor *= Colors.ltgray;
    
    if (Random.Range(0, 100) > 97) bodyColor = Colors.purple;
    
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = BaseGene(0, 0, 0);
    geneBody.position  = BaseGene(0, 0.7, 0);
    geneBody.scale     = BaseGene(0.4, 0.4, 0.9);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = BaseGene(0, 1.02, 0.254);
    geneHead.position  = BaseGene(0, 0, 0.3);
    geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
    geneHead.color     = BaseGene(0.4, 0.4, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = BaseGene(0, -0.4, 0);
    geneLimbFrontLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontLeft.color     = BaseGene(0.4, 0.4, 0.4);
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
    geneLimbFrontRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbFrontRight.color     = BaseGene(0.4, 0.4, 0.4);
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
    geneLimbRearLeft.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearLeft.color     = BaseGene(0.4, 0.4, 0.4);
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
    geneLimbRearRight.scale     = BaseGene(0.2, 0.65, 0.2);
    geneLimbRearRight.color     = BaseGene(0.4, 0.4, 0.4);
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

