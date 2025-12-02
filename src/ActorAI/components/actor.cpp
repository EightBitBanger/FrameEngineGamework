#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;
extern RenderSystem   Renderer;


Actor::Actor() : 
    isGarbage(false),
    isActive(false),
    isSaved(false),
    mName("")
{
}

void Actor::Reset(void) {
    isGarbage = false;
    isActive = false;
    isSaved = false;
    mName = "";
    
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
    state.current       = ActorState::State::None;
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
    physical.mDoUpdateCollider = false;
    physical.mColliderOffset = glm::vec3(0, 0.5f, 0);
    physical.mColliderScale  = glm::vec3(0.5f, 0.5f, 0.5f);
    physical.mColliderBody = nullptr;
    
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
    current(ActorState::State::None),
    mIsWalking(false),
    mIsRunning(false),
    mIsFacing(false)
{
}

Actor::IdiosyncraticCharacteristics::IdiosyncraticCharacteristics() 
{
}

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
    mSexualOrientation(false),
    mDoUpdateCollider(false),
    mColliderOffset(glm::vec3(0, 0.5f, 0)),
    mColliderScale(glm::vec3(0.5f, 0.5f, 0.5f)),
    mColliderBody(nullptr)
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


void Actor::SetName(std::string newName) {
    mName = newName;
}

void Actor::RebuildGeneticExpression(void) {
    genetics.mDoReexpressGenetics = true;
}

std::string Actor::GetName(void) {
    return mName;
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
// Memories

void Actor::IdiosyncraticCharacteristics::Add(std::string name, std::string memory) {
    mMemories[name] = memory;
}

bool Actor::IdiosyncraticCharacteristics::Remove(std::string name) {
    mMemories.erase(name);
    return false;
}

std::string Actor::IdiosyncraticCharacteristics::Get(std::string name) {
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

bool Actor::IdiosyncraticCharacteristics::CheckExists(std::string name) {
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

void Actor::PhysicalAttributes::UpdatePhysicalCollider(void) {
    mDoUpdateCollider = true;
}

void Actor::PhysicalAttributes::SetColliderScale(glm::vec3 extents) {
    mColliderScale = extents;
}

void Actor::PhysicalAttributes::SetColliderOffset(glm::vec3 offset) {
    mColliderOffset = offset;
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

void Actor::UserVariables::SetUserBitmask(uint8_t bitmask) {
    mBitmask = bitmask;
}

uint8_t Actor::UserVariables::GetUserBitmask(void) {
    return mBitmask;
}

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
