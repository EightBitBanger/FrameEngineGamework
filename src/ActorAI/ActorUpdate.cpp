#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Profiler/profiler.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern EngineSystemManager Engine;
extern MathCore Math;
extern ProfilerTimer Profiler;

extern bool isActorThreadActive;
extern bool doUpdate;

extern int tickCounter;


void ActorSystem::Update(void) {
    if (mAnimationTimer.Update()) 
        UpdateFast();
    
    if (mMainTimer.Update()) 
        UpdateTick();
    
    Profiler.profileActorAI = mAnimationTimer.GetCurrentDelta();
}


void ActorSystem::UpdateFast() {
    std::lock_guard<std::mutex> lock(Renderer.mux);
    
    unsigned int numberOfActors = mActiveActors.size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActiveActors[i];
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        // Cycle the animation states
        UpdateAnimationState(actor);
        
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        if (distance > mActorUpdateDistance / 2.0f) 
            continue;
        
        // Genetic expression update
        UpdateActorGenetics(actor);
        ExpressActorGenetics(actor);
        
        // Update actor mechanical / locomotion
        HandleMovementMechanics(actor);
        
        // Target tracking orientations
        UpdateTargetRotation(actor);
        
        // Handle death and health effects
        HandleVitality(actor);
    }
}

float counter=0.0f;

void ActorSystem::UpdateTick(void) {
    unsigned int numberOfActors = mActors.Size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        // Check update memories
        if (actor->memories.mDoUpdateMemories) {
            actor->memories.mDoUpdateMemories = false;
            
            actor->memories.UpdateMemories();
        }
        
        // Cull updates for far away actors
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        if (distance > mActorUpdateDistance) 
            continue;
        
        // Update tick divider
        actor->mUpdateCounter++;
        if (actor->mUpdateCounter > 20) {
            actor->mUpdateCounter=0;
            
            HandleCooldownCounters(actor);
            
            actor->physical.mAge++;
            
            // Run neural state update
            UpdateActorState(actor);
            
            // Keep actor nearby its associated "home" location
            if (actor->state.mode != ActorState::Mode::MoveAttack && 
                actor->state.mode != ActorState::Mode::MoveFlee) 
                HandleHomeLocation(actor);
            
            // Compile list of nearby actors
            HandleTargettingMechanics(actor);
            
            unsigned int numberOfGenes = actor->genetics.GetNumberOfGenes();
            for (unsigned int a=0; a < numberOfGenes; a++) {
                
                // Trigger physical expression if the expression age was achieved
                if (actor->physical.mAge == actor->genetics.mGenes[a].expressionAge) 
                    actor->RebuildGeneticExpression();
            }
            
        }
        
    }
    
    // Garbage collection pass
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        
        UpdateGarbageCollection(actor);
    }
    
    // Flush and clear debug renderer for the next frame
    if (mDebugLineRenderer) {
        std::lock_guard<std::mutex> lock(Renderer.mux);
        
        //mDebugLineRenderer->mesh->ClearSubMeshes();
        //mDebugLineRenderer->mesh->Load();
    }
}

