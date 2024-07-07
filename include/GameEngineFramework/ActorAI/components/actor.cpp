#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;


Actor::Actor() : 
    
    mName(""),
    
    mAge(0),
    mAdultAge(300),
    mYouthScale(0.4),
    mAdultScale(1.0),
    
    mSpeed(1.5f),
    mSpeedMul(1.3),
    mSnapSpeed(0.24),
    
    mVelocity(glm::vec3(0, 0, 0)),
    mPosition(glm::vec3(0, 0, 0)),
    mRotation(glm::vec3(0, 0, 0)),
    mRotateTo(glm::vec3(0, 0, 0)),
    mTargetPoint(glm::vec3(0, 0, 0)),
    mDistance(0),
    
    mIsActive(true),
    
    mDoUpdateGenetics(false),
    
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
    
    mHeightPreferenceMin    (0),
	mHeightPreferenceMax    (0),
    
    mObservationCoolDownCounter(0),
    
    mIsActorActiveInScene(false)
{
    return;
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

void Actor::SetAge(unsigned long int newAge) {
    mux.lock();
    mAge = newAge;
    mux.unlock();
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

unsigned long int Actor::GetAge(void) {
    mux.lock();
    unsigned long int ageValue = mAge;
    mux.unlock();
    return ageValue;
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
    glm::vec3 position;
    mux.lock();
    position = mPosition;
    mux.unlock();
    return position;
}

// Genetics


unsigned int Actor::AddGene(Gene& newGene) {
    mux.lock();
    mGenes.push_back( newGene );
    mDoUpdateGenetics = true;
    mux.unlock();
    return mGenes.size();
}

void Actor::RemoveGene(unsigned int index) {
    mux.lock();
    mGenes.erase( mGenes.begin() + index );
    mDoUpdateGenetics = true;
    mux.unlock();
    return;
}

unsigned int Actor::GetNumberOfGenes(void) {
    mux.lock();
    unsigned int sizeValue = mGenes.size();
    mux.unlock();
    return sizeValue;
}

Gene Actor::GetGeneFromGenome(unsigned int index) {
    mux.lock();
    Gene geneValue = mGenes[index];
    mux.unlock();
    return geneValue;
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

void Actor::SetChanceToFocusOnActor(float chance) {
    mux.lock();
    mChanceToFocusOnActor = chance;
    mux.unlock();
    return;
}

void Actor::SetChanceToWalk(float chance) {
    mux.lock();
    mChanceToWalk = chance;
    mux.unlock();
    return;
}

void Actor::SetChanceToStopWalking(float chance) {
    mux.lock();
    mChanceToStopWalking = chance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToWalk(float distance) {
    mux.lock();
    mDistanceToWalk = distance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToAttack(float distance) {
    mux.lock();
    mDistanceToAttack = distance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToFlee(float distance) {
    mux.lock();
    mDistanceToFlee = distance;
    mux.unlock();
    return;
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

bool Actor::CheckMemoryExists(std::string memory) {
    for (unsigned int i=0; i < mMemories.size(); i++) 
        if (mMemories[i] == memory) 
            return true;
    return false;
}

