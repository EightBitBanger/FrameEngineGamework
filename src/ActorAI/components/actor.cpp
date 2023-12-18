#include <GameEngineFramework/ActorAI/components/actor.h>

extern PhysicsSystem  Physics;


Actor::Actor() : 
    name(""),
    
    isActive(true),
    
    doUpdateGenetics(false),
    
    age(0),
    
    velocity(glm::vec3(0, 0, 0)),
    position(glm::vec3(0, 0, 0))
{
    
    
    
    return;
}

void Actor::SetName(std::string newName) {
    mux.lock();
    name = newName;
    mux.unlock();
    return;
}

void Actor::SetActive(bool state) {
    mux.lock();
    isActive = state;
    mux.unlock();
    return;
}

void Actor::SetAge(unsigned long int newAge) {
    mux.lock();
    age = newAge;
    mux.unlock();
    return;
}

std::string Actor::GetName(void) {
    mux.lock();
    std::string nameString = name;
    mux.unlock();
    return nameString;
}

bool Actor::GetActive(void) {
    mux.lock();
    bool activeState = isActive;
    mux.unlock();
    return activeState;
}

unsigned long int Actor::GetAge(void) {
    mux.lock();
    unsigned long int ageValue = age;
    mux.unlock();
    return ageValue;
}

// Genetics

void Actor::AddGene(Gene& newGene) {
    mux.lock();
    mGenes.push_back( newGene );
    mux.unlock();
    return;
}

void Actor::RemoveGene(unsigned int index) {
    mux.lock();
    mGenes.erase( mGenes.begin() + index );
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
    mNeuralLayerInput = inputLayer;
    mux.unlock();
    return;
}

NeuralLayer Actor::GetNeuralInputLayer(void) {
    mux.lock();
    NeuralLayer inputLayer = mNeuralLayerInput;
    mux.unlock();
    return inputLayer;
}
