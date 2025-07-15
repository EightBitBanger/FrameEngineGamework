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
    mPlayerPosition(0),
    mActorUpdateDistance(300),
    mWorldWaterLevel(0.0f),
    mNumberOfActors(0)
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
    std::lock_guard<std::mutex> lock(mux);
    if (!mGarbageActors.empty()) {
        Actor* actor = mGarbageActors[0];
        mGarbageActors.erase(mGarbageActors.begin());
        mNumberOfActors++;
        actor->Reset();
        return actor;
    }
    Actor* actorPtr = mActors.Create();
    actorPtr->Reset();
    mNumberOfActors++;
    return actorPtr;
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    std::lock_guard<std::mutex> lock(mux);
    if (actorPtr->colliderBody != nullptr) {
        extern PhysicsSystem Physics;
        Physics.DestroyCollisionBody(actorPtr->colliderBody);
        actorPtr->colliderBody = nullptr;
    }
    actorPtr->isGarbage = true;
    actorPtr->isActive = false;
    return true;
}

Actor* ActorSystem::GetActorFromSimulation(unsigned int index) {
    if (index < mActors.Size()) 
        return mActors[index];
    return nullptr;
}

unsigned int ActorSystem::GetNumberOfActors(void) {
    return mActors.Size();
}

unsigned int ActorSystem::GetNumberOfActiveActors(void) {
    return mNumberOfActors;
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


bool ActorSystem::UpdateGarbageCollection(Actor* actor) {
    if (!actor->isGarbage) 
        return false;
    
    // Destroy the renderers
    ClearOldGeneticRenderers(actor);
    
    //actor->Reset();
    actor->navigation.mVelocity = glm::vec3(0);
    
    mGarbageActors.push_back(actor);
    mNumberOfActors--;
    return true;
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

