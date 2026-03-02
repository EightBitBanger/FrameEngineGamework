#include <GameEngineFramework/Engine/Engine.h>

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

int tickCounter=0;

ActorSystem::ActorSystem() : 
    sceneMain(nullptr), 
    shader(nullptr), 
    baseMesh(nullptr), 
    mPlayerPosition(0), 
    mActorUpdateDistance(400), 
    mActorRenderDistance(250), 
    mWorldWaterLevel(0.0f), 
    mNumberOfActors(0) 
{}

void ActorSystem::Initiate(void) {
    
    mMainTimer.SetRefreshRate(1);
    mAnimationTimer.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    
    mActorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> Starting thread AI" );
}

void ActorSystem::Shutdown(void) {
    std::lock_guard<std::mutex> lock(mux);
    isActorThreadActive = false;
    mActorSystemThread->join();
}

void ActorSystem::SetWaterLevel(float waterLevel) {
    std::lock_guard<std::mutex> lock(mux);
    mWorldWaterLevel = waterLevel;
}

float ActorSystem::GetWaterLevel(void) {
    std::lock_guard<std::mutex> lock(mux);
    float waterLevel = mWorldWaterLevel;
    return waterLevel;
}

void ActorSystem::SetTimeOfDay(float time) {
    mTimeOfDay = time;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> lock(mux);
    mPlayerPosition = position;
}

glm::vec3 ActorSystem::GetPlayerWorldPosition(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mPlayerPosition;
}

void ActorSystem::UpdateSendSignal(void) {
    std::lock_guard<std::mutex> lock(mux);
    doUpdate = true;
}

Actor* ActorSystem::CreateActor(void) {
    std::lock_guard<std::mutex> lock(mux);
    if (!mFreeActors.empty()) {
        Actor* actorPtr = mFreeActors[0];
        mFreeActors.erase( mFreeActors.begin() );
        actorPtr->Reset();
        mNumberOfActors++;
        mActiveActors.push_back(actorPtr);
        return actorPtr;
    }
    Actor* actorPtr = mActors.Create();
    actorPtr->Reset();
    mNumberOfActors++;
    mActiveActors.push_back(actorPtr);
    return actorPtr;
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    std::lock_guard<std::mutex> lock(mux);
    actorPtr->isGarbage = true;
    actorPtr->isActive = false;
    
    unsigned int numberOfActiveActors = mActiveActors.size();
    for (unsigned int i=0; i < numberOfActiveActors; i++) {
        if (mActiveActors[i] != actorPtr) 
            continue;
        mActiveActors.erase(mActiveActors.begin() + i);
        break;
    }
    return true;
}

unsigned int ActorSystem::GetNumberOfActors(void) {
    return mActiveActors.size();
}

Actor* ActorSystem::GetActor(unsigned int index) {
    std::lock_guard<std::mutex> lock(mux);
    return mActiveActors[index];
}


void ActorSystem::SetActorUpdateDistance(float distance) {
    std::lock_guard<std::mutex> lock(mux);
    mActorUpdateDistance = distance;
}

void ActorSystem::SetActorRenderDistance(float distance) {
    std::lock_guard<std::mutex> lock(mux);
    mActorRenderDistance = distance;
}

unsigned int ActorSystem::GetNumberOfDeadRenderers() {
    std::lock_guard<std::mutex> lock(mux);
    return mDeadActorRenderers.size();
}

MeshRenderer* ActorSystem::GetDeadRenderer(unsigned int index) {
    std::lock_guard<std::mutex> lock(mux);
    return mDeadActorRenderers[index];
}

MeshRenderer* ActorSystem::RemoveDeadRenderer(unsigned int index) {
    std::lock_guard<std::mutex> lock(mux);
    MeshRenderer* renderer = mDeadActorRenderers[index];
    mDeadActorRenderers.erase(mDeadActorRenderers.begin() + index);
    return renderer;
}

bool ActorSystem::UpdateGarbageCollection(Actor* actor) {
    if (!actor->isGarbage) 
        return false;
    actor->isGarbage = false;
    std::lock_guard<std::mutex> lock(Renderer.mux);
    ClearOldGeneticRenderers(actor);
    
    actor->navigation.mVelocity = glm::vec3(0);
    
    mNumberOfActors--;
    
    unsigned int numberOfActiveActors = mActiveActors.size();
    for (unsigned int i=0; i < numberOfActiveActors; i++) {
        if (mActiveActors[i] != actor) 
            continue;
        mActiveActors.erase(mActiveActors.begin() + i);
        break;
    }
    mFreeActors.push_back(actor);
    return true;
}

Actor* ActorSystem::Raycast(const glm::vec3& position, const glm::vec3& direction, float maxDistance) {
    glm::vec3 rayDir = glm::normalize(direction);
    
    float  closestT     = (maxDistance < 0.0f) ? 1e30f : maxDistance;
    Actor* closestActor = nullptr;
    
    const float epsilon = 1e-6f;
    
    for (Actor* actor : mActiveActors) {
        if (!actor || !actor->isActive || actor->isGarbage)
            continue;
        
        const glm::vec3 actorPos = actor->navigation.GetPosition();
        
        const glm::vec3 worldMin = actorPos + actor->GetBoundingBoxMin();
        const glm::vec3 worldMax = actorPos + actor->GetBoundingBoxMax();
        
        float tMin = 0.0f;
        float tMax = closestT;
        
        bool hit = true;
        
        for (int axis = 0; axis < 3; ++axis) {
            float origin = position[axis];
            float dir    = rayDir[axis];
            
            if (fabs(dir) < epsilon)
            {
                if (origin < worldMin[axis] || origin > worldMax[axis])
                {
                    hit = false;
                    break;
                }
            } else {
                float invDir = 1.0f / dir;
                
                float t0 = (worldMin[axis] - origin) * invDir;
                float t1 = (worldMax[axis] - origin) * invDir;
                
                if (t0 > t1)
                    std::swap(t0, t1);
                
                if (t0 > tMin) tMin = t0;
                if (t1 < tMax) tMax = t1;
                
                if (tMax < tMin)
                {
                    hit = false;
                    break;
                }
            }
        }
        
        if (!hit)
            continue;
        
        // If ray started inside the box, tMin will be negative.
        float hitT = (tMin >= 0.0f) ? tMin : tMax;
        
        if (hitT >= 0.0f && hitT < closestT) {
            closestT     = hitT;
            closestActor = actor;
        }
    }
    
    return closestActor;
}


//
// Actor system thread
//

void actorThreadMain() {
    
    while (isActorThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        if (!doUpdate) 
            continue;
        
        AI.Update();
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(100) );
    Log.Write( " >> Shutting down on thread AI" );
}

