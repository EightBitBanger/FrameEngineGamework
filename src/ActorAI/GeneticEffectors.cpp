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

#include <cstdio>
#include <cstdlib>

// Small helpers to avoid tons of temporary std::string allocations.
namespace
{
    inline void AppendFloat(std::string& out, float value, char separator)
    {
        char buffer[32];
        int len = std::snprintf(buffer, sizeof(buffer), "%.6g", value);
        if (len > 0)
            out.append(buffer, static_cast<std::size_t>(len));
        if (separator != '\0')
            out.push_back(separator);
    }

    inline void AppendUInt(std::string& out, unsigned int value, char separator)
    {
        char buffer[16];
        int len = std::snprintf(buffer, sizeof(buffer), "%u", value);
        if (len > 0)
            out.append(buffer, static_cast<std::size_t>(len));
        if (separator != '\0')
            out.push_back(separator);
    }

    // Parse "x,y,z" into three floats
    inline bool ParseFloat3(const std::string& s, float& x, float& y, float& z)
    {
        const char* p   = s.c_str();
        char*       end = 0;

        x = std::strtof(p, &end);
        if (end == p || *end != ',')
            return false;

        p = end + 1;
        y = std::strtof(p, &end);
        if (end == p || *end != ',')
            return false;

        p = end + 1;
        z = std::strtof(p, &end);
        if (end == p)
            return false;

        return true;
    }

    // Parse "f1,f2,u32"
    inline bool ParseExpressionTriple(const std::string& s,
                                      float& factor,
                                      float& maximum,
                                      unsigned int& age)
    {
        const char* p   = s.c_str();
        char*       end = 0;

        factor = std::strtof(p, &end);
        if (end == p || *end != ',')
            return false;

        p = end + 1;
        maximum = std::strtof(p, &end);
        if (end == p || *end != ',')
            return false;

        p = end + 1;
        age = static_cast<unsigned int>(std::strtoul(p, &end, 10));
        if (end == p)
            return false;

        return true;
    }

    // Parse "doExpress,animType,doInverse,type"
    inline void ParseFlagsQuad(const std::string& s, Gene& gene)
    {
        const char* p = s.c_str();

        // doExpress: keep old behavior – only explicitly clear when "0"
        if (*p == '0')
            gene.doExpress = false;

        // Skip to next field
        while (*p != '\0' && *p != ',')
            ++p;
        if (*p == ',')
            ++p;

        // animationType encoded as '0','1','2'
        char anim = *p;
        if (anim == '0')
            gene.animationType = ActorState::Animation::Body;
        else if (anim == '1')
            gene.animationType = ActorState::Animation::Head;
        else if (anim == '2')
            gene.animationType = ActorState::Animation::Limb;

        while (*p != '\0' && *p != ',')
            ++p;
        if (*p == ',')
            ++p;

        // doInverseAnimation: only explicitly set true when "1"
        if (*p == '1')
            gene.doInverseAnimation = true;

        while (*p != '\0' && *p != ',')
            ++p;
        if (*p == ',')
            ++p;

        // type: rest of the string
        char* end = 0;
        gene.type = static_cast<unsigned int>(std::strtoul(p, &end, 10));
    }
}

std::string GeneticPresets::ExtractGenome(Actor* sourceActor) {
    unsigned int numberOfGenes = sourceActor->genetics.GetNumberOfGenes();
    
    // Reserve a decent chunk to avoid repeated reallocations
    // Rough guess: base ~256 chars + ~160 chars per gene
    std::string genetics;
    genetics.reserve(256u + numberOfGenes * 160u);
    
    // Name
    genetics += sourceActor->GetName();
    genetics.push_back(':');
    
    // Physical
    AppendFloat(genetics, sourceActor->physical.GetSpeed(),            ':');
    AppendFloat(genetics, sourceActor->physical.GetSpeedMultiplier(),  ':');
    AppendFloat(genetics, sourceActor->physical.GetSpeedYouth(),       ':');
    
    AppendFloat(genetics, sourceActor->physical.GetAdultAge(),         ':');
    AppendFloat(genetics, sourceActor->physical.GetSeniorAge(),        ':');
    
    AppendFloat(genetics, sourceActor->physical.GetYouthScale(),       ':');
    AppendFloat(genetics, sourceActor->physical.GetAdultScale(),       ':');
    
    // Biological
    AppendFloat(genetics, sourceActor->biological.health,              ':');
    AppendFloat(genetics, sourceActor->biological.hunger,              ':');
    AppendFloat(genetics, sourceActor->biological.strength,            ':');
    AppendFloat(genetics, sourceActor->biological.defense,             ':');
    
    // Personality
    AppendFloat(genetics, sourceActor->behavior.GetPredatorState(),    ':');
    AppendFloat(genetics, sourceActor->behavior.GetPreyState(),        ':');
    
    AppendFloat(genetics, sourceActor->behavior.GetDistanceToFocus(),  ':');
    AppendFloat(genetics, sourceActor->behavior.GetDistanceToWalk(),   ':');
    AppendFloat(genetics, sourceActor->behavior.GetDistanceToAttack(), ':');
    AppendFloat(genetics, sourceActor->behavior.GetDistanceToFlee(),   ':');
    AppendFloat(genetics, sourceActor->behavior.GetDistanceToInflict(),':');
    
    AppendUInt (genetics, sourceActor->behavior.GetCooldownAttack(),   ':');
    AppendUInt (genetics, sourceActor->behavior.GetCooldownObserve(),  ':');
    AppendUInt (genetics, sourceActor->behavior.GetCooldownMove(),     ':');
    
    AppendFloat(genetics, sourceActor->behavior.GetHeightPreferenceMin(), ':');
    AppendFloat(genetics, sourceActor->behavior.GetHeightPreferenceMax(), ':');
    
    // Characteristics
    AppendUInt(genetics, static_cast<unsigned int>(sourceActor->genetics.GetGeneration()), ':');
    
    if (sourceActor->physical.GetSexualOrientation() == true)
        genetics += "0:";
    else
        genetics += "1:";
    
    AppendFloat(genetics, sourceActor->physical.GetAdultAge(), ':');
    
    // Genes
    for (unsigned int i = 0; i < numberOfGenes; ++i) {
        Gene gene = sourceActor->genetics.GetGeneFromGenome(i);
        
        // Check invalid gene
        if (gene.color.x == -1.0f)
            continue;
        
        Phen phen = sourceActor->genetics.GetPhenFromPhenotype(i);
        
        // Check invalid phen
        if (phen.color.x == -1.0f)
            continue;
        
        genetics.push_back('#');
        
        // position
        AppendFloat(genetics, gene.position.x, ',');
        AppendFloat(genetics, gene.position.y, ',');
        AppendFloat(genetics, gene.position.z, '\0');
        genetics.push_back('|');
        
        // rotation
        AppendFloat(genetics, gene.rotation.x, ',');
        AppendFloat(genetics, gene.rotation.y, ',');
        AppendFloat(genetics, gene.rotation.z, '\0');
        genetics.push_back('|');
        
        // scale
        AppendFloat(genetics, gene.scale.x, ',');
        AppendFloat(genetics, gene.scale.y, ',');
        AppendFloat(genetics, gene.scale.z, '\0');
        genetics.push_back('|');
        
        // offset
        AppendFloat(genetics, gene.offset.x, ',');
        AppendFloat(genetics, gene.offset.y, ',');
        AppendFloat(genetics, gene.offset.z, '\0');
        genetics.push_back('|');
        
        // color
        AppendFloat(genetics, gene.color.x, ',');
        AppendFloat(genetics, gene.color.y, ',');
        AppendFloat(genetics, gene.color.z, '\0');
        genetics.push_back('|');
        
        // animationAxis
        AppendFloat(genetics, gene.animationAxis.x, ',');
        AppendFloat(genetics, gene.animationAxis.y, ',');
        AppendFloat(genetics, gene.animationAxis.z, '\0');
        genetics.push_back('|');
        
        // phenotype scale
        AppendFloat(genetics, phen.scale.x, ',');
        AppendFloat(genetics, phen.scale.y, ',');
        AppendFloat(genetics, phen.scale.z, '\0');
        genetics.push_back('|');
        
        // phenotype color
        AppendFloat(genetics, phen.color.x, ',');
        AppendFloat(genetics, phen.color.y, ',');
        AppendFloat(genetics, phen.color.z, '\0');
        genetics.push_back('|');
        
        // expressionFactor, expressionMax, expressionAge
        AppendFloat(genetics, gene.expressionFactor, ',');
        AppendFloat(genetics, gene.expressionMax,    ',');
        AppendUInt (genetics, gene.expressionAge,    '\0');
        genetics.push_back('|');
        
        // doExpress, animationType, doInverseAnimation, type
        AppendUInt(genetics, gene.doExpress ? 1u : 0u, ',');
        unsigned int animCode = 0u;
        if (gene.animationType == ActorState::Animation::Head)
            animCode = 1u;
        else if (gene.animationType == ActorState::Animation::Limb)
            animCode = 2u;
        
        AppendUInt(genetics, animCode, ',');
        AppendUInt(genetics, gene.doInverseAnimation ? 1u : 0u, ',');
        AppendUInt(genetics, gene.type, '\0');
        genetics.push_back('|');
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* targetActor, const std::string& genome) {
    std::vector<std::string> traits = String.Explode(genome, ':');
    unsigned int numberOfTraits = static_cast<unsigned int>(traits.size());
    
    // Physical
    if (numberOfTraits > 7) {
        targetActor->SetName(traits[0]);
        
        targetActor->physical.SetSpeed(           String.ToFloat(traits[1]) );
        targetActor->physical.SetSpeedMultiplier( String.ToFloat(traits[2]) );
        targetActor->physical.SetSpeedYouth(      String.ToFloat(traits[3]) );
        
        targetActor->physical.SetAdultAge(  String.ToFloat(traits[4]) );
        targetActor->physical.SetSeniorAge( String.ToFloat(traits[5]) );
        
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
        
        targetActor->behavior.SetCooldownAttack(      String.ToUint(traits[19]) );
        targetActor->behavior.SetCooldownObserve(     String.ToUint(traits[20]) );
        targetActor->behavior.SetCooldownMove(        String.ToUint(traits[21]) );
        
        targetActor->behavior.SetHeightPreferenceMin( String.ToFloat(traits[22]) );
        targetActor->behavior.SetHeightPreferenceMax( String.ToFloat(traits[23]) );
    }
    
    // Characteristics
    if (numberOfTraits > 26) {
        targetActor->genetics.SetGeneration(String.ToUint(traits[24]));
        
        if (String.ToUint(traits[25]) == 0)
            targetActor->physical.SetSexualOrientation(true);   // Male
        else
            targetActor->physical.SetSexualOrientation(false);  // Female
        
        targetActor->physical.mAgeAdult = String.ToFloat(traits[26]);
    }
    
    // Extract genes from the genome string
    std::vector<std::string> genes = String.Explode(genome, '#');
    
    unsigned int numberOfGenes = static_cast<unsigned int>(genes.size());
    for (unsigned int i = 1; i < numberOfGenes; ++i) {
        // Extract sub genes
        std::vector<std::string> subGenes = String.Explode(genes[i], '|');
        
        // Check invalid gene
        if (subGenes.size() != 10)
            continue;
        
        Gene gene;
        Phen phenotype;
        Bio  biotype;
        
        // Genotype
        if (!ParseFloat3(subGenes[0], gene.position.x,      gene.position.y,      gene.position.z))
            continue;
        if (!ParseFloat3(subGenes[1], gene.rotation.x,      gene.rotation.y,      gene.rotation.z))
            continue;
        if (!ParseFloat3(subGenes[2], gene.scale.x,         gene.scale.y,         gene.scale.z))
            continue;
        if (!ParseFloat3(subGenes[3], gene.offset.x,        gene.offset.y,        gene.offset.z))
            continue;
        if (!ParseFloat3(subGenes[4], gene.color.x,         gene.color.y,         gene.color.z))
            continue;
        if (!ParseFloat3(subGenes[5], gene.animationAxis.x, gene.animationAxis.y, gene.animationAxis.z))
            continue;
        
        // Phenotype
        if (!ParseFloat3(subGenes[6], phenotype.scale.x,    phenotype.scale.y,    phenotype.scale.z))
            continue;
        if (!ParseFloat3(subGenes[7], phenotype.color.x,    phenotype.color.y,    phenotype.color.z))
            continue;
        
        // Expression triple
        if (!ParseExpressionTriple(subGenes[8],
                                gene.expressionFactor,
                                gene.expressionMax,
                                gene.expressionAge))
            continue;
        
        // Flags: doExpress, animationType, doInverseAnimation, type
        ParseFlagsQuad(subGenes[9], gene);
        
        targetActor->genetics.mPhen.push_back(phenotype);
        targetActor->biological.mBiologics.push_back(biotype);
        targetActor->genetics.AddGene(gene);
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

