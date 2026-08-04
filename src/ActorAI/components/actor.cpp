#include <GameEngineFramework/ActorAI/components/actor.h>
#include <GameEngineFramework/Types/Types.h>

extern PhysicsSystem  Physics;
extern RenderSystem   Renderer;
extern StringType     String;

Actor::Actor() : 
    isGarbage(false),
    isActive(false),
    isSaved(false),
    mName(""),
    mBoundingBoxMin(-0.5f, -0.5f, -0.5f),
    mBoundingBoxMax(0.5f, 0.5f, 0.5f),
    mUpdateCounter(0)
{}

void Actor::SetName(const std::string& newName) {
    mName = newName;
}

const std::string& Actor::GetName(void) {
    return mName;
}

void Actor::RebuildGeneticExpression(void) {
    genetics.mDoReexpressGenetics = true;
}

void Actor::Reset(void) {
    isGarbage = false;
    isActive = false;
    isSaved = false;
    mName = "";
    
    mBoundingBoxMin = {-0.5f, -0.5f, -0.5f};
    mBoundingBoxMax = {0.5f, 0.5f, 0.5f};
    
    mUpdateCounter = (unsigned int)Random.Range(0, 8);
    
    // Inventory
    inventory.damageMul       = 1.0f;
    inventory.defenseMul      = 1.0f;
    inventory.handPosition    = glm::vec3(0.0f, 0.0f, 0.0f);
    inventory.handOffset      = glm::vec3(0.0f, 0.0f, 0.0f);
    inventory.handRotation    = glm::vec3(0.0f, 0.0f, 0.0f);
    inventory.handScale       = glm::vec3(1.0f, 1.0f, 1.0f);
    inventory.holdingRenderer = nullptr;
    inventory.itemClassList.clear();
    
    // Navigation
    navigation.mVelocity  = glm::vec3(0);
    navigation.mPosition  = glm::vec3(0);
    navigation.mRotation  = glm::vec3(0);
    navigation.mFacing    = glm::vec3(0);
    navigation.mLookAt    = glm::vec3(0);
    
    navigation.mRotateTo         = glm::vec3(0);
    navigation.mTargetPoint      = glm::vec3(0);
    navigation.mTargetLook       = glm::vec3(0);
    navigation.mDistanceToTarget = 0;
    navigation.mTargetActor      = nullptr;
    navigation.mQueryPoints.clear();
    
    // Behavior
    behavior.mDistanceToFocus      = 50.0f;
    behavior.mDistanceToWalk       = 30.0f;
    behavior.mDistanceToAttack     = 30.0f;
    behavior.mDistanceToFlee       = 20.0f;
    behavior.mDistanceToInflict    = 5.24f;
    
    behavior.mCooldownAttack       = 2;
    behavior.mCooldownObserve      = 16;
    behavior.mCooldownSocial       = 8;
    behavior.mCooldownMove         = 8;
    behavior.mCooldownBreed        = 220;
    
    behavior.mHeightPreferenceMin  = 0.0f;
    behavior.mHeightPreferenceMax  = 1000.0f;
    
    // State
    state.mode          = ActorState::Mode::Idle;
    state.mIsWalking    = false;
    state.mIsRunning    = false;
    state.mIsFacing     = true;
    
    // Idiosyncrasies
    memories.Clear();
    
    // Genetics
    genetics.mDoUpdateGenetics     = false;
    genetics.mDoReexpressGenetics  = false;
    genetics.mGeneration           = 0;
    genetics.ClearGenome();
    genetics.ClearPhenome();
    
    // Biological
    biological.health    = 10.0f;
    biological.hunger    = 0.0f;
    biological.defense   = 0.0f;
    biological.strength  = 1.0f;
    
    // Emotions
    emotions.current.Clear();
    
    // Physical
    physical.mAge            = 0;
    physical.mAgeAdult       = 20000.0f;
    physical.mAgeSenior      = 70000.0f;
    physical.mSpeed          = 1.5f;
    physical.mSpeedYouth     = 0.8f;
    physical.mSpeedMul       = 1.3f;
    physical.mSnapSpeed      = 0.07f;
    physical.mYouthScale     = 0.5f;
    physical.mAdultScale     = 1.0f;
    physical.mSexualOrientation = false;
    
    // Cool-down timers
    counters.mObservationCoolDownCounter = 0;
    counters.mSocialCoolDownCounter      = 0;
    counters.mAttackCoolDownCounter      = 0;
    counters.mMovementCoolDownCounter    = 0;
    counters.mBreedingCoolDownCounter    = 0;
    
    // User variables
    user.mBitmask    = 0;
    user.mUserDataA  = nullptr;
    user.mUserDataB  = nullptr;
}

// Bounding box

void Actor::SetBoundingBox(const glm::vec3& min, const glm::vec3& max) {
    mBoundingBoxMin = min;
    mBoundingBoxMax = max;
}

glm::vec3 Actor::GetBoundingBoxMin(void) {
    return mBoundingBoxMin;
}

glm::vec3 Actor::GetBoundingBoxMax(void) {
    return mBoundingBoxMax;
}

// Update

void Actor::CalculateBoundingRegionFromGenome(void) {
    unsigned int geneCount = genetics.mGenes.size();
    if (geneCount == 0) {
        SetBoundingBox(glm::vec3(-0.3f, -0.3f, -0.3f), glm::vec3(0.3f, 0.3f, 0.3f));
        return;
    }
    
    // Per-gene base mesh local bounds at scale = 1.
    // Using [-0.5..0.5] fixes the "twice as tall" symptom if the source mesh is unit-sized.
    glm::vec3 baseMin(-0.5f, -0.5f, -0.5f);
    glm::vec3 baseMax( 0.5f,  0.5f,  0.5f);
    
    bool hasAny = false;
    
    glm::vec3 finalMin(0.0f);
    glm::vec3 finalMax(0.0f);
    
    bool sexualOrientation = physical.GetSexualOrientation();
    
    for (unsigned int a = 0; a < geneCount; a++) {
        
        Gene& gene = genetics.mGenes[a];
        
        if (!gene.doExpress)
            continue;
        
        if (gene.form != ActorState::Genetic::Base) {
            if ((gene.form == ActorState::Genetic::Male   && sexualOrientation == false) ||
                (gene.form == ActorState::Genetic::Female && sexualOrientation == true)) {
                continue;
            }
            
            if (physical.mAge < gene.expressionAge)
                continue;
        }
        
        glm::vec3 targetScale;
        if (gene.scaleIndex == 0) {
            targetScale = gene.scale.ToVec3();
        } else {
            unsigned int scaleIndex = gene.scaleIndex - 1;
            if (scaleIndex < geneCount) {
                targetScale = genetics.mGenes[scaleIndex].scale.ToVec3();
            } else {
                targetScale = gene.scale.ToVec3();
            }
        }
        targetScale = glm::clamp(targetScale, 0.0f, 2.0f);
        
        if (gene.form != ActorState::Genetic::Base) {
            
            float expressionFactor = gene.expressionFactor;
            
            if (a < genetics.mPhen.size()) {
                genetics.mPhen[a].scale.x = expressionFactor;
                genetics.mPhen[a].scale.y = expressionFactor;
                genetics.mPhen[a].scale.z = expressionFactor;
            }
            
            float maxUniformScale = gene.expressionMax;
            
            glm::vec3 phenScale(1.0f);
            if (a < genetics.mPhen.size()) {
                phenScale = glm::vec3(genetics.mPhen[a].scale.x,
                                      genetics.mPhen[a].scale.y,
                                      genetics.mPhen[a].scale.z);
            }
            
            targetScale *= glm::clamp(phenScale, 0.0f, maxUniformScale);
        }
        
        if (gene.attachmentIndex > 0) {
            unsigned int attachmentIndex = gene.attachmentIndex - 1;
            if (attachmentIndex < geneCount) {
                gene.offset = genetics.mGenes[attachmentIndex].offset;
            }
        }
        
        glm::vec3 localPos = gene.position.ToVec3() + gene.offset.ToVec3();
        
        glm::vec3 scaledMin = baseMin * targetScale;
        glm::vec3 scaledMax = baseMax * targetScale;
        
        glm::vec3 geneMin = glm::min(scaledMin, scaledMax) + localPos;
        glm::vec3 geneMax = glm::max(scaledMin, scaledMax) + localPos;
        
        if (!hasAny) {
            finalMin = geneMin;
            finalMax = geneMax;
            hasAny   = true;
        } else {
            finalMin = glm::min(finalMin, geneMin);
            finalMax = glm::max(finalMax, geneMax);
        }
    }
    
    if (!hasAny) {
        SetBoundingBox(glm::vec3(-0.3f, -0.3f, -0.3f), glm::vec3(0.3f, 0.3f, 0.3f));
        return;
    }
    
    // Make X/Z square while preserving center and Y extents.
    glm::vec3 center   = (finalMin + finalMax) * 0.5f;
    glm::vec3 halfSize = (finalMax - finalMin) * 0.5f;
    
    float halfXZ = (halfSize.x > halfSize.z) ? halfSize.x : halfSize.z;
    halfSize.x = halfXZ;
    halfSize.z = halfXZ;
    
    finalMin = center - halfSize;
    finalMax = center + halfSize;
    
    glm::vec3 finalScale(0.75f);
    finalMin *= finalScale;
    finalMax *= finalScale;
    
    SetBoundingBox(finalMin, finalMax);
}

