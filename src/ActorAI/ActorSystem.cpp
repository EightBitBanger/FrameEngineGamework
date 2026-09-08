#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

#include <fstream>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;
UniversalConstants UniversalConst;

// Actor system thread
bool isActorThreadActive = true;
bool doUpdate = false;

void actorThreadMain(void);
void actorThreadThinking(void);

ActorSystem::ActorSystem() : 
    sceneMain(nullptr), 
    shader(nullptr), 
    baseMesh(nullptr), 
    mPlayerPosition(0), 
    mActorUpdateDistance(400), 
    mActorRenderDistance(250), 
    mWorldWaterLevel(0.0f), 
    mTimeOfDay(0.0f), 
    mDebugLineRenderer(nullptr), 
    mNumberOfActors(0),
    mTimeScale(0.5f),
    mFrameTimeCurrent(0.0d)
{}

void ActorSystem::Initiate(void) {
    mTimeScale = 0.5f;
    
    mainTimer.SetRefreshRate(20.0f);
    auxiliaryTimer.SetRefreshRate(30.0f);
    animationTimer.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    
    // Fire up the actor system & animation thread
    mActorSystemThread = new std::thread( actorThreadMain );
    mAnimationThread   = new std::thread( actorThreadThinking );
    Log.Write( " >> Starting thread AI" );
}

void ActorSystem::SetTimeScale(float scale) {
    std::lock_guard<std::mutex> lock(mux);
    mTimeScale = (scale < 0.01f) ? 0.01f : scale; // Avoid division by zero or negative time
    mainTimer.SetRefreshRate(static_cast<int>(20.0f * mTimeScale));
    animationTimer.SetRefreshRate(static_cast<int>(RENDER_FRAMES_PER_SECOND * mTimeScale));
    auxiliaryTimer.SetRefreshRate(static_cast<int>(30.0f * mTimeScale));
}

float ActorSystem::GetTimeScale(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mTimeScale;
}
bool ActorSystem::DebugRendererEnable(void) {
    if (mDebugLineRenderer == nullptr) {
        mDebugLineRenderer = Renderer.CreateMeshRenderer();
        sceneMain->AddMeshRendererToSceneRoot(mDebugLineRenderer, RENDER_QUEUE_FOREGROUND);
        
        mDebugLineRenderer->mesh = Renderer.CreateMesh();
        mDebugLineRenderer->mesh->SetPrimitive(MESH_LINES);
        
        mDebugLineRenderer->material = Renderer.CreateMaterial();
        mDebugLineRenderer->material->shader = Resources.shaders.color;
    }
    
    mDebugLineRenderer->isActive = true;
    return true;
}

void ActorSystem::DebugRendererDisable(void) {
    if (mDebugLineRenderer == nullptr) 
        return;
    
    mDebugLineRenderer->isActive = false;
    sceneMain->RemoveMeshRendererFromSceneRoot(mDebugLineRenderer, RENDER_QUEUE_FOREGROUND);
    Renderer.DestroyMeshRenderer(mDebugLineRenderer);
    mDebugLineRenderer = nullptr;
}

void ActorSystem::DebugRenderDrawLine(glm::vec3 from, glm::vec3 to, Color color) {
    if (mDebugLineRenderer == nullptr) 
        return;
    
    mDebugLineRenderer->mesh->AddLine(from, to, color);
    mDebugLineRenderer->mesh->Load();
}

void ActorSystem::SetWorldPickupCallbacks(WorldPickupQueryCallback query, WorldPickupPlaceCallback place, WorldPickupRemoveCallback destroy) {
    mWorldPickupQueryCallback = std::move(query);
    mWorldPickupPlaceCallback   = std::move(place);
    mWorldPickupRemoveCallback  = std::move(destroy);
}

void ActorSystem::SetWorldStaticCallbacks(WorldStaticRaycastCallback query, WorldStaticPlaceCallback place, WorldStaticRemoveCallback destroy) {
    mWorldStaticQueryCallback = std::move(query);
    mWorldStaticPlaceCallback   = std::move(place);
    mWorldStaticRemoveCallback  = std::move(destroy);
}

void ActorSystem::SetNameGenerator(WorldGetNameCallback getter) {
    mWorldGetNameCallback = std::move(getter);
}

void ActorSystem::Shutdown(void) {
    std::lock_guard<std::mutex> lock(mux);
    isActorThreadActive = false;
    mActorSystemThread->join();
    mAnimationThread->join();
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

float ActorSystem::GetTimeOfDay(void) {
    return mTimeOfDay;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> lock(mux);
    mPlayerPosition = position;
}

glm::vec3 ActorSystem::GetPlayerWorldPosition(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mPlayerPosition;
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

void ActorSystem::SwapDeadRendererList(std::vector<MeshRenderer*>& newList) {
    std::lock_guard<std::mutex> lock(mux);
    mDeadActorRenderers.swap(newList);
}

bool ActorSystem::UpdateGarbageCollection(Actor* actor) {
    if (!actor->isGarbage) 
        return false;
    actor->isGarbage = false;
    std::lock_guard<std::mutex> lock(Renderer.mux);
    ClearOldGeneticRenderers(actor);
    actor->navigation.mVelocity = glm::vec3(0);
    
    mNumberOfActors--;
    
    // Destroy currently holding item
    if (actor->inventory.holdingRenderer != nullptr) {
        sceneMain->RemoveMeshRendererFromSceneRoot(actor->inventory.holdingRenderer, RENDER_QUEUE_GEOMETRY);
        Renderer.DestroyMeshRenderer(actor->inventory.holdingRenderer);
        actor->inventory.holdingRenderer = nullptr;
    }
    
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
            
            if (fabs(dir) < epsilon) {
                if (origin < worldMin[axis] || origin > worldMax[axis]) {
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
                
                if (tMax < tMin) {
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

void ActorSystem::RecordBirth(Actor* child, Actor* father, Actor* mother, const std::string& familyName, const glm::vec3& position) {
    std::lock_guard<std::mutex> lock(mux);
    
    Genealogy entry;
    entry.eventType  = GenealogyEventType::Birth;
    entry.actor      = child->GetName().empty() ? "Actor_" + std::to_string(reinterpret_cast<uintptr_t>(child)) : child->GetName();
    entry.father     = father->GetName().empty() ? "Actor_" + std::to_string(reinterpret_cast<uintptr_t>(father)) : father->GetName();
    entry.mother     = mother->GetName().empty() ? "Actor_" + std::to_string(reinterpret_cast<uintptr_t>(mother)) : mother->GetName();
    entry.family     = familyName.empty() ? "Independent" : familyName;
    entry.generation = child->genetics.GetGeneration();
    entry.age        = child->physical.GetAge();
    entry.position   = position;
    entry.details    = "Birth";
    
    mGenealogy.push_back(entry);
}

void ActorSystem::RecordDeath(Actor* actor, const std::string& cause) {
    if (!actor) return;
    std::lock_guard<std::mutex> lock(mux);
    
    if (actor->isGarbage) return;
    
    Genealogy entry;
    entry.eventType  = GenealogyEventType::Death;
    entry.actor      = actor->GetName().empty() ? "Actor_" + std::to_string(reinterpret_cast<uintptr_t>(actor)) : actor->GetName();
    entry.family     = actor->memories.Get("family").empty() ? "Independent" : actor->memories.Get("family");
    entry.father     = "";
    entry.mother     = "";
    entry.generation = actor->genetics.GetGeneration();
    entry.age        = actor->physical.GetAge();
    entry.position   = actor->navigation.GetPosition();
    entry.details    = cause;
    
    mGenealogy.push_back(entry);
}

bool ActorSystem::DumpGenealogy(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mux);
    
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        Log.Write(" [!] Error: Failed to open genealogy export file: " + filename);
        return false;
    }
    
    file << "Event,Actor,Family,Generation,Age,Father,Mother,Details,PosX,PosY,PosZ\n";
    
    for (const Genealogy& entry : mGenealogy) {
        file << (entry.eventType == GenealogyEventType::Birth ? "BIRTH" : "DEATH") << ","
            << entry.actor      << ","
            << entry.family     << ","
            << entry.generation << ","
            << entry.age        << ","
            << entry.father     << ","
            << entry.mother     << ","
            << "\"" << entry.details << "\","
            << entry.position.x << ","
            << entry.position.y << ","
            << entry.position.z << "\n";
    }
    
    file.close();
    Log.Write(" >> Unified genealogy tree successfully exported to: " + filename);
    return true;
}

std::vector<Genealogy> ActorSystem::GetGenealogy(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mGenealogy;
}

void ActorSystem::ClearGenealogy(void) {
    std::lock_guard<std::mutex> lock(mux);
    mGenealogy.clear();
}

//
// Actor system thread
//

void actorThreadMain(void) {
    while (isActorThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        if (AI.animationTimer.Update()) 
            AI.UpdateFast();
        
        if (AI.mainTimer.Update()) 
            AI.UpdateTick();
        
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(100) );
    Log.Write( " >> Shutting down on thread AI" );
}

void actorThreadThinking(void) {
    while (isActorThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        if (AI.auxiliaryTimer.Update()) 
            AI.UpdateThinking();
        
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(150) );
    Log.Write( " >> Shutting down on thread AI::Animation" );
}
