#include <GameEngineFramework/ActorAI/components/plant.h>

extern PhysicsSystem  Physics;


Plant::Plant() : 
    mName(""),
    mAge(0),
    mIsActive(true),
    mDoUpdateGenetics(false)
{
    return;
}

void Plant::SetName(std::string newName) {
    mux.lock();
    mName = newName;
    mux.unlock();
    return;
}

void Plant::SetActive(bool state) {
    mux.lock();
    mIsActive = state;
    mux.unlock();
    
    // Set renderer states
    //for (unsigned int i=0; i < mGeneticRenderers.size(); i++) 
    //    mGeneticRenderers[i]->isActive = state;
    return;
}

void Plant::SetAge(unsigned long int newAge) {
    mux.lock();
    mAge = newAge;
    mux.unlock();
    return;
}

std::string Plant::GetName(void) {
    mux.lock();
    std::string nameString = mName;
    mux.unlock();
    return nameString;
}

bool Plant::GetActive(void) {
    mux.lock();
    bool activeState = mIsActive;
    mux.unlock();
    return activeState;
}

unsigned long int Plant::GetAge(void) {
    mux.lock();
    unsigned long int ageValue = mAge;
    mux.unlock();
    return ageValue;
}

void Plant::SetPosition(glm::vec3 position) {
    mux.lock();
    mPosition = position;
    mux.unlock();
    return;
}

glm::vec3 Plant::GetPosition(void) {
    glm::vec3 position;
    mux.lock();
    position = mPosition;
    mux.unlock();
    return position;
}
