#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

// Actor system thread
bool isActorThreadActive = true;
bool doUpdate = false;
void actorThreadMain(void);

int actorCounter=0;


ActorSystem::ActorSystem() : 
    mPlayerPosition(0),
    mActorUpdateDistance(300),
    mActorDetailDistance(60)
{
}

void ActorSystem::Initiate(void) {
    
    mActorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> Starting thread AI" );
    
    return;
}

void ActorSystem::Shutdown(void) {
    
    mux.lock();
    isActorThreadActive = false;
    mux.unlock();
    
    mActorSystemThread->join();
    
    return;
}

void ActorSystem::SetActorDetailDistance(float position) {
    mActorDetailDistance = position;
    return;
}

float ActorSystem::GetActorDetailDistance(void) {
    return mActorDetailDistance;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    mux.lock();
    mPlayerPosition = position;
    mux.unlock();
    return;
}

glm::vec3 ActorSystem::GetPlayerWorldPosition(void) {
    mux.lock();
    glm::vec3 position(mPlayerPosition);
    mux.unlock();
    return position;
}

void ActorSystem::UpdateSendSignal(void) {
    mux.lock();
    doUpdate = true;
    mux.unlock();
    return;
}

Actor* ActorSystem::CreateActor(void) {
    mux.lock();
    Actor* newActor = mActors.Create();
    mux.unlock();
    return newActor;
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    mux.lock();
    bool state = mActors.Destroy(actorPtr);
    mux.unlock();
    return state;
}


unsigned int ActorSystem::GetNumberOfActors(void) {
    mux.lock();
    unsigned int value = mActors.GetObjectCount();
    mux.unlock();
    return value;
}

Actor* ActorSystem::GetActor(unsigned int index) {
    mux.lock();
    Actor* actorPtr = mActors[index];
    mux.unlock();
    return actorPtr;
}


void ActorSystem::SetActorUpdateDistance(float distance) {
    mux.lock();
    mActorUpdateDistance = distance;
    mux.unlock();
    return;
}



//
// Actor system thread
//

void actorThreadMain() {
    
    while (isActorThreadActive) {
        
        if (!doUpdate) {
            std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
            continue;
        }
        
        AI.Update();
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
    Log.Write( " >> Shutting down on thread AI" );
    
    return;
}

