#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern EngineSystemManager Engine;
extern MathCore Math;

extern bool isActorThreadActive;
extern bool doUpdate;

extern int tickCounter;


void ActorSystem::Update(void) {
    if (mAnimationTimer.Update()) 
        UpdateFast();
    
    if (mMainTimer.Update()) 
        UpdateTick();
}


void ActorSystem::UpdateFast() {
    std::lock_guard<std::mutex> lock(Renderer.mux);
    
    unsigned int numberOfActors = mActiveActors.size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActiveActors[i];
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        UpdateActorGenetics(actor);
        ExpressActorGenetics(actor);
        
        // Cycle the animation states
        UpdateAnimationState(actor);
        
        // Target tracking orientations
        UpdateTargetRotation(actor);
        HandleMovementMechanics(actor);
        
        // Update actor mechanical / locomotion state
        HandleMovementMechanics(actor);
        HandleTargettingMechanics(actor);
        
        HandleVitality(actor);
    }
}


void ActorSystem::UpdateTick() {
    unsigned int numberOfActors = mActors.Size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        // Cull updates for far away actors
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        if (distance > mActorUpdateDistance) 
            continue;
        
        UpdateProximityList(actor);
        UpdateActorState(actor);
        
        UpdateActorMemories(actor);
        
        HandleCooldownCounters(actor);
    }
    
    // Garbage collection pass
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        
        // Check garbage actors
        if (UpdateGarbageCollection(actor)) 
            continue;
    }
    
}

