#include "actor.h"

extern PhysicsSystem  Physics;


Actor::Actor() : 
    name(""),
    isActive(true),
    doUpdateGenetics(false),
    age(0),
    
    chanceToMove(0),
    chanceToLookAtPlayer(0),
    chanceToLookAtEntity(0),
    chanceToStopMoving(0),
    
    distanceToMove(0),
    distanceToLookAtPlayer(0),
    distanceToLookAtEntity(0),
    distanceToStopMoving(0),
    
    velocity(glm::vec3(0, 0, 0))
{
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

void Actor::SetPosition(glm::vec3 newPosition) {
    mux.lock();
    worldPosition = newPosition;
    mux.unlock();
    return;
}

glm::vec3 Actor::GetPosition(void) {
    mux.lock();
    glm::vec3 currentPosition = worldPosition;
    mux.unlock();
    return currentPosition;
}

void Actor::SetChanceToMove(int chance) {
    mux.lock();
    chanceToMove = chance;
    mux.unlock();
    return;
}

void Actor::SetChanceToLookAtPlayer(int chance) {
    mux.lock();
    chanceToLookAtPlayer = chance;
    mux.unlock();
    return;
}

void Actor::SetChanceToLookAtEntity(int chance) {
    mux.lock();
    chanceToLookAtEntity = chance;
    mux.unlock();
    return;
}

void Actor::SetChanceToStopMoving(int chance) {
    mux.lock();
    chanceToStopMoving = chance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToMove(float distance) {
    mux.lock();
    distanceToMove = distance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToLookAtPlayer(float distance) {
    mux.lock();
    distanceToLookAtPlayer = distance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToLookAtEntity(float distance) {
    mux.lock();
    distanceToLookAtEntity = distance;
    mux.unlock();
    return;
}

void Actor::SetDistanceToStopMoving(float distance) {
    mux.lock();
    distanceToStopMoving = distance;
    mux.unlock();
    return;
}

int Actor::GetChanceToMove(void) {
    mux.lock();
    int chance = chanceToMove;
    mux.unlock();
    return chance;
}

int Actor::GetChanceToLookAtPlayer(void) {
    mux.lock();
    int chance = chanceToLookAtPlayer;
    mux.unlock();
    return chance;
}

int Actor::GetChanceToLookAtEntity(void) {
    mux.lock();
    int chance = chanceToLookAtEntity;
    mux.unlock();
    return chance;
}

int Actor::GetChanceToStopMoving(void) {
    mux.lock();
    int chance = chanceToStopMoving;
    mux.unlock();
    return chance;
}

float Actor::GetDistanceToMove(void) {
    mux.lock();
    int dist = distanceToMove;
    mux.unlock();
    return dist;
}

float Actor::GetDistanceToLookAtPlayer(void) {
    mux.lock();
    int dist = distanceToLookAtPlayer;
    mux.unlock();
    return dist;
}

float Actor::GetDistanceToLookAtEntity(void) {
    mux.lock();
    int dist = distanceToLookAtEntity;
    mux.unlock();
    return dist;
}

float Actor::GetDistanceToStopMoving(void) {
    mux.lock();
    int dist = distanceToStopMoving;
    mux.unlock();
    return dist;
}

glm::vec3 Actor::GetVelocity(void) {
    mux.lock();
    glm::vec3 velocityVec = velocity;
    mux.unlock();
    return velocityVec;
}

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
