#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Serialization/Serialization.h>

#include <sstream>

extern Serialization     Serializer;
extern NumberGeneration  Random;
extern ColorPreset       Colors;
extern FloatType         Float;
extern IntType           Int;
extern UIntType          UInt;
extern StringType        String;
extern RenderSystem      Renderer;
extern ActorSystem       AI;

inline void AppendFloat(std::string& out, float value, char separator) {
    char buffer[32];
    int len = std::snprintf(buffer, sizeof(buffer), "%.6g", value);
    if (len > 0)
        out.append(buffer, static_cast<std::size_t>(len));
    if (separator != '\0')
        out.push_back(separator);
}

inline void AppendUInt(std::string& out, unsigned int value, char separator) {
    char buffer[16];
    int len = std::snprintf(buffer, sizeof(buffer), "%u", value);
    if (len > 0)
        out.append(buffer, static_cast<std::size_t>(len));
    if (separator != '\0')
        out.push_back(separator);
}

inline bool ParseFloat3(const std::string& s, float& x, float& y, float& z) {
    const char* p   = s.c_str();
    char* end = 0;
    
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

inline bool ParseExpressionTriple(const std::string& s, float& factor, float& maximum, unsigned int& age) {
    const char* p   = s.c_str();
    char* end = 0;
    
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

inline void ParseFlags(const std::string& s, Gene& gene) {
    const char* p = s.c_str();
    char* end = nullptr;
    
    // Reset to safe defaults before parsing
    gene.doExpress           = true;
    gene.doInverseAnimation  = false;
    gene.animationType       = ActorState::Animation::Body;
    gene.form                = ActorState::Genetic::Base;
    gene.attachmentIndex     = 0;
    gene.colorIndex          = 0;
    gene.scaleIndex          = 0;
    gene.animationRange      = 0.0f;
    
    // DoExpress
    unsigned long doExpress = std::strtoul(p, &end, 10);
    if (end == p) return;
    gene.doExpress = (doExpress != 0);
    if (*end != ',') return;
    p = end + 1;
    
    // AnimationType (Handles LimbHolding state seamlessly)
    unsigned long anim = std::strtoul(p, &end, 10);
    if (end == p) return;
    if (anim == 1)      gene.animationType = ActorState::Animation::Head;
    else if (anim == 2) gene.animationType = ActorState::Animation::Limb;
    else if (anim == 3) gene.animationType = ActorState::Animation::LimbHolding;
    else                gene.animationType = ActorState::Animation::Body;
    if (*end != ',') return;
    p = end + 1;
    
    // DoInverseAnimation
    unsigned long doInverse = std::strtoul(p, &end, 10);
    if (end == p) return;
    gene.doInverseAnimation = (doInverse != 0);
    if (*end != ',') return;
    p = end + 1;
    
    // Expression form
    unsigned long geneExp = std::strtoul(p, &end, 10);
    if (end == p) return;
    if (geneExp == 1)      gene.form = ActorState::Genetic::Male;
    else if (geneExp == 2) gene.form = ActorState::Genetic::Female;
    else                   gene.form = ActorState::Genetic::Base;
    if (*end != ',') return;
    p = end + 1;
    
    // AttachmentIndex
    gene.attachmentIndex = static_cast<unsigned int>(std::strtoul(p, &end, 10));
    if (end == p || *end != ',') return;
    p = end + 1;
    
    // ColorIndex
    gene.colorIndex = static_cast<unsigned int>(std::strtoul(p, &end, 10));
    if (end == p || *end != ',') return;
    p = end + 1;
    
    // ScaleIndex
    gene.scaleIndex = static_cast<unsigned int>(std::strtoul(p, &end, 10));
    if (end == p || *end != ',') return;
    p = end + 1;
    
    // AnimationRange
    gene.animationRange = std::strtof(p, &end);
}

std::string GeneticPresets::ExtractGenome(Actor* sourceActor) {
    unsigned int numberOfGenes = sourceActor->genetics.GetNumberOfGenes();
    
    std::string genetics;
    genetics.reserve(256u + numberOfGenes * 200u);
    
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
    AppendUInt (genetics, sourceActor->behavior.GetCooldownSocial(),   ':');
    AppendUInt (genetics, sourceActor->behavior.GetCooldownMove(),     ':');
    AppendUInt (genetics, sourceActor->behavior.GetCooldownBreed(),    ':');
    
    AppendFloat(genetics, sourceActor->behavior.GetHeightPreferenceMin(), ':');
    AppendFloat(genetics, sourceActor->behavior.GetHeightPreferenceMax(), ':');
    
    // Biological
    AppendFloat(genetics, sourceActor->biological.healthMax, ':');
    
    // Animation
    AppendFloat(genetics, sourceActor->animation.mWalkRate, ':');
    
    // Circadian rhythm
    AppendFloat(genetics, sourceActor->sleep.mPreferredSleepStart, ':');
    AppendFloat(genetics, sourceActor->sleep.mPreferredDuration, ':');
    
    // Characteristics
    AppendUInt(genetics, static_cast<unsigned int>(sourceActor->genetics.GetGeneration()), ':');
    
    // Sexual Orientation (0 = Male, 1 = Female) - end traits section without trailing colon
    if (sourceActor->physical.GetSexualOrientation() == true)
        genetics += "0";
    else
        genetics += "1";
    
    // Genes (# delimited section)
    for (unsigned int i = 0; i < numberOfGenes; ++i) {
        Gene gene = sourceActor->genetics.GetGeneFromGenome(i);
        
        if (gene.color.x == -1.0f)
            continue;
        
        Phen phen = sourceActor->genetics.GetPhenFromPhenotype(i);
        
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
        
        AppendUInt(genetics, gene.doExpress ? 1u : 0u, ',');
        AppendUInt(genetics, to_underlying(gene.animationType), ',');
        AppendUInt(genetics, gene.doInverseAnimation ? 1u : 0u, ',');
        AppendUInt(genetics, to_underlying(gene.form), ',');
        
        AppendUInt(genetics, gene.attachmentIndex, ',');
        AppendUInt(genetics, gene.colorIndex, ',');
        AppendUInt(genetics, gene.scaleIndex, ',');
        AppendFloat(genetics, gene.animationRange, '\0');
        genetics.push_back('|');
    }
    
    return genetics;
}

bool GeneticPresets::InjectGenome(Actor* targetActor, const std::string& genome) {
    targetActor->genetics.mGenes.clear();
    targetActor->genetics.mPhen.clear();
    targetActor->biological.mBiologics.clear();
    
    std::size_t firstHash = genome.find('#');
    std::string traitsSection = (firstHash != std::string::npos) ? genome.substr(0, firstHash) : genome;
    
    // Stream reader helpers for cleaner sequential trait extraction
    std::stringstream ss(traitsSection);
    std::string token;
    
    auto nextString = [&ss, &token]() -> bool {
        return static_cast<bool>(std::getline(ss, token, ':'));
    };
    auto nextFloat = [&]() -> float { return nextString() ? String.ToFloat(token) : 0.0f; };
    auto nextUint  = [&]() -> unsigned int { return nextString() ? String.ToUint(token) : 0; };
    
    // Name
    if (nextString()) targetActor->SetName(token);
    
    // Physical
    targetActor->physical.SetSpeed(nextFloat());
    targetActor->physical.SetSpeedMultiplier(nextFloat());
    targetActor->physical.SetSpeedYouth(nextFloat());
    
    targetActor->physical.SetAdultAge(nextFloat());
    targetActor->physical.SetSeniorAge(nextFloat());
    
    targetActor->physical.SetYouthScale(nextFloat());
    targetActor->physical.SetAdultScale(nextFloat());
    
    // Behavior & Personality
    targetActor->behavior.SetPredatorState(nextFloat());
    targetActor->behavior.SetPreyState(nextFloat());
    
    targetActor->behavior.SetDistanceToFocus(nextFloat());
    targetActor->behavior.SetDistanceToWalk(nextFloat());
    targetActor->behavior.SetDistanceToAttack(nextFloat());
    targetActor->behavior.SetDistanceToFlee(nextFloat());
    targetActor->behavior.SetDistanceToInflict(nextFloat());
    
    targetActor->behavior.SetCooldownAttack(nextUint());
    targetActor->behavior.SetCooldownObserve(nextUint());
    targetActor->behavior.SetCooldownSocial(nextUint());
    targetActor->behavior.SetCooldownMove(nextUint());
    targetActor->behavior.SetCooldownBreed(nextUint());
    
    targetActor->behavior.SetHeightPreferenceMin(nextFloat());
    targetActor->behavior.SetHeightPreferenceMax(nextFloat());
    
    // Biological
    targetActor->biological.healthMax = nextFloat();
    
    // Animation
    targetActor->animation.mWalkRate = nextFloat();
    
    // Circadian rhythm
    targetActor->sleep.mPreferredSleepStart  = nextFloat();
    targetActor->sleep.mPreferredDuration    = nextFloat();
    
    // Characteristics
    targetActor->genetics.SetGeneration(nextUint());
    targetActor->physical.SetSexualOrientation(nextUint() == 0); // 0 = Male (true), 1 = Female (false)
    
    // Gene Extraction
    std::vector<std::string> genes = String.Explode(genome, '#');
    unsigned int numberOfGenes = static_cast<unsigned int>(genes.size());
    
    for (unsigned int i = 1; i < numberOfGenes; ++i) {
        std::vector<std::string> subGenes = String.Explode(genes[i], '|');
        
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
        
        // Flags
        ParseFlags(subGenes[9], gene);
        
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
    
    // =========================================================================
    // Behavior & Personality
    
    float DistanceToFocusA   = parentA->behavior.GetDistanceToFocus();
    float DistanceToAttackA  = parentA->behavior.GetDistanceToAttack();
    float DistanceToFleeA    = parentA->behavior.GetDistanceToFlee();
    float DistanceToWalkA    = parentA->behavior.GetDistanceToWalk();
    float DistanceToInflictA = parentA->behavior.GetDistanceToInflict();
    float HeightPrefMaxA     = parentA->behavior.GetHeightPreferenceMax();
    float HeightPrefMinA     = parentA->behavior.GetHeightPreferenceMin();
    
    float DistanceToFocusB   = parentB->behavior.GetDistanceToFocus();
    float DistanceToAttackB  = parentB->behavior.GetDistanceToAttack();
    float DistanceToFleeB    = parentB->behavior.GetDistanceToFlee();
    float DistanceToWalkB    = parentB->behavior.GetDistanceToWalk();
    float DistanceToInflictB = parentB->behavior.GetDistanceToInflict();
    float HeightPrefMaxB     = parentB->behavior.GetHeightPreferenceMax();
    float HeightPrefMinB     = parentB->behavior.GetHeightPreferenceMin();
    
    offspring->behavior.SetDistanceToFocus  (Float.Lerp(DistanceToFocusA,   DistanceToFocusB,   gradient));
    offspring->behavior.SetDistanceToAttack (Float.Lerp(DistanceToAttackA,  DistanceToAttackB,  gradient));
    offspring->behavior.SetDistanceToFlee   (Float.Lerp(DistanceToFleeA,    DistanceToFleeB,    gradient));
    offspring->behavior.SetDistanceToWalk   (Float.Lerp(DistanceToWalkA,    DistanceToWalkB,    gradient));
    offspring->behavior.SetDistanceToInflict(Float.Lerp(DistanceToInflictA, DistanceToInflictB, gradient));
    offspring->behavior.SetHeightPreferenceMax(Float.Lerp(HeightPrefMaxA,   HeightPrefMaxB,     gradient));
    offspring->behavior.SetHeightPreferenceMin(Float.Lerp(HeightPrefMinA,   HeightPrefMinB,     gradient));
    
    // Predator / Prey state
    offspring->behavior.SetPredatorState(gradient > 0.5f ? parentB->behavior.GetPredatorState() : parentA->behavior.GetPredatorState());
    offspring->behavior.SetPreyState    (gradient > 0.5f ? parentB->behavior.GetPreyState()     : parentA->behavior.GetPreyState());
    
    // Behavioral Cooldowns
    offspring->behavior.SetCooldownAttack (static_cast<unsigned int>(Float.Lerp(parentA->behavior.GetCooldownAttack(),  parentB->behavior.GetCooldownAttack(),  gradient)));
    offspring->behavior.SetCooldownObserve(static_cast<unsigned int>(Float.Lerp(parentA->behavior.GetCooldownObserve(), parentB->behavior.GetCooldownObserve(), gradient)));
    offspring->behavior.SetCooldownSocial (static_cast<unsigned int>(Float.Lerp(parentA->behavior.GetCooldownSocial(),  parentB->behavior.GetCooldownSocial(),  gradient)));
    offspring->behavior.SetCooldownMove   (static_cast<unsigned int>(Float.Lerp(parentA->behavior.GetCooldownMove(),    parentB->behavior.GetCooldownMove(),    gradient)));
    offspring->behavior.SetCooldownBreed  (static_cast<unsigned int>(Float.Lerp(parentA->behavior.GetCooldownBreed(),   parentB->behavior.GetCooldownBreed(),   gradient)));
    
    // =========================================================================
    // Circadian Rhythm
    
    float preferredSleepStart = Float.Lerp(parentA->sleep.GetPreferredSleepStart(), parentB->sleep.GetPreferredSleepStart(), gradient);
    float preferredDuration   = Float.Lerp(parentA->sleep.GetPreferredDuration(),   parentB->sleep.GetPreferredDuration(),   gradient);
    float entrainmentRate     = Float.Lerp(parentA->sleep.GetEntrainmentRate(),     parentB->sleep.GetEntrainmentRate(),     gradient);
    
    offspring->sleep.SetPreferredSleepStart(preferredSleepStart);
    offspring->sleep.SetPreferredDuration(preferredDuration);
    offspring->sleep.SetEntrainmentRate(entrainmentRate);
    
    // Initialize starting state to the inherited baseline schedule
    offspring->sleep.SetCurrentSleepStart(preferredSleepStart);
    offspring->sleep.SetCurrentDuration(preferredDuration);
    
    // =========================================================================
    // Animation & Biological
    
    offspring->animation.mWalkRate   = Float.Lerp(parentA->animation.mWalkRate, parentB->animation.mWalkRate, gradient);
    
    offspring->biological.healthMax  = Float.Lerp(parentA->biological.healthMax,  parentB->biological.healthMax,  gradient);
    offspring->biological.health     = offspring->biological.healthMax;
    offspring->biological.strength   = Float.Lerp(parentA->biological.strength,   parentB->biological.strength,   gradient);
    offspring->biological.defense    = Float.Lerp(parentA->biological.defense,    parentB->biological.defense,    gradient);
    
    // =========================================================================
    // Physical Attributes
    
    offspring->physical.mAgeAdult    = Float.Lerp(parentA->physical.mAgeAdult,    parentB->physical.mAgeAdult,    gradient);
    offspring->physical.mAgeSenior   = Float.Lerp(parentA->physical.mAgeSenior,   parentB->physical.mAgeSenior,   gradient);
    
    offspring->physical.mSpeed       = Float.Lerp(parentA->physical.mSpeed,       parentB->physical.mSpeed,       gradient);
    offspring->physical.mSnapSpeed   = Float.Lerp(parentA->physical.mSnapSpeed,   parentB->physical.mSnapSpeed,   gradient);
    offspring->physical.mSpeedMul    = Float.Lerp(parentA->physical.mSpeedMul,    parentB->physical.mSpeedMul,    gradient);
    offspring->physical.mSpeedYouth  = Float.Lerp(parentA->physical.mSpeedYouth,  parentB->physical.mSpeedYouth,  gradient);
    offspring->physical.mYouthScale  = Float.Lerp(parentA->physical.mYouthScale,  parentB->physical.mYouthScale,  gradient);
    offspring->physical.mAdultScale  = Float.Lerp(parentA->physical.mAdultScale,  parentB->physical.mAdultScale,  gradient);
    
    // Increment generation based on the maximum parent generation
    unsigned int maxParentGen = std::max(parentA->genetics.GetGeneration(), parentB->genetics.GetGeneration()) + 1;
    offspring->genetics.SetGeneration(maxParentGen);
    
    // Random new sexual orientation
    if (Random.Range(0, 100) > 60) {
        offspring->physical.SetSexualOrientation(true);  // Male
    } else {
        offspring->physical.SetSexualOrientation(false); // Female
    }
    
    // Inheritance bias
    offspring->genetics.mGenes.clear();
    std::vector<float> geneGradients(numberOfGenesA, 0.0f);
    
    for (unsigned int i = 0; i < numberOfGenesA; i++) {
        float geneGradient = (Random.Range(0, 100) > 50) ? 0.0f : 1.0f;
        
        // If this gene links to an earlier gene via scaleIndex, inherit from the same parent
        unsigned int scaleIndex = parentA->genetics.mGenes[i].scaleIndex;
        if (scaleIndex > 0) {
            unsigned int parentGeneIndex = scaleIndex - 1;
            if (parentGeneIndex < i) {
                geneGradient = geneGradients[parentGeneIndex];
            }
        }
        
        geneGradients[i] = geneGradient;
        
        Gene geneticVariant = Lerp(parentA->genetics.mGenes[i], 
                                parentB->genetics.mGenes[i], geneGradient);
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
                unsigned int j = gene.colorIndex - 1;
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
                    // Bad reference: disable inheritance
                    gene.colorIndex = 0;
                }
            }
            
            // Scale
            if (gene.scaleIndex > 0) {
                unsigned int j = gene.scaleIndex - 1; // 1-based -> 0-based
                if (j < numberOfGenes && j != i) {
                    Gene& parent = offspring->genetics.mGenes[j];
                    // Copy parent scale (genotype scale) and expression attributes
                    if (gene.scale.x != parent.scale.x || 
                        gene.scale.y != parent.scale.y || 
                        gene.scale.z != parent.scale.z ||
                        gene.expressionFactor != parent.expressionFactor ||
                        gene.expressionMax != parent.expressionMax ||
                        gene.expressionAge != parent.expressionAge) {
                        gene.scale.x = parent.scale.x;
                        gene.scale.y = parent.scale.y;
                        gene.scale.z = parent.scale.z;
                        gene.expressionFactor = parent.expressionFactor;
                        gene.expressionMax = parent.expressionMax;
                        gene.expressionAge = parent.expressionAge;
                        wasChanged = true;
                    }
                } else {
                    // Bad reference: disable inheritance
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
    if (!actorPtr || actorPtr->genetics.mGenes.empty()) 
        return;
    
    // Radiation input and scale bounds
    const float kMaxRadiationCap        = 2.45f;
    const float kMinScaleClamp          = 0.01f;
    const float kMaxScaleClamp          = 2.0f;
    
    // Skeletal posture and distortion multipliers
    const float kHunchIntensity         = 1.0f;
    const float kTwistIntensity         = 1.0f;
    const float kAsymmetryIntensity     = 1.0f;
    const float kBodyBulgeIntensity     = 0.3f;
    const float kLimbSplayIntensity     = 1.0f;
    
    // Locomotion and animation multipliers
    const float kGaitBreakChance        = 30.0f;
    const float kGaitSwingScale         = 1.0f;
    
    // Palette shift modifiers
    const float kRedDampenMultiplier    = 1.001f;
    const float kGreenTintMultiplier    = 1.003f;
    const float kBlueDampenMultiplier   = 1.002f;
    
    // Biological inflammation multiplier
    const float kInflammationMultiplier = 1.0f;
    
    // Random genetic noise multiplier
    const float kRandomNoiseIntensity   = 60.0f;
    
    // Growth spawning and maturation parameters
    const float kGrowthChance           = 60.0f;
    const float kGrowthSurfaceChance    = 80.0f;
    const unsigned int kMaxGrowthSpawns = 3;
    const unsigned int kMaxGenomeSize   = 64;
    const float kGrowthScaleRatioMin    = 0.35f;
    const float kGrowthScaleRatioMax    = 0.85f;
    const float kGrowthAgeDelayMin      = 0;
    const float kGrowthAgeDelayMax      = actorPtr->physical.mAgeSenior;
    
    // Its stronger than you think
    radiationAmount *= 0.1f;
    
    unsigned int numberOfGenes = actorPtr->genetics.mGenes.size();
    float radNorm = glm::clamp(radiationAmount * 0.1f, 0.0f, kMaxRadiationCap);
    
    // Deformity parameters
    float globalHunch = Random.Range(0.1f, 0.4f) * radNorm * kHunchIntensity;
    float globalTwist = Random.Range(-0.35f, 0.35f) * radNorm * kTwistIntensity;
    bool breakGait    = (Random.Range(0, 100) < static_cast<int>(kGaitBreakChance * radNorm));
    
    // Asymmetric hypertrophy bias
    bool asymmetryBias = (Random.Range(0, 100) > 50);
    
    for (unsigned int i = 0; i < numberOfGenes; i++) {
        Gene& gene = actorPtr->genetics.mGenes[i];
        
        // Lateral positioning
        float lateralPos = gene.position.x + gene.offset.x;
        bool isLeft  = lateralPos < -0.05f;
        bool isRight = lateralPos > 0.05f;
        
        // Asymmetric scale modification
        float scaleMod = 1.0f;
        if (isLeft) {
            scaleMod = asymmetryBias ? (1.0f + Random.Range(0.2f, 0.8f) * radNorm * kAsymmetryIntensity) 
                                     : (1.0f / (1.0f + Random.Range(0.2f, 0.5f) * radNorm * kAsymmetryIntensity));
        } else if (isRight) {
            scaleMod = (!asymmetryBias) ? (1.0f + Random.Range(0.2f, 0.8f) * radNorm * kAsymmetryIntensity) 
                                        : (1.0f / (1.0f + Random.Range(0.2f, 0.5f) * radNorm * kAsymmetryIntensity));
        } else {
            // Central body bulging
            gene.scale.x *= (1.0f + Random.Range(-0.2f, 0.6f) * radNorm * kBodyBulgeIntensity);
            gene.scale.z *= (1.0f + Random.Range(-0.2f, 0.6f) * radNorm * kBodyBulgeIntensity);
            gene.scale.y *= (1.0f - Random.Range(0.1f, 0.3f) * radNorm * kBodyBulgeIntensity);
        }
        
        // Apply scale clamping with minimum bound
        gene.scale.x = glm::clamp(gene.scale.x * scaleMod, kMinScaleClamp, kMaxScaleClamp);
        gene.scale.y = glm::clamp(gene.scale.y * scaleMod, kMinScaleClamp, kMaxScaleClamp);
        gene.scale.z = glm::clamp(gene.scale.z * scaleMod, kMinScaleClamp, kMaxScaleClamp);
        
        // Skeletal offsets and joint displacement
        if (gene.animationType == ActorState::Animation::Head) {
            gene.offset.y -= globalHunch * 0.4f;
            gene.offset.z += globalHunch * 0.5f;
            gene.rotation.x += globalHunch;
            gene.rotation.z += globalTwist;
        } else if (gene.animationType == ActorState::Animation::Body) {
            gene.offset.y -= globalHunch * 0.2f;
            gene.rotation.x += globalHunch * 0.5f;
            gene.rotation.y += globalTwist * 0.5f;
        } else if (gene.animationType == ActorState::Animation::Limb || 
                   gene.animationType == ActorState::Animation::LimbHolding) {
            gene.rotation.z += (isLeft ? -1.0f : 1.0f) * Random.Range(0.1f, 0.4f) * radNorm * kLimbSplayIntensity;
            gene.rotation.x += Random.Range(-0.2f, 0.2f) * radNorm * kLimbSplayIntensity;
        
            // Gait desynchronization
            if (breakGait && Random.Range(0, 100) < 50) {
                gene.animationRange *= Random.Range(0.3f * kGaitSwingScale, 1.8f * kGaitSwingScale);
                if (Random.Range(0, 100) < 25) {
                    gene.doInverseAnimation = !gene.doInverseAnimation;
                }
            }
        }
        
        // Sickly palette shift
        gene.color.x = glm::clamp(gene.color.x * (1.0f - kRedDampenMultiplier * radNorm * 0.1f), 0.0f, 1.0f);
        gene.color.y = glm::clamp(gene.color.y * (1.0f + kGreenTintMultiplier * radNorm * 0.1f), 0.0f, 1.0f);
        gene.color.z = glm::clamp(gene.color.z * (1.0f - kBlueDampenMultiplier * radNorm * 0.1f), 0.0f, 1.0f);
        
        // Random genetic noise
        if (kRandomNoiseIntensity > 0.0f) {
            float noise = (Random.Range(0.0f, radNorm * 0.1f) - Random.Range(0.0f, radNorm * 0.1f)) * kRandomNoiseIntensity;
            
            gene.offset.x += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise * 0.3f;
            gene.offset.y += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise * 0.3f;
            gene.offset.z += ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise * 0.3f;
            
            gene.position.x += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 0.3f;
            gene.position.y += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 0.3f;
            gene.position.z += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 0.3f;
            
            gene.rotation.x += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 16.0f;
            gene.rotation.y += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 16.0f;
            gene.rotation.z += ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise * 16.0f;
            
            gene.scale.x = glm::clamp(gene.scale.x + ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise, kMinScaleClamp, kMaxScaleClamp);
            gene.scale.y = glm::clamp(gene.scale.y + ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise, kMinScaleClamp, kMaxScaleClamp);
            gene.scale.z = glm::clamp(gene.scale.z + ((Random.Range(0, 100) * 0.001f) - (Random.Range(0, 100) * 0.001f)) * noise, kMinScaleClamp, kMaxScaleClamp);
            
            gene.color.x = glm::clamp(gene.color.x + ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise, 0.0f, 1.0f);
            gene.color.y = glm::clamp(gene.color.y + ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise, 0.0f, 1.0f);
            gene.color.z = glm::clamp(gene.color.z + ((Random.Range(0, 100) * 0.0001f) - (Random.Range(0, 100) * 0.0001f)) * noise, 0.0f, 1.0f);
        }
        
        // Biological inflammation
        if (i < actorPtr->biological.mBiologics.size()) {
            actorPtr->biological.mBiologics[i].inflammation += Random.Range(0.05f, 0.25f) * radNorm * kInflammationMultiplier;
            actorPtr->biological.mBiologics[i].tint.y += 0.1f * radNorm * kGreenTintMultiplier;
        }
    }
    
    // Bulbous growths and structural malformations
    if (Random.Range(0, 100) < static_cast<int>(kGrowthChance * radNorm * 10.0f) && numberOfGenes < kMaxGenomeSize) {
        unsigned int growthsToSpawn = static_cast<unsigned int>(Random.Range(1.0f, static_cast<float>(kMaxGrowthSpawns)));
        
        for (unsigned int g = 0; g < growthsToSpawn && actorPtr->genetics.mGenes.size() < kMaxGenomeSize; g++) {
            // Select random host gene to spawn from
            unsigned int hostIndex = static_cast<unsigned int>(Random.Range(0.0f, static_cast<float>(numberOfGenes - 1)));
            const Gene& host = actorPtr->genetics.mGenes[hostIndex];
            
            // Base growth configuration matched directly to host kinematics
            Gene growth = host;
            
            // Age dependent expression matching sexual attribute morphology
            growth.form             = actorPtr->physical.GetSexualOrientation() ? ActorState::Genetic::Male : ActorState::Genetic::Female;
            growth.expressionAge    = Random.Range(kGrowthAgeDelayMin, kGrowthAgeDelayMax);
            growth.expressionFactor = Random.Range(1.2f, 1.5f);
            growth.expressionMax    = 1.8f;
            growth.doExpress        = true;
            growth.scaleIndex       = 0;
            growth.colorIndex       = 0;
            
            // Form a cubic growth scaled from host limb thickness
            float hostThickness = (host.scale.x + host.scale.z) * 0.5f;
            float cubeDimension = hostThickness * Random.Range(kGrowthScaleRatioMin, kGrowthScaleRatioMax) * radNorm;
            cubeDimension = glm::clamp(cubeDimension, kMinScaleClamp, kMaxScaleClamp);
            
            growth.scale = Codon(
                glm::clamp(cubeDimension * Random.Range(0.9f, 1.1f), kMinScaleClamp, kMaxScaleClamp),
                glm::clamp(cubeDimension * Random.Range(0.9f, 1.1f), kMinScaleClamp, kMaxScaleClamp),
                glm::clamp(cubeDimension * Random.Range(0.9f, 1.1f), kMinScaleClamp, kMaxScaleClamp)
            );
            
            // Radial orientation along host cross section
            float angle = Random.Range(0.0f, glm::two_pi<float>());
            
            // Determine surface vs internal distribution
            bool isSurfaceGrowth = (Random.Range(0.0f, 100.0f) < kGrowthSurfaceChance);
            float surfaceDistance = isSurfaceGrowth 
                ? Random.Range(0.75f, 1.05f) 
                : Random.Range(0.05f, 0.45f);
            
            float lateralOffsetX = std::cos(angle) * (host.scale.x * 0.5f) * surfaceDistance;
            float lateralOffsetZ = std::sin(angle) * (host.scale.z * 0.5f) * surfaceDistance;
            float verticalOffset = Random.Range(-host.scale.y * 0.35f, host.scale.y * 0.35f);
            
            growth.position = Codon(
                host.position.x + lateralOffsetX,
                host.position.y + verticalOffset,
                host.position.z + lateralOffsetZ
            );
            
            // Discoloration derived from host pigmentation
            growth.color.x = glm::clamp(host.color.x * 0.8f + (0.1f * radNorm), 0.0f, 1.0f);
            growth.color.y = glm::clamp(host.color.y * (1.0f + kGreenTintMultiplier * radNorm), 0.0f, 1.0f);
            growth.color.z = glm::clamp(host.color.z * 0.5f, 0.0f, 1.0f);
            
            actorPtr->genetics.AddGene(growth);
            
            // Dormant scale phenotype before reaching expression age
            Phen phen;
            float dormantScale = isSurfaceGrowth 
                ? Random.Range(0.15f, 0.35f) 
                : Random.Range(0.05f, 0.15f);
            phen.scale = Codon(dormantScale, dormantScale, dormantScale);
            actorPtr->genetics.mPhen.push_back(phen);
            
            // Biological inflammation overlay
            Bio bio;
            bio.inflammation = Random.Range(0.05f, 0.3f) * radNorm * kInflammationMultiplier;
            bio.tint.y = 0.15f * radNorm * kGreenTintMultiplier;
            actorPtr->biological.mBiologics.push_back(bio);
        }
        
        // Rebuild renderer list for newly added geometry
        actorPtr->genetics.mDoUpdateGenetics = true;
    }
    
    // Rebuild genetic expression and collision bounding volume
    actorPtr->RebuildGeneticExpression();
    actorPtr->CalculateBoundingRegionFromGenome();
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
    
    if (geneA.doExpress || geneB.doExpress) {gene.doExpress = true;} else {gene.doExpress = false;}
    
    gene.animationRange = Float.Lerp(geneA.animationRange, geneB.animationRange, bias);
    gene.animationType = (ActorState::Animation)Int.Lerp((int)geneA.animationType, (int)geneB.animationType, bias);
    
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
        {gene.form = geneA.form;} else 
        {gene.form = geneB.form;}
    
    gene.animationAxis.x = Float.Lerp(geneA.animationAxis.x, geneB.animationAxis.x, bias);
    gene.animationAxis.y = Float.Lerp(geneA.animationAxis.y, geneB.animationAxis.y, bias);
    gene.animationAxis.z = Float.Lerp(geneA.animationAxis.z, geneB.animationAxis.z, bias);
    
    return gene;
}

