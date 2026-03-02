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
        
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        
        // Genetic expression update
        UpdateActorGenetics(actor);
        ExpressActorGenetics(actor);
        
        // Update actor mechanical / locomotion
        HandleMovementMechanics(actor);
        
        // Process nearby actors
        HandleTargettingMechanics(actor);
        
        // Target tracking orientations
        UpdateTargetRotation(actor);
        
        // Cycle the animation states
        UpdateAnimationState(actor);
        
        // Handle death and health effects
        HandleVitality(actor);
        
        if (distance > mActorUpdateDistance) 
            continue;
        
        UpdateActorState(actor);
        
    }
}

float counter=0.0f;

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
        
        actor->physical.mAge++;
        
        HandleCooldownCounters(actor);
        
        UpdateProximityList(actor);
        
        HandleHomeLocation(actor);
        
        unsigned int numberOfGenes = actor->genetics.GetNumberOfGenes();
        for (unsigned int a=0; a < numberOfGenes; a++) {
            
            // Trigger physical expression if the expression age was achieved
            if (actor->physical.mAge == actor->genetics.mGenes[a].expressionAge) 
                actor->RebuildGeneticExpression();
        }
        
    }
    
    // Garbage collection pass
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        
        // Check garbage actors
        if (UpdateGarbageCollection(actor)) 
            continue;
    }
}

