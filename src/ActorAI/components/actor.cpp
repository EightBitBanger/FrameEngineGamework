#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;
extern RenderSystem   Renderer;


Actor::Actor() : 
    
    mName(""),
    
    mAge(0),
    mGeneration(0),
    mYouthScale(0.3f),
    mAdultScale(1.3f),
    
    mColliderOffset(glm::vec3(0)),
    mColliderScale(glm::vec3(1)),
    
    mSpeed(1.5f),
    mSpeedYouth(0.3f),
    
    mSpeedMul(1.3f),
    mSnapSpeed(0.24f),
    
    mVelocity(glm::vec3(0)),
    mPosition(glm::vec3(0)),
    mRotation(glm::vec3(0)),
    mRotateTo(glm::vec3(0)),
    mTargetPoint(glm::vec3(0, 0, 0)),
    mDistance(0),
    
    mIsActive(true),
    
    mDoUpdateGenetics(false),
    mDoReexpressGenetics(false),
    
    mIsWalking(false),
    mIsRunning(false),
    mIsAttacking(false),
	mIsFleeing(false),
	mIsConsuming(false),
	mIsFacing(true),
	
	mChanceToChangeDirection(40),
	mChanceToFocusOnActor   (1),
	mChanceToWalk           (1200),
    mChanceToStopWalking    (0),
	
	mDistanceToFocusOnActor (10),
    mDistanceToWalk         (30),
    mDistanceToAttack       (30),
	mDistanceToFlee         (20),
    
    mHeightPreferenceMin    (0.0f),
	mHeightPreferenceMax    (1000),
    
    mObservationCoolDownCounter(0),
    mMovementCoolDownCounter(0),
    
    mBitmask(0),
    
    mUserDataA(nullptr),
    mUserDataB(nullptr),
    
    mBreedWithActor(nullptr)
{
}

void Actor::SetName(std::string newName) {
    mux.lock();
    mName = newName;
    mux.unlock();
    return;
}

void Actor::SetActive(bool state) {
    mux.lock();
    mIsActive = state;
    mux.unlock();
    
    for (unsigned int i=0; i < mGeneticRenderers.size(); i++) 
        mGeneticRenderers[i]->isActive = state;
    return;
}

void Actor::SetGeneticUpdateFlag(void) {
    
    mDoUpdateGenetics = true;
    
    return;
}

void Actor::SetGeneticExpressionFlag(void) {
    
    mDoReexpressGenetics = true;
    
    return;
}

std::string Actor::GetName(void) {
    mux.lock();
    std::string nameString = mName;
    mux.unlock();
    return nameString;
}

bool Actor::GetActive(void) {
    mux.lock();
    bool activeState = mIsActive;
    mux.unlock();
    return activeState;
}

void Actor::SetAge(unsigned long int newAge) {
    mux.lock();
    mAge = newAge;
    mux.unlock();
    return;
}

unsigned long int Actor::GetAge(void) {
    mux.lock();
    unsigned long int ageValue = mAge;
    mux.unlock();
    return ageValue;
}

void Actor::SetGeneration(unsigned int newGeneration) {
    mux.lock();
    mGeneration = newGeneration;
    mux.unlock();
    return;
}

unsigned int Actor::GetGeneration(void) {
    mux.lock();
    unsigned int generation = mGeneration;
    mux.unlock();
    return generation;
}

void Actor::SetSpeed(float newSpeed) {
    mux.lock();
    mSpeed = newSpeed;
    mux.unlock();
    return;
}

float Actor::GetSpeed(void) {
    mux.lock();
    float speedValue = mSpeed;
    mux.unlock();
    return speedValue;
}

void Actor::SetSpeedYouth(float newSpeed) {
    mux.lock();
    mSpeedYouth = newSpeed;
    mux.unlock();
    return;
}

float Actor::GetSpeedYouth(void) {
    mux.lock();
    float speedValue = mSpeedYouth;
    mux.unlock();
    return speedValue;
}

void Actor::SetSpeedMultiplier(float newSpeedMul) {
    mux.lock();
    mSpeedMul = newSpeedMul;
    mux.unlock();
    return;
}

float Actor::GetSpeedMultiplier(void) {
    mux.lock();
    float speedMul = mSpeedMul;
    mux.unlock();
    return speedMul;
}

void Actor::SetPosition(glm::vec3 position) {
    mux.lock();
    mPosition = position;
    mux.unlock();
    return;
}

glm::vec3 Actor::GetPosition(void) {
    mux.lock();
    glm::vec3 position = mPosition;
    mux.unlock();
    return position;
}

void Actor::SetTargetPoint(glm::vec3 position) {
    mux.lock();
    mTargetPoint = position;
    mux.unlock();
    return;
}

glm::vec3 Actor::GetTargetPoint(void) {
    mux.lock();
    glm::vec3 position = mTargetPoint;
    mux.unlock();
    return position;
}

unsigned int Actor::GetNumberOfMeshRenderers(void) {
    return mGeneticRenderers.size();
}

MeshRenderer* Actor::GetMeshRendererAtIndex(unsigned int index) {
    return mGeneticRenderers[index];
}

void Actor::SetUserBitmask(uint8_t bitmask) {
    mBitmask = bitmask;
    return;
}

uint8_t Actor::GetUserBitmask(void) {
    return mBitmask;
}

void Actor::SetUserDataA(void* ptr) {
    mUserDataA = ptr;
    return;
}

void* Actor::GetUserDataA(void) {
    return mUserDataA;
}

void Actor::SetUserDataB(void* ptr) {
    mUserDataB = ptr;
    return;
}

void* Actor::GetUserDataB(void) {
    return mUserDataB;
}


// Genetics


unsigned int Actor::AddGene(Gene& newGene) {
    mGenes.push_back( newGene );
    mDoUpdateGenetics = true;
    return mGenes.size();
}

void Actor::RemoveGene(unsigned int index) {
    mGenes.erase( mGenes.begin() + index );
    mDoUpdateGenetics = true;
    return;
}

void Actor::ClearGenome(void) {
    mGenes.clear();
    return;
}

unsigned int Actor::GetNumberOfGenes(void) {
    unsigned int sizeValue = mGenes.size();
    return sizeValue;
}

Gene Actor::GetGeneFromGenome(unsigned int index) {
    Gene geneValue = mGenes[index];
    return geneValue;
}

Actor* Actor::GetBreedWithActor(void) {
    return mBreedWithActor;
}

void Actor::SetBreedWithActor(Actor* actorPtr) {
    mBreedWithActor = actorPtr;
    return;
}


// Neural networking


void Actor::AddWeightedLayer(WeightedLayer& newNeuralLayer) {
    mux.lock();
    mWeightedLayers.push_back( newNeuralLayer );
    mux.unlock();
    return;
}

void Actor::RemoveWeightedLayer(unsigned int index) {
    mux.lock();
    mWeightedLayers.erase( mWeightedLayers.begin() + index );
    mux.unlock();
    return;
}

void Actor::ClearWeightedLayers(void) {
    mux.lock();
    mWeightedLayers.clear();
    mux.unlock();
    return;
}

unsigned int Actor::GetNumberOfWeightedLayers(void) {
    mux.lock();
    unsigned int sizeValue = mWeightedLayers.size();
    mux.unlock();
    return sizeValue;
}

WeightedLayer Actor::GetWeightedLayerFromNetwork(unsigned int index) {
    mux.lock();
    WeightedLayer layer = mWeightedLayers[index];
    mux.unlock();
    return layer;
}

void Actor::SetNeuralInputLayer(NeuralLayer inputLayer) {
    mux.lock();
    for (int i=0; i < NEURAL_LAYER_WIDTH; i++) 
        mWeightedLayers[0].node[i] = inputLayer.node[i];
    mux.unlock();
    return;
}


// AI state behavioral hardwiring


void Actor::SetChanceToChangeDirection(float chance) {
    mux.lock();
    mChanceToChangeDirection = chance;
    mux.unlock();
    return;
}

float Actor::GetChanceToChangeDirection(void) {
    mux.lock();
    float chance = mChanceToChangeDirection;
    mux.unlock();
    return chance;
}

void Actor::SetChanceToFocusOnActor(float chance) {
    mux.lock();
    mChanceToFocusOnActor = chance;
    mux.unlock();
    return;
}

float Actor::GetChanceToFocusOnActor(void) {
    mux.lock();
    float chance = mChanceToFocusOnActor;
    mux.unlock();
    return chance;
}

void Actor::SetChanceToWalk(float chance) {
    mux.lock();
    mChanceToWalk = chance;
    mux.unlock();
    return;
}

float Actor::GetChanceToWalk(void) {
    mux.lock();
    float chance = mChanceToWalk;
    mux.unlock();
    return chance;
}

void Actor::SetChanceToStopWalking(float chance) {
    mux.lock();
    mChanceToStopWalking = chance;
    mux.unlock();
    return;
}

float Actor::GetChanceToStopWalking(void) {
    mux.lock();
    float chance = mChanceToStopWalking;
    mux.unlock();
    return chance;
}

void Actor::SetDistanceToWalk(float distance) {
    mux.lock();
    mDistanceToWalk = distance;
    mux.unlock();
    return;
}

float Actor::GetDistanceToWalk(void) {
    mux.lock();
    float distance = mDistanceToWalk;
    mux.unlock();
    return distance;
}

void Actor::SetDistanceToAttack(float distance) {
    mux.lock();
    mDistanceToAttack = distance;
    mux.unlock();
    return;
}

float Actor::GetDistanceToAttack(void) {
    mux.lock();
    float distance = mDistanceToAttack;
    mux.unlock();
    return distance;
}

void Actor::SetDistanceToFlee(float distance) {
    mux.lock();
    mDistanceToFlee = distance;
    mux.unlock();
    return;
}

float Actor::GetDistanceToFlee(void) {
    mux.lock();
    float distance = mDistanceToFlee;
    mux.unlock();
    return distance;
}

void Actor::SetHeightPreferenceMin(float height) {
    mux.lock();
    mHeightPreferenceMin = height;
    mux.unlock();
    return;
}

float Actor::GetHeightPreferenceMin(void) {
    mux.lock();
    float height = mHeightPreferenceMin;
    mux.unlock();
    return height;
}

void Actor::SetHeightPreferenceMax(float height) {
    mux.lock();
    mHeightPreferenceMax = height;
    mux.unlock();
    return;
}

float Actor::GetHeightPreferenceMax(void) {
    mux.lock();
    float height = mHeightPreferenceMax;
    mux.unlock();
    return height;
}

void Actor::AddMemory(std::string memory) {
    mMemories.push_back( memory );
    return;
}

bool Actor::RemoveMemory(std::string memory) {
    for (unsigned int i=0; i < mMemories.size(); i++) {
        if (mMemories[i] == memory) {
            mMemories.erase( mMemories.begin() + i );
            return true;
        }
    }
    return false;
}

void Actor::ClearMemories(void) {
    mMemories.clear();
    return;
}

bool Actor::CheckMemoryExists(std::string memory) {
    for (unsigned int i=0; i < mMemories.size(); i++) 
        if (mMemories[i] == memory) 
            return true;
    return false;
}



void Actor::SetYouthScale(float scale) {
    
    mYouthScale = scale;
    
    return;
}

float Actor::GetYouthScale(void) {
    
    return mYouthScale;
}

void Actor::SetAdultScale(float scale) {
    
    mAdultScale = scale;
    
    return;
}

float Actor::GetAdultScale(void) {
    
    return mAdultScale;
}
