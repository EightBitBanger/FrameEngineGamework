#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;
extern RenderSystem   Renderer;


Actor::Actor() : 
    isGarbage(false),
    isActive(false),
    isSaved(false),
    mName(""),
    mBoundingBoxMin(-0.5f, -0.5f, -0.5f),
    mBoundingBoxMax(0.5f, 0.5f, 0.5f)
{}

void Actor::Reset(void) {
    isGarbage = false;
    isActive = false;
    isSaved = false;
    mName = "";
    
    mBoundingBoxMin = {-0.5f, -0.5f, -0.5f};
    mBoundingBoxMax = {0.5f, 0.5f, 0.5f};
    
    // Navigation
    navigation.mVelocity      = glm::vec3(0);
    navigation.mPosition      = glm::vec3(0);
    navigation.mRotation      = glm::vec3(0);
    navigation.mFacing        = glm::vec3(0);
    navigation.mLookAt        = glm::vec3(0);
    
    navigation.mRotateTo      = glm::vec3(0);
    navigation.mTargetPoint   = glm::vec3(0);
    navigation.mTargetLook    = glm::vec3(0);
    navigation.mDistanceToTarget = 0;
    navigation.mTargetActor   = nullptr;
    navigation.mQueryPoints.clear();
    
    // Behavior
    behavior.mDistanceToFocus      = 50.0f;
    behavior.mDistanceToWalk       = 30.0f;
    behavior.mDistanceToAttack     = 30.0f;
    behavior.mDistanceToFlee       = 20.0f;
    behavior.mDistanceToInflict    = 1.5f;
    
    behavior.mCooldownAttack       = 2;
    behavior.mCooldownObserve      = 16;
    behavior.mCooldownMove         = 8;
    
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
    emotions.mFear = 0.0f;
    emotions.mAnger = 0.0f;
    emotions.mFatigue = 0.0f;
    emotions.mStress = 0.0f;
    emotions.mCuriosity = 0.0f;
    emotions.mComfort = 0.0f;
    emotions.mLibido = 0.0f;
    
    // Physical
    physical.mAge            = 0;
    physical.mAgeAdult       = 1000.0f;
    physical.mAgeSenior      = 50000.0f;
    physical.mSpeed          = 1.5f;
    physical.mSpeedYouth     = 0.8f;
    physical.mSpeedMul       = 1.3f;
    physical.mSnapSpeed      = 0.07f;
    physical.mYouthScale     = 0.5f;
    physical.mAdultScale     = 1.0f;
    physical.mSexualOrientation = false;
    
    // Cool-down timers
    counters.mObservationCoolDownCounter = 0;
    counters.mAttackCoolDownCounter      = 0;
    counters.mMovementCoolDownCounter    = 0;
    counters.mBreedingCoolDownCounter    = 0;
    
    // User variables
    user.mBitmask    = 0;
    user.mUserDataA  = nullptr;
    user.mUserDataB  = nullptr;
    
    return;
}


Actor::NavigationSystem::NavigationSystem() : 
    mVelocity(glm::vec3(0)),
    mPosition(glm::vec3(0)),
    mRotation(glm::vec3(0)),
    mFacing(glm::vec3(0)),
    mLookAt(glm::vec3(0)),
    mRotateTo(glm::vec3(0)),
    mTargetPoint(glm::vec3(0)),
    mTargetLook(glm::vec3(0)),
    mDistanceToTarget(0),
    mTargetActor(nullptr)
{
}

Actor::Behavior::Behavior() : 
    mIsPredator(false),
    mIsPrey(false),
    mDistanceToFocus(0),
    mDistanceToWalk(0),
    mDistanceToAttack(0),
	mDistanceToFlee(0),
    mDistanceToInflict(0),
    
    mCooldownAttack(0),
    mCooldownObserve(0),
    mCooldownMove(0),
    
    mHeightPreferenceMin(0),
	mHeightPreferenceMax(0)
{
}

Actor::State::State() : 
    mode(ActorState::Mode::Idle),
    mIsWalking(false),
    mIsRunning(false),
    mIsFacing(false)
{
}

Actor::IdiosyncraticCharacteristics::IdiosyncraticCharacteristics() 
{
}

Actor::EmotionalState::EmotionalState() : 
    mFear(0.0f),
    mAnger(0.0f),
    mFatigue(0.0f),
    mStress(0.0f),
    mCuriosity(0.0f),
    mComfort(0.0f),
    mLibido(0.0f)
{}

Actor::PhysicalAttributes::PhysicalAttributes() : 
    mAge(0),
    mAgeAdult(0),
    mAgeSenior(0),
    mSpeed(0),
    mSpeedYouth(0),
    mSpeedMul(0),
    mSnapSpeed(0.07f),
    mYouthScale(1),
    mAdultScale(1),
    mSexualOrientation(false)
{
}

Actor::GeneticsSystem::GeneticsSystem() : 
    mDoUpdateGenetics(false),
    mDoReexpressGenetics(false),
    mGeneration(0)
{
}

Actor::BiologicalSystem::BiologicalSystem() : 
    health(0),
    hunger(0),
    strength(0),
    defense(0)
{
}

Actor::CooldownCounters::CooldownCounters() : 
    mObservationCoolDownCounter(0),
    mMovementCoolDownCounter(0),
    mAttackCoolDownCounter(0),
    mBreedingCoolDownCounter(0)
{
}

Actor::UserVariables::UserVariables() : 
    mBitmask(0),
    mUserDataA(nullptr),
    mUserDataB(nullptr)
{
}


void Actor::SetName(const std::string& newName) {
    mName = newName;
}

const std::string& Actor::GetName(void) {
    return mName;
}

void Actor::RebuildGeneticExpression(void) {
    genetics.mDoReexpressGenetics = true;
}



//
// Targeting / navigation and focus

void Actor::NavigationSystem::SetPosition(glm::vec3 position) {
    mPosition = position;
    mTargetPoint = position;
}

glm::vec3 Actor::NavigationSystem::GetPosition(void) {
    return mPosition;
}

void Actor::NavigationSystem::SetTargetPoint(glm::vec3 position) {
    mTargetPoint = position;
    return;
}

glm::vec3 Actor::NavigationSystem::GetTargetPoint(void) {
    return mTargetPoint;
}

Actor* Actor::NavigationSystem::GetTargetActor(void) {
    return mTargetActor;
}

void Actor::NavigationSystem::SetTargetActor(Actor* actorPtr) {
    mTargetActor = actorPtr;
}

float Actor::NavigationSystem::GetDistanceToTarget(void) {
    return mDistanceToTarget;
}


//
// AI state behavioral hardwiring

void Actor::Behavior::SetDistanceToFocus(float distance) {
    mDistanceToFocus = distance;
}

float Actor::Behavior::GetDistanceToFocus(void) {
    return mDistanceToFocus;
}

void Actor::Behavior::SetDistanceToWalk(float distance) {
    mDistanceToWalk = distance;
}

float Actor::Behavior::GetDistanceToWalk(void) {
    return mDistanceToWalk;
}

void Actor::Behavior::SetDistanceToAttack(float distance) {
    mDistanceToAttack = distance;
}

float Actor::Behavior::GetDistanceToAttack(void) {
    return mDistanceToAttack;
}

void Actor::Behavior::SetDistanceToFlee(float distance) {
    mDistanceToFlee = distance;
}

float Actor::Behavior::GetDistanceToFlee(void) {
    return mDistanceToFlee;
}

void Actor::Behavior::SetDistanceToInflict(float distance) {
    mDistanceToInflict = distance;
}

float Actor::Behavior::GetDistanceToInflict(void) {
    return mDistanceToInflict;
}

void Actor::Behavior::SetCooldownAttack(unsigned int ticks) {
    mCooldownAttack = ticks;
}

void Actor::Behavior::SetCooldownObserve(unsigned int ticks) {
    mCooldownObserve = ticks;
}

void Actor::Behavior::SetCooldownMove(unsigned int ticks) {
    mCooldownMove = ticks;
}

unsigned int Actor::Behavior::GetCooldownAttack(void) {
    return mCooldownAttack;
}

unsigned int Actor::Behavior::GetCooldownObserve(void) {
    return mCooldownObserve;
}

unsigned int Actor::Behavior::GetCooldownMove(void) {
    return mCooldownMove;
}

void Actor::Behavior::SetHeightPreferenceMin(float height) {
    mHeightPreferenceMin = height;
}

float Actor::Behavior::GetHeightPreferenceMin(void) {
    return mHeightPreferenceMin;
}

void Actor::Behavior::SetHeightPreferenceMax(float height) {
    mHeightPreferenceMax = height;

}

float Actor::Behavior::GetHeightPreferenceMax(void) {
    return mHeightPreferenceMax;
}

void Actor::Behavior::SetPredatorState(bool state) {
    mIsPredator = state;
}

bool Actor::Behavior::GetPredatorState(void) {
    return mIsPredator;
}

void Actor::Behavior::SetPreyState(bool state) {
    mIsPrey = state;
}

bool Actor::Behavior::GetPreyState(void) {
    return mIsPrey;
}

//
// Voice

void Actor::VocalSynthesizer::AddVoice(const std::string& name, Sound* sound) {
    if (name == "") 
        return;
    mVocals[name] = sound;
}

Sound* Actor::VocalSynthesizer::GetVoice(const std::string& name) {
    return mVocals[name];
}

//
// Memories

void Actor::IdiosyncraticCharacteristics::Add(const std::string& name, const std::string& memory) {
    mMemories[name] = memory;
}

bool Actor::IdiosyncraticCharacteristics::Remove(const std::string& name) {
    mMemories.erase(name);
    return false;
}

std::string Actor::IdiosyncraticCharacteristics::Get(const std::string& name) {
    return mMemories[name];
}

std::string Actor::IdiosyncraticCharacteristics::GetMemoryNameByIndex(unsigned int index) {
    unsigned int counter;
    for (std::unordered_map<std::string, std::string>::iterator it = mMemories.begin(); it != mMemories.end(); ++it) {
        if (counter == index) 
            return it->first;
        counter++;
    }
    return "";
}

std::string Actor::IdiosyncraticCharacteristics::GetMemoryValueByIndex(unsigned int index) {
    unsigned int counter;
    for (std::unordered_map<std::string, std::string>::iterator it = mMemories.begin(); it != mMemories.end(); ++it) {
        if (counter == index) 
            return it->second;
        counter++;
    }
    return "";
}

unsigned int Actor::IdiosyncraticCharacteristics::GetNumberOfMemories(void) {
    return mMemories.size();
}

void Actor::IdiosyncraticCharacteristics::Clear(void) {
    mMemories.clear();
}

bool Actor::IdiosyncraticCharacteristics::CheckExists(const std::string& name) {
    std::unordered_map<std::string, std::string>::iterator it = mMemories.find(name);
    if (it != mMemories.end()) 
        return true;
    return false;
}


//
// Genetics system

unsigned int Actor::GeneticsSystem::AddGene(Gene& newGene) {
    mGenes.push_back( newGene );
    return mGenes.size();
}

void Actor::GeneticsSystem::RemoveGene(unsigned int index) {
    mGenes.erase( mGenes.begin() + index );
}

void Actor::GeneticsSystem::ClearGenome(void) {
    mGenes.clear();
}

void Actor::GeneticsSystem::ClearPhenome(void) {
    mPhen.clear();
}

unsigned int Actor::GeneticsSystem::GetNumberOfGenes(void) {
    return mGenes.size();
}

Gene Actor::GeneticsSystem::GetGeneFromGenome(unsigned int index) {
    if (index < mGenes.size()) 
        return mGenes[index];
    Gene gene;
    gene.color.x = -1.0f;
    return gene;
    
}

Phen Actor::GeneticsSystem::GetPhenFromPhenotype(unsigned int index) {
    if (index < mPhen.size()) 
        return mPhen[index];
    Phen phen;
    phen.color.x = -1.0f;
    return phen;
}

void Actor::GeneticsSystem::SetGeneration(unsigned int newGeneration) {
    mGeneration = newGeneration;
}

unsigned int Actor::GeneticsSystem::GetGeneration(void) {
    return mGeneration;
}

unsigned int Actor::GeneticsSystem::GetNumberOfMeshRenderers(void) {
    return mGeneticRenderers.size();
}

MeshRenderer* Actor::GeneticsSystem::GetMeshRendererAtIndex(unsigned int index) {
    if (index < mGeneticRenderers.size()) 
        return mGeneticRenderers[index];
    return nullptr;
}

//
// Emotional state

void Actor::EmotionalState::SetFear(float fear) {
    mFear = fear;
}

float Actor::EmotionalState::GetFear(void) {
    return mFear;
}

void Actor::EmotionalState::AddFear(float additive) {
    mFear += additive;
}


void Actor::EmotionalState::SetAnger(float anger) {
    mAnger = anger;
}

float Actor::EmotionalState::GetAnger(void) {
    return mAnger;
}

void Actor::EmotionalState::AddAnger(float additive) {
    mAnger += additive;
}


void Actor::EmotionalState::SetFatigue(float fatigue) {
    mFatigue = fatigue;
}

float Actor::EmotionalState::GetFatigue(void) {
    return mFatigue;
}

void Actor::EmotionalState::AddFatigue(float additive) {
    mFatigue += additive;
}


void Actor::EmotionalState::SetStress(float stress) {
    mStress = stress;
}

float Actor::EmotionalState::GetStress(void) {
    return mStress;
}

void Actor::EmotionalState::AddStress(float additive) {
    mStress += additive;
}


void Actor::EmotionalState::SetCuriosity(float curiosity) {
    mCuriosity = curiosity;
}

float Actor::EmotionalState::GetCuriosity(void) {
    return mCuriosity;
}

void Actor::EmotionalState::AddCuriosity(float additive) {
    mCuriosity += additive;
}


void Actor::EmotionalState::SetComfort(float comfort) {
    mComfort = comfort;
}

float Actor::EmotionalState::GetComfort(void) {
    return mComfort;
}

void Actor::EmotionalState::AddComfort(float additive) {
    mComfort += additive;
}


void Actor::EmotionalState::SetLibido(float libido) {
    mLibido = libido;
}

float Actor::EmotionalState::GetLibido(void) {
    return mLibido;
}

void Actor::EmotionalState::AddLibido(float additive) {
    mLibido += additive;
}


//
// Physical characteristics

void Actor::PhysicalAttributes::SetAge(unsigned long int newAge) {
    mAge = newAge;
}

unsigned long int Actor::PhysicalAttributes::GetAge(void) {
    return mAge;
}

void Actor::PhysicalAttributes::SetSeniorAge(float oldAge) {
    mAgeSenior = oldAge;
}

float Actor::PhysicalAttributes::GetSeniorAge(void) {
    return mAgeSenior;
}

void Actor::PhysicalAttributes::SetAdultAge(float age) {
    mAgeAdult = age;
}

float Actor::PhysicalAttributes::GetAdultAge(void) {
    return mAgeAdult;
}

void Actor::PhysicalAttributes::SetSpeed(float newSpeed) {
    mSpeed = newSpeed;
}

float Actor::PhysicalAttributes::GetSpeed(void) {
    return mSpeed;
}

void Actor::PhysicalAttributes::SetSpeedYouth(float newSpeed) {
    mSpeedYouth = newSpeed;
}

float Actor::PhysicalAttributes::GetSpeedYouth(void) {
    return mSpeedYouth;
}

void Actor::PhysicalAttributes::SetSpeedMultiplier(float newSpeedMul) {
    mSpeedMul = newSpeedMul;
}

float Actor::PhysicalAttributes::GetSpeedMultiplier(void) {
    return mSpeedMul;
}

void Actor::PhysicalAttributes::SetYouthScale(float scale) {
    mYouthScale = scale;
}

float Actor::PhysicalAttributes::GetYouthScale(void) {
    return mYouthScale;
}

void Actor::PhysicalAttributes::SetAdultScale(float scale) {
    mAdultScale = scale;
}

float Actor::PhysicalAttributes::GetAdultScale(void) {
    return mAdultScale;
}

void Actor::PhysicalAttributes::SetSexualOrientation(bool orientation) {
    mSexualOrientation = orientation;
}

bool Actor::PhysicalAttributes::GetSexualOrientation(void) {
    return mSexualOrientation;
}


//
// Cool down counters

void Actor::CooldownCounters::SetCoolDownObservation(unsigned int counter) {
    mObservationCoolDownCounter = counter;
}
unsigned int Actor::CooldownCounters::GetCoolDownObservation(void) {
    return mObservationCoolDownCounter;
}

void Actor::CooldownCounters::SetCoolDownMovement(unsigned int counter) {
    mMovementCoolDownCounter = counter;
}
unsigned int Actor::CooldownCounters::GetCoolDownMovement(void) {
    return mMovementCoolDownCounter;
}

void Actor::CooldownCounters::SetCoolDownAttack(unsigned int counter) {
    mAttackCoolDownCounter = counter;
}
unsigned int Actor::CooldownCounters::GetCoolDownAttack(void) {
    return mAttackCoolDownCounter;
}

void Actor::CooldownCounters::SetCoolDownBreeding(unsigned int counter) {
    mBreedingCoolDownCounter = counter;
}
unsigned int Actor::CooldownCounters::GetCoolDownBreeding(void) {
    return mBreedingCoolDownCounter;
}


//
// User assignable variables

void Actor::UserVariables::SetUserDataA(void* ptr) {
    mUserDataA = ptr;
}

void* Actor::UserVariables::GetUserDataA(void) {
    return mUserDataA;
}

void Actor::UserVariables::SetUserDataB(void* ptr) {
    mUserDataB = ptr;
}

void* Actor::UserVariables::GetUserDataB(void) {
    return mUserDataB;
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

void Actor::UpdateBoundingBoxFromGenome(void) {
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
        
        if (gene.type != EXPRESSION_TYPE_BASE) {
            if ((gene.type == EXPRESSION_TYPE_MALE   && sexualOrientation == false) ||
                (gene.type == EXPRESSION_TYPE_FEMALE && sexualOrientation == true)) {
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
        
        if (gene.type != EXPRESSION_TYPE_BASE) {
            
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
    {
        glm::vec3 center   = (finalMin + finalMax) * 0.5f;
        glm::vec3 halfSize = (finalMax - finalMin) * 0.5f;
        
        float halfXZ = (halfSize.x > halfSize.z) ? halfSize.x : halfSize.z;
        halfSize.x = halfXZ;
        halfSize.z = halfXZ;
        
        finalMin = center - halfSize;
        finalMax = center + halfSize;
    }
    
    const float padding = 0.002f;
    finalMin -= glm::vec3(padding);
    finalMax += glm::vec3(padding);
    
    glm::vec3 finalScale(0.75f, 0.5f, 0.75f);
    finalMin *= finalScale;
    finalMax *= finalScale;
    
    SetBoundingBox(finalMin, finalMax);
}

