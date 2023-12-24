#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;


Actor::Actor() : 
    mName(""),
    
    mIsActive(true),
    
    mDoUpdateGenetics(false),
    
    mAge(0),
    
    mVelocity(glm::vec3(0, 0, 0)),
    mPosition(glm::vec3(0, 0, 0))
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

// Genetics

void Actor::AddGene(Gene& newGene) {
    mux.lock();
    mGenes.push_back( newGene );
    mDoUpdateGenetics = true;
    mux.unlock();
    return;
}

void Actor::RemoveGene(unsigned int index) {
    mux.lock();
    mGenes.erase( mGenes.begin() + index );
    mDoUpdateGenetics = true;
    mux.unlock();
    return;
}

Gene Actor::GetGeneFromGenome(unsigned int index) {
    mux.lock();
    Gene geneValue = mGenes[index];
    mux.unlock();
    return geneValue;
}

unsigned int Actor::GetNumberOfGenes(void) {
    mux.lock();
    unsigned int sizeValue = mGenes.size();
    mux.unlock();
    return sizeValue;
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

WeightedLayer Actor::GetWeightedLayer(unsigned int index) {
    mux.lock();
    WeightedLayer layer = mWeightedLayers[index];
    mux.unlock();
    return layer;
}

unsigned int Actor::GetNumberOfWeightedLayers(void) {
    mux.lock();
    unsigned int sizeValue = mWeightedLayers.size();
    mux.unlock();
    return sizeValue;
}

void Actor::SetNeuralInputLayer(NeuralLayer inputLayer) {
    mux.lock();
    for (int i=0; i < NEURAL_LAYER_WIDTH; i++) 
        mWeightedLayers[0].node[i] = inputLayer.node[i];
    mux.unlock();
    return;
}
