#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Serialization/Serialization.h>

extern Serialization     Serializer;
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
    
    // Physical
    genetics += Float.ToString( sourceActor->physical.GetSpeed() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSpeedMultiplier() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSpeedYouth() ) + ":";
    
    genetics += Float.ToString( sourceActor->physical.GetAdultAge() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetSeniorAge() ) + ":";
    
    genetics += Float.ToString( sourceActor->physical.GetYouthScale() ) + ":";
    genetics += Float.ToString( sourceActor->physical.GetAdultScale() ) + ":";
    
    // Biological
    genetics += Float.ToString( sourceActor->biological.health ) + ":";
    genetics += Float.ToString( sourceActor->biological.hunger ) + ":";
    genetics += Float.ToString( sourceActor->biological.strength ) + ":";
    genetics += Float.ToString( sourceActor->biological.defense ) + ":";
    
    // Personality
    genetics += Float.ToString( sourceActor->behavior.GetPredatorState() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetPreyState() ) + ":";
    
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToFocus() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToWalk() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToAttack() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToFlee() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetDistanceToInflict() ) + ":";
    
    genetics += UInt.ToString( sourceActor->behavior.GetCooldownAttack() ) + ":";
    genetics += UInt.ToString( sourceActor->behavior.GetCooldownObserve() ) + ":";
    genetics += UInt.ToString( sourceActor->behavior.GetCooldownMove() ) + ":";
    
    genetics += Float.ToString( sourceActor->behavior.GetHeightPreferenceMin() ) + ":";
    genetics += Float.ToString( sourceActor->behavior.GetHeightPreferenceMax() ) + ":";
    
    // Characteristics
    genetics += UInt.ToString( (unsigned int)sourceActor->genetics.GetGeneration() ) + ":";
    
    if (sourceActor->physical.GetSexualOrientation() == true) 
        {genetics += "0:";} else {genetics += "1:";}
    
    genetics += Float.ToString( sourceActor->physical.GetAdultAge() ) + ":";
    
    for (unsigned int i=0; i < numberOfGenes; i++) {
        
        Gene gene = sourceActor->genetics.GetGeneFromGenome( i );
        
        // Check invalid gene
        if (gene.color.x == -1.0f) 
            continue;
        
        Phen phen = sourceActor->genetics.GetPhenFromPhenotype( i );
        
        // Check invalid phen
        if (phen.color.x == -1.0f) 
            continue;
        
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
        
        genetics += Float.ToString( gene.animationAxis.x ) + ",";
        genetics += Float.ToString( gene.animationAxis.y ) + ",";
        genetics += Float.ToString( gene.animationAxis.z ) + "|";
        
        genetics += Float.ToString( phen.scale.x ) + ",";
        genetics += Float.ToString( phen.scale.y ) + ",";
        genetics += Float.ToString( phen.scale.z ) + "|";
        
        genetics += Float.ToString( phen.color.x ) + ",";
        genetics += Float.ToString( phen.color.y ) + ",";
        genetics += Float.ToString( phen.color.z ) + "|";
        
        genetics += Float.ToString( gene.expressionFactor ) + ",";
        genetics += Float.ToString( gene.expressionMax ) + ",";
        genetics += UInt.ToString(  gene.expressionAge ) + "|";
        
        genetics += Float.ToString( gene.doExpress ) + ",";
        if (gene.animationType == ActorState::Animation::Body) genetics += "0,";
        if (gene.animationType == ActorState::Animation::Head) genetics += "1,";
        if (gene.animationType == ActorState::Animation::Limb) genetics += "2,";
        genetics += Float.ToString( gene.doInverseAnimation ) + ",";
        
        genetics += UInt.ToString( gene.type ) + "|";
        
        continue;
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* targetActor, std::string genome) {
    std::vector<std::string> traits = String.Explode( genome, ':' );
    unsigned int numberOfTraits = traits.size();
    
    // Physical
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
    
    // Biological
    if (numberOfTraits > 11) {
        targetActor->biological.health   = String.ToFloat(traits[8]);
        targetActor->biological.hunger   = String.ToFloat(traits[9]);
        
        targetActor->biological.strength = String.ToFloat(traits[10]);
        targetActor->biological.defense  = String.ToFloat(traits[11]);
    }
    
    // Personality
    if (numberOfTraits > 23) {
        targetActor->behavior.SetPredatorState(       String.ToFloat(traits[12]) );
        targetActor->behavior.SetPreyState(           String.ToFloat(traits[13]) );
        
        targetActor->behavior.SetDistanceToFocus(     String.ToFloat(traits[14]) );
        targetActor->behavior.SetDistanceToWalk(      String.ToFloat(traits[15]) );
        targetActor->behavior.SetDistanceToAttack(    String.ToFloat(traits[16]) );
        targetActor->behavior.SetDistanceToFlee(      String.ToFloat(traits[17]) );
        targetActor->behavior.SetDistanceToInflict(   String.ToFloat(traits[18]) );
        
        targetActor->behavior.SetCooldownAttack(      String.ToUint( traits[19]) );
        targetActor->behavior.SetCooldownObserve(     String.ToUint( traits[20]) );
        targetActor->behavior.SetCooldownMove(        String.ToUint( traits[21]) );
        
        targetActor->behavior.SetHeightPreferenceMin( String.ToFloat(traits[22]) );
        targetActor->behavior.SetHeightPreferenceMax( String.ToFloat(traits[23]) );
    }
    
    // Characteristics
    if (numberOfTraits > 26) {
        
        // Actor details
        targetActor->genetics.SetGeneration( String.ToUint(traits[24]) );
        
        // Sexuality
        if (String.ToUint(traits[25]) == 0) {
            targetActor->physical.SetSexualOrientation( true );  // Male
        } else {
            targetActor->physical.SetSexualOrientation( false ); // Female
        }
        
        // Adult age
        targetActor->physical.mAgeAdult = String.ToFloat(traits[26]);
        
    }
    
    // Extract genes from the genome string
    std::vector<std::string> genes = String.Explode( genome, '#' );
    
    unsigned int numberOfGenes = genes.size();
    for (unsigned int i=1; i < numberOfGenes; i++) {
        
        // Extract sub genes
        std::vector<std::string> subGenes = String.Explode( (genes[i]), '|' );
        
        // Check invalid gene
        if (subGenes.size() != 10) 
            continue;
        
        std::vector<std::string> Codon;
        
        Gene gene;
        Phen phenotype;
        Bio biotype;
        
        // Genotype
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
            gene.animationAxis.x = String.ToFloat( Codon[0] );
            gene.animationAxis.y = String.ToFloat( Codon[1] );
            gene.animationAxis.z = String.ToFloat( Codon[2] );
        }
        
        // Phenotype
        Codon = String.Explode( subGenes[6], ',' );
        if (Codon.size() == 3) {
            phenotype.scale.x = String.ToFloat( Codon[0] );
            phenotype.scale.y = String.ToFloat( Codon[1] );
            phenotype.scale.z = String.ToFloat( Codon[2] );
        }
        Codon = String.Explode( subGenes[7], ',' );
        if (Codon.size() == 3) {
            phenotype.color.x = String.ToFloat( Codon[0] );
            phenotype.color.y = String.ToFloat( Codon[1] );
            phenotype.color.z = String.ToFloat( Codon[2] );
        }
        
        Codon = String.Explode( subGenes[8], ',' );
        if (Codon.size() == 3) {
            gene.expressionFactor = String.ToFloat( Codon[0] );
            gene.expressionMax    = String.ToFloat( Codon[1] );
            gene.expressionAge    = String.ToUint(  Codon[2] );
        }
        Codon = String.Explode( subGenes[9], ',' );
        if (Codon.size() == 4) {
            if (Codon[0] == "0") {gene.doExpress = false;}
            if (Codon[1] == "0") {gene.animationType = ActorState::Animation::Body;}
            if (Codon[1] == "1") {gene.animationType = ActorState::Animation::Head;}
            if (Codon[1] == "2") {gene.animationType = ActorState::Animation::Limb;}
            if (Codon[2] == "1") {gene.doInverseAnimation = true;}
            gene.type = String.ToUint( Codon[3] );
        }
        
        targetActor->genetics.mPhen.push_back(phenotype);
        targetActor->biological.mBiologics.push_back(biotype);
        targetActor->genetics.AddGene( gene );
    }
    
    return true;
}

bool GeneticPresets::BlendGenomes(Actor* parentA, Actor* parentB, Actor* offspring) {
    // Check genetic incompatibility
    unsigned int numberOfGenesA = parentA->genetics.mGenes.size();
    unsigned int numberOfGenesB = parentB->genetics.mGenes.size();
    if (numberOfGenesA != numberOfGenesB) 
        return false;
    if (parentA->GetName() != parentB->GetName()) 
        return false;
    offspring->SetName(parentA->GetName());
    
    float gradient = 0.0f;
    if (Random.Range(0, 100) > 50) 
        gradient = 1.0f;
    
    // Distance parameters
    float DistanceToFocusA  = parentA->behavior.GetDistanceToFocus();
    float DistanceToAttackA = parentA->behavior.GetDistanceToAttack();
    float DistanceToFleeA   = parentA->behavior.GetDistanceToFlee();
    float DistanceToWalkA   = parentA->behavior.GetDistanceToWalk();
    float HeightPrefMaxA    = parentA->behavior.GetHeightPreferenceMax();
    float HeightPrefMinA    = parentA->behavior.GetHeightPreferenceMin();
    
    float DistanceToFocusB  = parentB->behavior.GetDistanceToFocus();
    float DistanceToAttackB = parentB->behavior.GetDistanceToAttack();
    float DistanceToFleeB   = parentB->behavior.GetDistanceToFlee();
    float DistanceToWalkB   = parentB->behavior.GetDistanceToWalk();
    float HeightPrefMaxB    = parentB->behavior.GetHeightPreferenceMax();
    float HeightPrefMinB    = parentB->behavior.GetHeightPreferenceMin();
    
    float DistanceToFocus   = Float.Lerp(DistanceToFocusA,  DistanceToFocusB,   gradient);
    float DistanceToAttack  = Float.Lerp(DistanceToAttackA, DistanceToAttackB,  gradient);
    float DistanceToFlee    = Float.Lerp(DistanceToFleeA,   DistanceToFleeB,    gradient);
    float DistanceToWalk    = Float.Lerp(DistanceToWalkA,   DistanceToWalkB,    gradient);
    float HeightPrefMax     = Float.Lerp(HeightPrefMaxA,    HeightPrefMaxB,     gradient);
    float HeightPrefMin     = Float.Lerp(HeightPrefMinA,    HeightPrefMinB,     gradient);
    
    offspring->behavior.SetDistanceToFocus(DistanceToFocus);
    offspring->behavior.SetDistanceToAttack(DistanceToAttack);
    offspring->behavior.SetDistanceToFlee(DistanceToFlee);
    offspring->behavior.SetDistanceToWalk(DistanceToWalk);
    offspring->behavior.SetHeightPreferenceMax(HeightPrefMax);
    offspring->behavior.SetHeightPreferenceMin(HeightPrefMin);
    
    // Physical
    offspring->physical.mAgeAdult    = Float.Lerp(parentA->physical.mAgeAdult,    parentB->physical.mAgeAdult, gradient);
    offspring->physical.mAgeSenior   = Float.Lerp(parentA->physical.mAgeSenior,   parentB->physical.mAgeSenior, gradient);
    
    offspring->physical.mSpeed       = Float.Lerp(parentA->physical.mSpeed,       parentB->physical.mSpeed, gradient);
    offspring->physical.mSnapSpeed   = Float.Lerp(parentA->physical.mSnapSpeed,   parentB->physical.mSnapSpeed, gradient);
    offspring->physical.mSpeedMul    = Float.Lerp(parentA->physical.mSpeedMul,    parentB->physical.mSpeedMul, gradient);
    offspring->physical.mSpeedYouth  = Float.Lerp(parentA->physical.mSpeedYouth,  parentB->physical.mSpeedYouth, gradient);
    offspring->physical.mYouthScale  = Float.Lerp(parentA->physical.mYouthScale,  parentB->physical.mYouthScale, gradient);
    offspring->physical.mAdultScale  = Float.Lerp(parentA->physical.mAdultScale,  parentB->physical.mAdultScale, gradient);
    
    
    // Increment generation
    unsigned int generation = parentA->genetics.GetGeneration();
    
    offspring->genetics.SetGeneration( generation );
    
    // Random new sexual orientation
    if (Random.Range(0, 100) > 60) {
        offspring->physical.SetSexualOrientation(true);  // Male
    } else {
        offspring->physical.SetSexualOrientation(false); // Female
    }
    
    // Inheritance bias
    offspring->genetics.mGenes.clear();
    for (unsigned int i=0; i < numberOfGenesA; i++) {
        if (Random.Range(0, 100) > 50) 
            {gradient = 0.0f;} else {gradient = 1.0f;}
        
        Gene geneticVariant = Lerp(parentA->genetics.mGenes[i], 
                                   parentB->genetics.mGenes[i], gradient);
        offspring->genetics.mGenes.push_back(geneticVariant);
    }
    
    // Parent-indexed inheritance for color and scale.
    // - Index 0 = no inheritance
    // - Index k>0 = inherit from gene (k-1)
    // - Multiple passes allow chained references to settle
    // - Invalid/self references are nulled (set to 0) to avoid OOB or no-op loops
    unsigned int numberOfGenes = offspring->genetics.mGenes.size();
    for (unsigned int pass = 0; pass < numberOfGenes; ++pass) {
        bool wasChanged = false;
        
        for (unsigned int i = 0; i < numberOfGenes; ++i) {
            Gene& gene = offspring->genetics.mGenes[i];
            
            // Color
            if (gene.colorIndex > 0) {
                unsigned int j = gene.colorIndex - 1; // 1-based -> 0-based
                if (j < numberOfGenes && j != i) {
                    Gene& parent = offspring->genetics.mGenes[j];
                    // Copy parent color
                    if (gene.color.x != parent.color.x || 
                        gene.color.y != parent.color.y || 
                        gene.color.z != parent.color.z) {
                        gene.color.x = parent.color.x;
                        gene.color.y = parent.color.y;
                        gene.color.z = parent.color.z;
                        wasChanged = true;
                    }
                } else {
                    // Bad reference -> disable inheritance
                    gene.colorIndex = 0;
                }
            }
            
            // Scale
            if (gene.scaleIndex > 0) {
                unsigned int j = gene.scaleIndex - 1; // 1-based -> 0-based
                if (j < numberOfGenes && j != i) {
                    Gene& parent = offspring->genetics.mGenes[j];
                    // Copy parent scale (genotype scale)
                    if (gene.scale.x != parent.scale.x || 
                        gene.scale.y != parent.scale.y || 
                        gene.scale.z != parent.scale.z) {
                        gene.scale.x = parent.scale.x;
                        gene.scale.y = parent.scale.y;
                        gene.scale.z = parent.scale.z;
                        wasChanged = true;
                    }
                } else {
                    // Bad reference -> disable inheritance
                    gene.scaleIndex = 0;
                }
            }
        }
        
        if (!wasChanged) break; // done early if nothing changed on this pass
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
    gene.expressionAge    = Float.Lerp(geneA.expressionAge,    geneB.expressionAge, bias);
    
    if (geneA.doInverseAnimation || geneB.doInverseAnimation) 
    {gene.doInverseAnimation = true;} else {gene.doInverseAnimation = false;}
    
    if (geneA.doExpress || geneB.doExpress) 
    {gene.doExpress = true;} else {gene.doExpress = false;}
    
    gene.animationRange = Float.Lerp(geneA.animationRange, geneB.animationRange, bias);
    gene.animationType = (ActorState::Animation)Int.Lerp((int)geneA.animationType, (int)geneB.animationType, bias);
    
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
    
    return gene;
}

