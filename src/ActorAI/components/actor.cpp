#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;
extern RenderSystem   Renderer;


Actor::Actor() : 
    mName("")
{
}

void Actor::Reset(void) {
    mName = "";
    
    // Navigation
    navigation.mVelocity      = glm::vec3(0);
    navigation.mPosition      = glm::vec3(0);
    navigation.mRotation      = glm::vec3(0);
    navigation.mRotateTo      = glm::vec3(0);
    navigation.mTargetPoint   = glm::vec3(0, 0, 0);
    navigation.mDistance      = 0.0f;
    navigation.mTargetActor   = nullptr;
    navigation.mTargetBreeding= nullptr;
    
    // Behavior
    behavior.mDistanceToFocus      = 10.0f;
    behavior.mDistanceToWalk       = 30.0f;
    behavior.mDistanceToAttack     = 30.0f;
    behavior.mDistanceToFlee       = 20.0f;
    behavior.mHeightPreferenceMin  = 0.0f;
    behavior.mHeightPreferenceMax  = 1000.0f;
    
    // State
    state.mIsActive     = true;
    state.mIsWalking    = false;
    state.mIsRunning    = false;
    state.mIsAttacking  = false;
    state.mIsFleeing    = false;
    state.mIsConsuming  = false;
    state.mIsFacing     = true;
    //state.mAnimation.clear();
    
    // Idiosyncrasies
    idiosyncrasies.mNeuralNetwork.ClearTopology();
    idiosyncrasies.mMemories.clear();
    
    // Genetics
    genetics.mDoUpdateGenetics     = false;
    genetics.mDoReexpressGenetics  = false;
    genetics.mGeneration           = 0;
    //genetics.mGenes.clear();
    //genetics.mPhen.clear();
    //genetics.mGeneticRenderers.clear();
    
    // Biological
    biological.health    = 100.0f;
    biological.hunger    = 0.0f;
    biological.immunity  = 0.0f;
    biological.energy    = 0.0f;
    biological.stress    = 0.0f;
    //biological.mBiologics.clear();
    
    // Physical
    physical.mAge            = 0;
    physical.mAgeAdult       = 1000.0f;
    physical.mAgeSenior      = 50000.0f;
    physical.mSpeed          = 1.5f;
    physical.mSpeedYouth     = 0.3f;
    physical.mSpeedMul       = 1.3f;
    physical.mSnapSpeed      = 0.24f;
    physical.mYouthScale     = 0.3f;
    physical.mAdultScale     = 1.3f;
    physical.mSexualOrientation = false;
    physical.mColliderOffset = glm::vec3(0);
    physical.mColliderScale  = glm::vec3(1);
    
    // Cooldowns
    counters.mObservationCoolDownCounter = 0;
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
    mRotateTo(glm::vec3(0)),
    mTargetPoint(glm::vec3(0, 0, 0)),
    mDistance(0),
    mTargetActor(nullptr),
    mTargetBreeding(nullptr)
{
}

Actor::Behavior::Behavior() : 
    mDistanceToFocus  (10),
    mDistanceToWalk   (30),
    mDistanceToAttack (30),
	mDistanceToFlee   (20),
    
    mHeightPreferenceMin(0.0f),
	mHeightPreferenceMax(1000)
{
}

Actor::State::State() : 
    mIsActive(true),
    mIsWalking(false),
    mIsRunning(false),
    mIsAttacking(false),
	mIsFleeing(false),
	mIsConsuming(false),
	mIsFacing(true)
{
}

Actor::IdiosyncraticCharacteristics::IdiosyncraticCharacteristics() 
{
}

Actor::PhysicalAttributes::PhysicalAttributes() : 
    mAge(0),
    mAgeAdult(1000),
    mAgeSenior(50000),
    mSpeed(1.5f),
    mSpeedYouth(0.3f),
    mSpeedMul(1.3f),
    mSnapSpeed(0.24f),
    mYouthScale(0.3f),
    mAdultScale(1.3f),
    mSexualOrientation(false),
    mColliderOffset(glm::vec3(0)),
    mColliderScale(glm::vec3(1))
{
}

Actor::GeneticsSystem::GeneticsSystem() : 
    mDoUpdateGenetics(false),
    mDoReexpressGenetics(false),
    mGeneration(0)
{
}

Actor::BiologicalSystem::BiologicalSystem() : 
    health(100.0f),
    hunger(0),
    immunity(0),
    energy(0),
    stress(0)
{
}

Actor::CooldownCounters::CooldownCounters() : 
    mObservationCoolDownCounter(0),
    mMovementCoolDownCounter(0),
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
    return;
}

void Actor::SetActive(bool active) {
    state.mIsActive = active;
    
    for (unsigned int i=0; i < genetics.mGeneticRenderers.size(); i++) 
        genetics.mGeneticRenderers[i]->isActive = active;
    return;
}

bool Actor::GetActive(void) {
    return state.mIsActive;
}

void Actor::RebuildPhenotype(void) {
    genetics.mDoUpdateGenetics = true;
    return;
}

void Actor::ReexpressPhenotype(void) {
    genetics.mDoReexpressGenetics = true;
    return;
}

std::string Actor::GetName(void) {
    std::string nameString = mName;
    return nameString;
}


//
// Targeting / navigation and focus

void Actor::NavigationSystem::SetPosition(glm::vec3 position) {
    mux.lock();
    mPosition = position;
    mux.unlock();
    return;
}

glm::vec3 Actor::NavigationSystem::GetPosition(void) {
    mux.lock();
    glm::vec3 position = mPosition;
    mux.unlock();
    return position;
}

void Actor::NavigationSystem::SetTargetPoint(glm::vec3 position) {
    mux.lock();
    mTargetPoint = position;
    mux.unlock();
    return;
}

glm::vec3 Actor::NavigationSystem::GetTargetPoint(void) {
    mux.lock();
    glm::vec3 position = mTargetPoint;
    mux.unlock();
    return position;
}

Actor* Actor::NavigationSystem::GetBreedWithActor(void) {
    return mTargetBreeding;
}

void Actor::NavigationSystem::SetBreedWithActor(Actor* actorPtr) {
    mTargetBreeding = actorPtr;
    return;
}

Actor* Actor::NavigationSystem::GetTargetActor(void) {
    return mTargetActor;
}

void Actor::NavigationSystem::SetTargetActor(Actor* actorPtr) {
    mTargetActor = actorPtr;
    return;
}


//
// AI state behavioral hardwiring

void Actor::Behavior::SetDistanceToFocus(float distance) {
    mDistanceToFocus = distance;
    return;
}

float Actor::Behavior::GetDistanceToFocus(void) {
    return mDistanceToFocus;
}

void Actor::Behavior::SetDistanceToWalk(float distance) {
    mDistanceToWalk = distance;
    return;
}

float Actor::Behavior::GetDistanceToWalk(void) {
    return mDistanceToWalk;
}

void Actor::Behavior::SetDistanceToAttack(float distance) {
    mDistanceToAttack = distance;
    return;
}

float Actor::Behavior::GetDistanceToAttack(void) {
    return mDistanceToAttack;
}

void Actor::Behavior::SetDistanceToFlee(float distance) {
    mDistanceToFlee = distance;
    return;
}

float Actor::Behavior::GetDistanceToFlee(void) {
    return mDistanceToFlee;
}

void Actor::Behavior::SetHeightPreferenceMin(float height) {
    mHeightPreferenceMin = height;
    return;
}

float Actor::Behavior::GetHeightPreferenceMin(void) {
    return mHeightPreferenceMin;
}

void Actor::Behavior::SetHeightPreferenceMax(float height) {
    mHeightPreferenceMax = height;
    return;
}

float Actor::Behavior::GetHeightPreferenceMax(void) {
    return mHeightPreferenceMax;
}


//
// Neural network

void Actor::IdiosyncraticCharacteristics::SetNeuralTopology(std::vector<NeuralLayer>& layers) {
    mux.lock();
    
    mNeuralNetwork.ClearTopology();
    
    unsigned int numberOfLayers = layers.size();
    
    for (unsigned int i=0; i < numberOfLayers; i++) {
        NeuralLayer& layer = layers[i];
        
        mNeuralNetwork.AddNeuralLayer(layer.numberOfNeurons, layer.numberOfInputs);
    }
    
    mux.unlock();
    return;
}

std::vector<float> Actor::IdiosyncraticCharacteristics::SaveNeuralStates(void) {
    mux.lock();
    std::vector<float> states = mNeuralNetwork.SaveStateBin();
    mux.unlock();
    return states;
}

void Actor::IdiosyncraticCharacteristics::LoadNeuralStates(std::vector<float>& states) {
    mux.lock();
    mNeuralNetwork.LoadStateBin( states );
    mux.unlock();
    return;
}


void Actor::IdiosyncraticCharacteristics::AddMemory(std::string name, std::string memory) {
    mMemories[name] = memory;
    return;
}

bool Actor::IdiosyncraticCharacteristics::RemoveMemory(std::string name) {
    mMemories.erase(name);
    return false;
}

void Actor::IdiosyncraticCharacteristics::ClearMemories(void) {
    mMemories.clear();
    return;
}

bool Actor::IdiosyncraticCharacteristics::CheckMemoryExists(std::string name) {
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
    return;
}

void Actor::GeneticsSystem::ClearGenome(void) {
    mGenes.clear();
    return;
}

void Actor::GeneticsSystem::ClearPhenome(void) {
    mPhen.clear();
    return;
}

unsigned int Actor::GeneticsSystem::GetNumberOfGenes(void) {
    return mGenes.size();
}

Gene Actor::GeneticsSystem::GetGeneFromGenome(unsigned int index) {
    return mGenes[index];
}

Phen Actor::GeneticsSystem::GetPhenFromPhenotype(unsigned int index) {
    return mPhen[index];
}

void Actor::GeneticsSystem::SetGeneration(unsigned int newGeneration) {
    mGeneration = newGeneration;
    return;
}

unsigned int Actor::GeneticsSystem::GetGeneration(void) {
    return mGeneration;
}

unsigned int Actor::GeneticsSystem::GetNumberOfMeshRenderers(void) {
    return mGeneticRenderers.size();
}

MeshRenderer* Actor::GeneticsSystem::GetMeshRendererAtIndex(unsigned int index) {
    return mGeneticRenderers[index];
}


void Actor::PhysicalAttributes::SetAge(unsigned long int newAge) {
    mAge = newAge;
    return;
}

unsigned long int Actor::PhysicalAttributes::GetAge(void) {
    return mAge;
}

void Actor::PhysicalAttributes::SetSeniorAge(float oldAge) {
    mAgeSenior = oldAge;
    return;
}

float Actor::PhysicalAttributes::GetSeniorAge(void) {
    return mAgeSenior;
}

void Actor::PhysicalAttributes::SetAdultAge(float age) {
    mAgeAdult = age;
    return;
}

float Actor::PhysicalAttributes::GetAdultAge(void) {
    return mAgeAdult;
}

void Actor::PhysicalAttributes::SetSpeed(float newSpeed) {
    mSpeed = newSpeed;
    return;
}

float Actor::PhysicalAttributes::GetSpeed(void) {
    return mSpeed;
}

void Actor::PhysicalAttributes::SetSpeedYouth(float newSpeed) {
    mSpeedYouth = newSpeed;
    return;
}

float Actor::PhysicalAttributes::GetSpeedYouth(void) {
    return mSpeedYouth;
}

void Actor::PhysicalAttributes::SetSpeedMultiplier(float newSpeedMul) {
    mSpeedMul = newSpeedMul;
    return;
}

float Actor::PhysicalAttributes::GetSpeedMultiplier(void) {
    return mSpeedMul;
}

void Actor::PhysicalAttributes::SetYouthScale(float scale) {
    mYouthScale = scale;
    return;
}

float Actor::PhysicalAttributes::GetYouthScale(void) {
    return mYouthScale;
}

void Actor::PhysicalAttributes::SetAdultScale(float scale) {
    mAdultScale = scale;
    return;
}

float Actor::PhysicalAttributes::GetAdultScale(void) {
    return mAdultScale;
}

void Actor::PhysicalAttributes::SetSexualOrientation(bool orientation) {
    mSexualOrientation = orientation;
    return;
}

bool Actor::PhysicalAttributes::GetSexualOrientation(void) {
    return mSexualOrientation;
}


void Actor::CooldownCounters::SetCoolDownBreeding(unsigned int counter) {
    mBreedingCoolDownCounter = counter;
    return;
}

unsigned int Actor::CooldownCounters::GetCoolDownBreeding(void) {
    return mBreedingCoolDownCounter;
}


void Actor::UserVariables::SetUserBitmask(uint8_t bitmask) {
    mBitmask = bitmask;
    return;
}

uint8_t Actor::UserVariables::GetUserBitmask(void) {
    return mBitmask;
}

void Actor::UserVariables::SetUserDataA(void* ptr) {
    mUserDataA = ptr;
    return;
}

void* Actor::UserVariables::GetUserDataA(void) {
    return mUserDataA;
}

void Actor::UserVariables::SetUserDataB(void* ptr) {
    mUserDataB = ptr;
    return;
}

void* Actor::UserVariables::GetUserDataB(void) {
    return mUserDataB;
}
