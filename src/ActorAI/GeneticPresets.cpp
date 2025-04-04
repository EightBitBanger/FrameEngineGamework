#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>

extern NumberGeneration  Random;
extern ColorPreset       Colors;
extern FloatType         Float;
extern IntType           Int;
extern UIntType          UInt;
extern StringType        String;
extern RenderSystem      Renderer;
extern ActorSystem       AI;


std::string GeneticPresets::ExtractGenome(Actor* sourceActor) {
    std::string genetics;
    
    unsigned int numberOfGenes = sourceActor->genetics.GetNumberOfGenes();
    
    genetics += sourceActor->GetName() + ":";
    
    genetics += Float.ToString( sourceActor->physical.GetSpeed() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSpeedMultiplier() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSpeedYouth() ) + ":";
    
    genetics += Float.ToString( sourceActor->physical.GetAdultAge() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSeniorAge() ) + ":";
    
    genetics += Float.ToString( sourceActor->physical.GetYouthScale() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetAdultScale() ) + ":";
    
    // Personality
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToFocus() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToWalk() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToAttack() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToFlee() ) + ":";
    
    genetics += Float.ToString( sourceActor->behavior.GetHeightPreferenceMin() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetHeightPreferenceMax() ) + ":";
    
    // Characteristics
    genetics += UInt.ToString( (unsigned int)sourceActor->genetics.GetGeneration() ) + ":";
    
    if (sourceActor->physical.GetSexualOrientation() == true) 
        {genetics += "0:";} else {genetics += "1:";}
    
    genetics += Float.ToString( sourceActor->physical.GetAdultAge() ) + ":";
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene gene = sourceActor->genetics.GetGeneFromGenome( i );
        Phen phen = sourceActor->genetics.GetPhenFromPhenotype( i );
        
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
        
        genetics += Float.ToString( phen.scale.x ) + ",";
        genetics += Float.ToString( phen.scale.y ) + ",";
        genetics += Float.ToString( phen.scale.z ) + "|";
        
        genetics += Float.ToString( phen.color.x ) + ",";
        genetics += Float.ToString( phen.color.y ) + ",";
        genetics += Float.ToString( phen.color.z ) + "|";
        
        genetics += Float.ToString( gene.expressionFactor ) + ",";
        genetics += Float.ToString( gene.expressionMax ) + ",";
        genetics += UInt.ToString( gene.expressionBegin ) + ",";
        genetics += UInt.ToString( gene.expressionEnd ) + "|";
        
        genetics += Float.ToString( gene.doExpress ) + ",";
        genetics += Float.ToString( gene.doInverseAnimation ) + ",";
        genetics += Float.ToString( gene.doAnimationCycle ) + ",";
        genetics += UInt.ToString( gene.type ) + "|";
        
        continue;
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* targetActor, std::string genome) {
    std::vector<std::string> traits = String.Explode( genome, ':' );
    unsigned int numberOfTraits = traits.size();
    
    if (numberOfTraits > 7) {
        targetActor->SetName(traits[0]);
        
        targetActor->physical.SetSpeed(            String.ToFloat(traits[1]) );
        targetActor->physical.SetSpeedMultiplier(  String.ToFloat(traits[2]) );
        targetActor->physical.SetSpeedYouth(       String.ToFloat(traits[3]) );
        
        targetActor->physical.SetAdultAge(   String.ToFloat(traits[4]) );
        targetActor->physical.SetSeniorAge(  String.ToFloat(traits[5]) );
        
        targetActor->physical.SetYouthScale( String.ToFloat(traits[6]) );
        targetActor->physical.SetAdultScale( String.ToFloat(traits[7]) );
    }
    
    // Personality
    if (numberOfTraits > 13) {
        targetActor->behavior.SetDistanceToFocus(  String.ToFloat(traits[8]) );
        targetActor->behavior.SetDistanceToWalk(   String.ToFloat(traits[9]) );
        targetActor->behavior.SetDistanceToAttack( String.ToFloat(traits[10]) );
        targetActor->behavior.SetDistanceToFlee(   String.ToFloat(traits[11]) );
        
        targetActor->behavior.SetHeightPreferenceMin( String.ToFloat(traits[12]) );
        targetActor->behavior.SetHeightPreferenceMax( String.ToFloat(traits[13]) );
    }
    
    // Characteristics
    if (numberOfTraits > 16) {
        
        // Actor details
        targetActor->genetics.SetGeneration( String.ToUint(traits[14]) );
        
        // Sexuality
        if (String.ToUint(traits[15]) == 0) {
            targetActor->physical.SetSexualOrientation( true );  // Male
        } else {
            targetActor->physical.SetSexualOrientation( false ); // Female
        }
        
        // Adult age
        targetActor->physical.mAgeAdult = String.ToFloat(traits[16]);
        
    }
    
    // Extract genes from the genome string
    std::vector<std::string> genes = String.Explode( genome, '#' );
    
    unsigned int numberOfGenes = genes.size();
    for (unsigned int i=1; i < numberOfGenes; i++) {
        
        // Extract sub genes
        std::vector<std::string> subGenes = String.Explode( (genes[i]), '|' );
        std::vector<std::string> Codon;
        
        Gene gene;
        Phen phenotype;
        Bio biotype;
        
        Codon = String.Explode( subGenes[0], ',' );
        if (Codon.size() == 3) {
            gene.position.x = String.ToFloat( Codon[0] );
            gene.position.y = String.ToFloat( Codon[1] );
            gene.position.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[1], ',' );
        if (Codon.size() == 3) {
            gene.rotation.x = String.ToFloat( Codon[0] );
            gene.rotation.y = String.ToFloat( Codon[1] );
            gene.rotation.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[2], ',' );
        if (Codon.size() == 3) {
            gene.scale.x = String.ToFloat( Codon[0] );
            gene.scale.y = String.ToFloat( Codon[1] );
            gene.scale.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[3], ',' );
        if (Codon.size() == 3) {
            gene.offset.x = String.ToFloat( Codon[0] );
            gene.offset.y = String.ToFloat( Codon[1] );
            gene.offset.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[4], ',' );
        if (Codon.size() == 3) {
            gene.color.x = String.ToFloat( Codon[0] );
            gene.color.y = String.ToFloat( Codon[1] );
            gene.color.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[5], ',' );
        if (Codon.size() == 3) {
            phenotype.scale.x = String.ToFloat( Codon[0] );
            phenotype.scale.y = String.ToFloat( Codon[1] );
            phenotype.scale.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[6], ',' );
        if (Codon.size() == 3) {
            phenotype.color.x = String.ToFloat( Codon[0] );
            phenotype.color.y = String.ToFloat( Codon[1] );
            phenotype.color.z = String.ToFloat( Codon[2] );
        }
        
        Codon = String.Explode( subGenes[7], ',' );
        if (Codon.size() == 4) {
            gene.expressionFactor = String.ToFloat( Codon[0] );
            gene.expressionMax    = String.ToFloat( Codon[1] );
            gene.expressionBegin  = String.ToUint( Codon[2] );
            gene.expressionEnd    = String.ToUint( Codon[3] );
        }
        Codon = String.Explode( subGenes[8], ',' );
        if (Codon.size() == 4) {
            if (Codon[0] == "0") {gene.doExpress          = false;}
            if (Codon[1] == "1") {gene.doInverseAnimation = true;}
            if (Codon[2] == "1") {gene.doAnimationCycle   = true;}
            
            gene.type = String.ToUint( Codon[3] );
            
        }
        
        targetActor->genetics.mPhen.push_back(phenotype);
        targetActor->biological.mBiologics.push_back(biotype);
        targetActor->genetics.AddGene( gene );
    }
    
    return true;
}

bool GeneticPresets::BlendGenomes(Actor* parentA, Actor* parentB, Actor* offspring) {
    unsigned int numberOfGenesA = parentA->genetics.mGenes.size();
    unsigned int numberOfGenesB = parentB->genetics.mGenes.size();
    
    // Check genetic incompatibility
    if (numberOfGenesA != numberOfGenesB) 
        return false;
    
    offspring->genetics.mGenes.clear();
    
    for (unsigned int i=0; i < numberOfGenesA; i++) {
        
        float gradient = 0.0f;
        if (Random.Range(0, 100) > 50) 
            gradient = 1.0f;
        
        // Interpolate the genetic values
        Gene variant = Lerp(parentA->genetics.mGenes[i], parentB->genetics.mGenes[i], gradient);
        offspring->genetics.mGenes.push_back(variant);
        
        // Distance parameters
        
        float DistanceToFocusA  = parentA->behavior.GetDistanceToFocus();
        float DistanceToAttackA = parentA->behavior.GetDistanceToAttack();
        float DistanceToFleeA   = parentA->behavior.GetDistanceToFlee();
        float DistanceToWalkA   = parentA->behavior.GetDistanceToWalk();
        
        float DistanceToFocusB  = parentB->behavior.GetDistanceToFocus();
        float DistanceToAttackB = parentB->behavior.GetDistanceToAttack();
        float DistanceToFleeB   = parentB->behavior.GetDistanceToFlee();
        float DistanceToWalkB   = parentB->behavior.GetDistanceToWalk();
        
        float DistanceToFocus   = Float.Lerp(DistanceToFocusA,  DistanceToFocusB,   gradient);
        float DistanceToAttack  = Float.Lerp(DistanceToAttackA, DistanceToAttackB,  gradient);
        float DistanceToFlee    = Float.Lerp(DistanceToFleeA,   DistanceToFleeB,    gradient);
        float DistanceToWalk    = Float.Lerp(DistanceToWalkA,   DistanceToWalkB,    gradient);
        
        offspring->behavior.SetDistanceToFocus(DistanceToFocus);
        offspring->behavior.SetDistanceToAttack(DistanceToAttack);
        offspring->behavior.SetDistanceToFlee(DistanceToFlee);
        offspring->behavior.SetDistanceToWalk(DistanceToWalk);
        
        offspring->physical.mAgeAdult = Float.Lerp(parentA->physical.mAgeAdult, parentB->physical.mAgeAdult, gradient);
        
        // Increment generation
        unsigned int generation = parentA->genetics.GetGeneration();
        
        offspring->genetics.SetGeneration( generation );
        
        // Random new sexual orientation
        if (Random.Range(0, 100) > 60) {
            offspring->physical.SetSexualOrientation(true);  // Male
        } else {
            offspring->physical.SetSexualOrientation(false); // Female
        }
        
        continue;
    }
    
    // Process genetic index colors
    for (unsigned int w=0; w < numberOfGenesA; w++) {
        for (unsigned int i=0; i < numberOfGenesA; i++) {
            if (offspring->genetics.mGenes[i].colorIndex == 0) 
                continue;
            
            // Color index
            unsigned int geneParentIndex = offspring->genetics.mGenes[i].colorIndex - 1;
            offspring->genetics.mGenes[i].color.x = offspring->genetics.mGenes[geneParentIndex].color.x;
            offspring->genetics.mGenes[i].color.y = offspring->genetics.mGenes[geneParentIndex].color.y;
            offspring->genetics.mGenes[i].color.z = offspring->genetics.mGenes[geneParentIndex].color.z;
            
            continue;
        }
    }
    return true;
}

void GeneticPresets::ClearGenes(Actor* actorPtr) {
    
    actorPtr->genetics.mGenes.clear();
    
    return;
}

void GeneticPresets::ExposeToRadiation(Actor* actorPtr, float radiationAmount) {
    
    float radiation = Random.Range(0.0f, radiationAmount) - Random.Range(0.0f, radiationAmount);
    
    unsigned int numberOfGenes = actorPtr->genetics.mGenes.size();
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene& gene = actorPtr->genetics.mGenes[i];
        
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
    
    gene.position.x = Float.Lerp(geneA.position.x, geneB.position.x, bias);
    gene.position.y = Float.Lerp(geneA.position.y, geneB.position.y, bias);
    gene.position.z = Float.Lerp(geneA.position.z, geneB.position.z, bias);
    
    gene.rotation.x = Float.Lerp(geneA.rotation.x, geneB.rotation.x, bias);
    gene.rotation.y = Float.Lerp(geneA.rotation.y, geneB.rotation.y, bias);
    gene.rotation.z = Float.Lerp(geneA.rotation.z, geneB.rotation.z, bias);
    
    gene.scale.x = Float.Lerp(geneA.scale.x, geneB.scale.x, bias);
    gene.scale.y = Float.Lerp(geneA.scale.y, geneB.scale.y, bias);
    gene.scale.z = Float.Lerp(geneA.scale.z, geneB.scale.z, bias);
    
    gene.offset.x = Float.Lerp(geneA.offset.x, geneB.offset.x, bias);
    gene.offset.y = Float.Lerp(geneA.offset.y, geneB.offset.y, bias);
    gene.offset.z = Float.Lerp(geneA.offset.z, geneB.offset.z, bias);
    
    gene.color.x = Float.Lerp(geneA.color.x, geneB.color.x, bias);
    gene.color.y = Float.Lerp(geneA.color.y, geneB.color.y, bias);
    gene.color.z = Float.Lerp(geneA.color.z, geneB.color.z, bias);
    
    gene.expressionFactor = Float.Lerp(geneA.expressionFactor, geneB.expressionFactor, bias);
    gene.expressionMax    = Float.Lerp(geneA.expressionMax,    geneB.expressionMax, bias);
    gene.expressionBegin  = Float.Lerp(geneA.expressionBegin,  geneB.expressionBegin, bias);
    gene.expressionEnd    = Float.Lerp(geneA.expressionEnd,    geneB.expressionEnd, bias);
    
    gene.doInverseAnimation = geneA.doInverseAnimation || geneB.doInverseAnimation;
    gene.doAnimationCycle = geneA.doAnimationCycle || geneB.doAnimationCycle;
    gene.animationRange = Float.Lerp(geneA.animationRange, geneB.animationRange, bias);
    
    // Random attachment index
    if (Random.Range(0, 100) > 50) 
        {gene.attachmentIndex = geneA.attachmentIndex;} else 
        {gene.attachmentIndex = geneB.attachmentIndex;}
    
    if (Random.Range(0, 100) > 50) 
        {gene.colorIndex = geneA.colorIndex;} else 
        {gene.colorIndex = geneB.colorIndex;}
    if (Random.Range(0, 100) > 50) 
        {gene.scaleIndex = geneA.scaleIndex;} else 
        {gene.scaleIndex = geneB.scaleIndex;}
    if (Random.Range(0, 100) > 50) 
        {gene.type = geneA.type;} else 
        {gene.type = geneB.type;}
    
    gene.animationAxis.x = Float.Lerp(geneA.animationAxis.x, geneB.animationAxis.x, bias);
    gene.animationAxis.y = Float.Lerp(geneA.animationAxis.y, geneB.animationAxis.y, bias);
    gene.animationAxis.z = Float.Lerp(geneA.animationAxis.z, geneB.animationAxis.z, bias);
    
    gene.doExpress = geneA.doExpress || geneB.doExpress;
    
    return gene;
}

// Mental models
//

void GeneticPresets::PsychologicalPresets::PreyBase(Actor* targetActor) {
    
    
    
    return;
}

void GeneticPresets::PsychologicalPresets::PredatorBase(Actor* targetActor) {
    
    
    
    return;
}


// Genomes
//

void GeneticPresets::ActorPresets::Sheep(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Sheep");
    
    targetActor->physical.SetAge( 0 );
    targetActor->physical.SetAdultAge( 400 );
    targetActor->physical.SetSeniorAge( 4000 );
    
    targetActor->physical.SetSpeed(1.3);
    targetActor->physical.SetSpeedYouth(0.8);
    
    targetActor->physical.SetYouthScale(0.2f);
    targetActor->physical.SetAdultScale(0.8f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    AI.genomes.mental.PreyBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
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
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.7, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.4, 0.4, 0.9);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.02, 0.254);
    geneHead.position  = Codon(0, 0, 0.3);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.65, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    
    return;
}

void GeneticPresets::ActorPresets::Bovine(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Ox");
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(0.8);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    AI.genomes.mental.PreyBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor  = Colors.dkgray;
    Color bodyColor  = Colors.dkgray;
    Color limbColor  = Colors.dkgray;
    Color hornColor  = Colors.MakeGrayScale(0.037);
    Color udderColor = Colors.Lerp(Colors.MakeGrayScale(0.1), Colors.red, 0.2);
    
    headColor *= Colors.MakeRandomGrayScale();
    bodyColor *= Colors.MakeRandomGrayScale();
    limbColor *= Colors.MakeRandomGrayScale();
    udderColor *= Colors.MakeGrayScale(0.3);
    
    if (Random.Range(0, 100) > 50) headColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) bodyColor *= Colors.brown;
    if (Random.Range(0, 100) > 50) limbColor *= Colors.brown;
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.5, 0.5, 1.2);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.1, 0.45);
    geneHead.position  = Codon(0, 0, 0.35);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45, 0.42, 0.4);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Horns
    Gene geneHornLeft;
    geneHornLeft.attachmentIndex = 2;
    geneHornLeft.offset    = Codon(0, 0, 0);
    geneHornLeft.position  = Codon(0.25, 0.2, 0.22);
    geneHornLeft.rotation  = Codon(0, 0, -0.3);
    geneHornLeft.scale     = Codon(0.4, 0.1, 0.1);
    geneHornLeft.color.x   = hornColor.r;
    geneHornLeft.color.y   = hornColor.g;
    geneHornLeft.color.z   = hornColor.b;
    geneHornLeft.type = EXPRESSION_TYPE_MALE;
    geneHornLeft.expressionFactor = 1.0f;
    
    Gene geneHornLeftUpper;
    geneHornLeftUpper.attachmentIndex = 2;
    geneHornLeftUpper.offset    = Codon(0, 0, 0);
    geneHornLeftUpper.position  = Codon(0.5, 0.2, 0.22);
    geneHornLeftUpper.rotation  = Codon(0, 0, 0.75);
    geneHornLeftUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornLeftUpper.color.x   = hornColor.r;
    geneHornLeftUpper.color.y   = hornColor.g;
    geneHornLeftUpper.color.z   = hornColor.b;
    geneHornLeftUpper.type = EXPRESSION_TYPE_MALE;
    geneHornLeftUpper.expressionFactor = 1.0f;
    
    Gene geneHornRight;
    geneHornRight.attachmentIndex = 2;
    geneHornRight.offset    = Codon(0, 0, 0);
    geneHornRight.position  = Codon(-0.25, 0.2, 0.22);
    geneHornRight.rotation  = Codon(0, 0, 0.3);
    geneHornRight.scale     = Codon(0.4, 0.1, 0.1);
    geneHornRight.color.x   = hornColor.r;
    geneHornRight.color.y   = hornColor.g;
    geneHornRight.color.z   = hornColor.b;
    geneHornRight.type = EXPRESSION_TYPE_MALE;
    geneHornRight.expressionFactor = 1.0f;
    
    Gene geneHornRightUpper;
    geneHornRightUpper.attachmentIndex = 2;
    geneHornRightUpper.offset    = Codon(0, 0, 0);
    geneHornRightUpper.position  = Codon(-0.5, 0.2, 0.22);
    geneHornRightUpper.rotation  = Codon(0, 0, -0.75);
    geneHornRightUpper.scale     = Codon(0.2, 0.1, 0.1);
    geneHornRightUpper.color.x   = hornColor.r;
    geneHornRightUpper.color.y   = hornColor.g;
    geneHornRightUpper.color.z   = hornColor.b;
    geneHornRightUpper.type = EXPRESSION_TYPE_MALE;
    geneHornRightUpper.expressionFactor = 1.0f;
    
    // Udders
    Gene geneUdders;
    geneUdders.offset    = Codon(0, 0, 0);
    geneUdders.position  = Codon(0, 0.68, -0.3);
    geneUdders.rotation  = Codon(0, 0, 0);
    geneUdders.scale     = Codon(0.2, 0.3, 0.24);
    geneUdders.color.x   = udderColor.r;
    geneUdders.color.y   = udderColor.g;
    geneUdders.color.z   = udderColor.b;
    geneUdders.type = EXPRESSION_TYPE_FEMALE;
    geneUdders.expressionFactor = 1.0f;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 10;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 10;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22, 0.75, -0.45);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 10;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22, 0.75, -0.45);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Tail
    Gene geneTail;
    geneTail.offset    = Codon(0, 0.85, -0.60);
    geneTail.position  = Codon(0, 0, 0);
    geneTail.rotation  = Codon(0, 0, 0);
    geneTail.scale     = Codon(0.1, 0.52, 0.1);
    geneTail.color.x   = bodyColor.r;
    geneTail.color.y   = bodyColor.g;
    geneTail.color.z   = bodyColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneHornLeft);
    targetActor->genetics.AddGene(geneHornLeftUpper);
    targetActor->genetics.AddGene(geneHornRight);
    targetActor->genetics.AddGene(geneHornRightUpper);
    targetActor->genetics.AddGene(geneUdders);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
    return;
}


void GeneticPresets::ActorPresets::Horse(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Horse");
    
    targetActor->physical.SetSpeed(1.5);
    targetActor->physical.SetSpeedYouth(0.9);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.2f);
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    
    AI.genomes.mental.PreyBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
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
    
    if (Random.Range(0, 100) > 30) limbFRColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbFLColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbRRColor *= Colors.brown;
    if (Random.Range(0, 100) > 30) limbRLColor *= Colors.brown;
    
    limbFRColor *= Colors.MakeRandomGrayScale();
    limbFLColor *= Colors.MakeRandomGrayScale();
    limbRRColor *= Colors.MakeRandomGrayScale();
    limbRLColor *= Colors.MakeRandomGrayScale();
    
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.black; limbFLColor = Colors.black;
        limbRRColor = Colors.black; limbRLColor = Colors.black;
    }
    if (Random.Range(0, 100) > 95) {
        limbFRColor = Colors.white; limbFLColor = Colors.white;
        limbRRColor = Colors.white; limbRLColor = Colors.white;
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
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
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
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
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
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
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
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
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
    
    return;
}



void GeneticPresets::ActorPresets::Bear(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Bear");
    
    targetActor->physical.SetSpeed(1.1f);
    targetActor->physical.SetSpeedYouth(0.9f);
    
    targetActor->physical.SetYouthScale(0.4f);
    targetActor->physical.SetAdultScale(1.3f);
    
    targetActor->behavior.SetHeightPreferenceMax(50.0f);
    targetActor->behavior.SetHeightPreferenceMin(20.0f);
    
    AI.genomes.mental.PredatorBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
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
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.9, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.68, 0.65, 1.1);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Body abdomen
    Gene geneAbdomin;
    geneAbdomin.offset    = Codon(0, 0, 0);
    geneAbdomin.position  = Codon(0, 0.9, -0.3);
    geneAbdomin.rotation  = Codon(0, 0, 0);
    geneAbdomin.scale     = Codon(0.78, 0.75, 0.8);
    geneAbdomin.color.x   = bodyColor.r;
    geneAbdomin.color.y   = bodyColor.g;
    geneAbdomin.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.05, 0.33);
    geneHead.position  = Codon(0, -0.1, 0.35);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.415, 0.395, 0.415);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 2;
    geneMuzzle.offset    = Codon(0.0, 0.0, 0.0);
    geneMuzzle.position  = Codon(0.0, -0.14, 0.6);
    geneMuzzle.rotation  = Codon(0, 0, 0);
    geneMuzzle.scale     = Codon(0.25, 0.2, 0.2);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = Codon(0.0, 0.0, 0.0);
    geneEarLeft.position  = Codon(0.1, 0.13, 0.34);
    geneEarLeft.rotation  = Codon(0, 0, 0);
    geneEarLeft.scale     = Codon(0.1, 0.1, 0.1);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = Codon(0.0, 0.0, 0.0);
    geneEarRight.position  = Codon(-0.1, 0.13, 0.34);
    geneEarRight.rotation  = Codon(0, 0, 0);
    geneEarRight.scale     = Codon(0.1, 0.1, 0.1);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.17, 0.75, 0.4);
    geneLimbFrontLeft.position  = Codon(0.02, -0.35, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.24, 0.5, 0.25);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.17, 0.75, 0.4);
    geneLimbFrontRight.position  = Codon(-0.02, -0.35, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.24, 0.5, 0.25);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.17, 0.75, -0.4);
    geneLimbRearLeft.position  = Codon(0.07, -0.35, -0.04);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.25, 0.65, 0.35);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.17, 0.75, -0.4);
    geneLimbRearRight.position  = Codon(-0.07, -0.35, -0.04);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.25, 0.65, 0.35);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneMuzzle);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneAbdomin);
    
    return;
}


void GeneticPresets::ActorPresets::Dog(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Dog");
    
    targetActor->physical.SetSpeed(1.0f);
    targetActor->physical.SetSpeedYouth(0.8f);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(10.0f);
    
    AI.genomes.mental.PredatorBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.gray;
    Color bodyColor = Colors.gray;
    Color limbColor = Colors.gray;
    Color collarColor = Colors.red;
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.8, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.6, 0.6, 1.2);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 1.0, 0.4);
    geneHead.position  = Codon(0, -0.1, 0.5);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.3, 0.3, 0.3);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 2;
    geneMuzzle.offset    = Codon(0.0, 0.0, 0.0);
    geneMuzzle.position  = Codon(0.0, -0.1, 0.5);
    geneMuzzle.rotation  = Codon(0, 0, 0);
    geneMuzzle.scale     = Codon(0.2, 0.2, 0.2);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 2;
    geneEarLeft.offset    = Codon(0.0, 0.0, 0.0);
    geneEarLeft.position  = Codon(0.1, 0.2, 0.3);
    geneEarLeft.rotation  = Codon(0, 0, 0);
    geneEarLeft.scale     = Codon(0.1, 0.1, 0.1);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 2;
    geneEarRight.offset    = Codon(0.0, 0.0, 0.0);
    geneEarRight.position  = Codon(-0.1, 0.2, 0.3);
    geneEarRight.rotation  = Codon(0, 0, 0);
    geneEarRight.scale     = Codon(0.1, 0.1, 0.1);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.2, 0.6, 0.5);
    geneLimbFrontLeft.position  = Codon(0.02, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.6, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.2, 0.6, 0.5);
    geneLimbFrontRight.position  = Codon(-0.02, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.6, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.2, 0.6, -0.4);
    geneLimbRearLeft.position  = Codon(0.07, -0.4, -0.04);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.6, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.2, 0.6, -0.4);
    geneLimbRearRight.position  = Codon(-0.07, -0.4, -0.04);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.6, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Collar gene
    Gene geneCollar;
    geneCollar.offset    = Codon(0, 1.0, 0);
    geneCollar.position  = Codon(0, 0, 0);
    geneCollar.rotation  = Codon(0, 0, 0);
    geneCollar.scale     = Codon(0.65, 0.1, 0.65);
    geneCollar.color.x   = collarColor.r;
    geneCollar.color.y   = collarColor.g;
    geneCollar.color.z   = collarColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneMuzzle);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneCollar);
    
    return;
}


void GeneticPresets::ActorPresets::Creepy(Actor* targetActor) {
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Creepy");
    
    targetActor->physical.SetSpeed(1.0);
    targetActor->physical.SetSpeedYouth(0.8);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(20.0f);
    
    AI.genomes.mental.PreyBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.green;
    Color bodyColor = Colors.green;
    Color limbColor = Colors.green;
    
    headColor *= Colors.MakeRandomGrayScale() * 0.3;
    bodyColor *= Colors.MakeRandomGrayScale() * 0.3;
    limbColor *= Colors.MakeRandomGrayScale() * 0.3;
    
    // Body
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, 0);
    geneBody.position  = Codon(0, 0.8, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.4, 0.8, 0.4);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Head
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 1.45, 0);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.45, 0.45, 0.45);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Front Left Leg
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.22, 0.6, 0.22);
    geneLimbFrontLeft.position  = Codon(0, -0.4, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 10;
    
    // Front Right Leg
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.22, 0.6, 0.22);
    geneLimbFrontRight.position  = Codon(0, -0.4, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 10;
    
    // Rear Left Leg
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.22, 0.6, -0.22);
    geneLimbRearLeft.position  = Codon(0, -0.4, 0);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 10;
    
    // Rear Right Leg
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.22, 0.6, -0.22);
    geneLimbRearRight.position  = Codon(0, -0.4, 0);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.2, 0.7, 0.2);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 10;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    
    return;
}

