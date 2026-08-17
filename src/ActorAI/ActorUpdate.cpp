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
        
        // Inventory selection
        UpdateActorInventory(actor);
        
        // Update actor mechanical / locomotion
        HandleMovementMechanics(actor);
        
        // Target tracking orientations
        UpdateTargetRotation(actor);
        
        // Handle death and health effects
        HandleVitality(actor);
    }
}

void ActorSystem::UpdateTick(void) {
    unsigned int numberOfActors = mActors.Size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        // Cull updates for far away actors
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        if (distance > mActorUpdateDistance) 
            continue;
        
        // Update tick divider
        
        actor->mUpdateCounter++;
        
        if ((actor->mUpdateCounter % 5) == 0) 
            actor->physical.mAge++;
        
        // Get actor sentience score and emotional state
        const std::vector<MemoryTrigger>& sentienceList = actor->memories.mMemoryTriggers["sentience"];
        float sentientScore = !sentienceList.empty() ? sentienceList[0].value : 0.0f;
        EmotionalEmbedding& emotion = actor->emotions.current;
        
        if ((actor->mUpdateCounter % 20) == 0) {
            HandleCooldownCounters(actor);
            
            // Run neural state update
            UpdateActorState(actor, emotion, sentientScore);
            
            // Target observation
            UpdateGazeTarget(actor);
            
            // Compile list of nearby actors
            UpdateTargetingMechanics(actor);
            
            // Evaluate Thought Matrix across targets
            UpdateThoughtMatrix(actor, emotion, sentientScore);
        }
        
        if (actor->mUpdateCounter > 40) {
            actor->mUpdateCounter = 0;
            
            // Process behavioral idiosyncrasies
            ProcessMemoryTriggers(actor, emotion);
            
            // Check update memories
            actor->memories.UpdateMemories();
            
            // Trigger physical expression if the expression age was achieved
            unsigned int numberOfGenes = actor->genetics.GetNumberOfGenes();
            for (unsigned int a = 0; a < numberOfGenes; a++) {
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
    }
}

