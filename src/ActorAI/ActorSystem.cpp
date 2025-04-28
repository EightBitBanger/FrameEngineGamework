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
int tickCounter=0;


ActorSystem::ActorSystem() : 
    sceneMain(nullptr),
    shader(nullptr),
    baseMesh(nullptr),
    SpawnActor(nullptr),
    KillActor(nullptr),
    mPlayerPosition(0),
    mActorUpdateDistance(300),
    mWorldWaterLevel(0.0f)
{
}

void ActorSystem::Initiate(void) {
    
    mMainTimer.SetRefreshRate(20);
    mAnimationTimer.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    
    mActorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> Starting thread AI" );
    
    return;
}

void ActorSystem::Shutdown(void) {
    std::lock_guard<std::mutex> lock(mux);
    isActorThreadActive = false;
    mActorSystemThread->join();
    return;
}

void ActorSystem::SetWaterLevel(float waterLevel) {
    std::lock_guard<std::mutex> lock(mux);
    mWorldWaterLevel = waterLevel;
    return;
}

float ActorSystem::GetWaterLevel(void) {
    std::lock_guard<std::mutex> lock(mux);
    float waterLevel = mWorldWaterLevel;
    return waterLevel;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> lock(mux);
    mPlayerPosition = position;
    return;
}

glm::vec3 ActorSystem::GetPlayerWorldPosition(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mPlayerPosition;
}

void ActorSystem::UpdateSendSignal(void) {
    std::lock_guard<std::mutex> lock(mux);
    doUpdate = true;
    return;
}

Actor* ActorSystem::CreateActor(void) {
    return mActors.Create();
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    std::lock_guard<std::mutex> lock(mux);
    return mActors.Destroy(actorPtr);
}

bool ActorSystem::AddActorToSimulation(Actor* actorPtr) {
    // Check already exists in the simulation
    for (unsigned int i=0; i < mActiveActors.size(); i++) {
        Actor* actor = mActiveActors[i];
        if (actor == actorPtr) 
            return false;
    }
    mActiveActors.push_back( actorPtr );
    return true;
}

bool ActorSystem::RemoveActorFromSimulation(Actor* actorPtr) {
    for (unsigned int i=0; i < mActiveActors.size(); i++) {
        Actor* actor = mActiveActors[i];
        if (actor != actorPtr) 
            continue;
        mActiveActors.erase( mActiveActors.begin() + i);
        return true;
    }
    return false;
}

Actor* ActorSystem::GetActorFromSimulation(unsigned int index) {
    if (index < mActiveActors.size()) 
        return mActiveActors[index];
    return nullptr;
}

unsigned int ActorSystem::GetNumberOfActors(void) {
    return mActiveActors.size();
}

Actor* ActorSystem::GetActor(unsigned int index) {
    std::lock_guard<std::mutex> lock(mux);
    return mActors[index];
}


void ActorSystem::SetActorUpdateDistance(float distance) {
    std::lock_guard<std::mutex> lock(mux);
    mActorUpdateDistance = distance;
    return;
}



//
// Actor system thread
//

void actorThreadMain() {
    
    while (isActorThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(1) );
        
        if (doUpdate) 
            AI.Update();
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(100) );
    
    Log.Write( " >> Shutting down on thread AI" );
    
    return;
}

