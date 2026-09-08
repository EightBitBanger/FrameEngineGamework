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

void ActorSystem::UpdateFast() {
    unsigned int numberOfActors = mActiveActors.size();
    mFrameTimeCurrent = mainTimer.GetCurrentDelta();
    
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActiveActors[i];
        std::lock_guard<std::mutex> lock(actor->mux);
        
        // Skip physical locomotion and body yaw turning while frozen
        if (actor->state.mode != ActorState::Mode::Frozen) {
            HandleMovementMechanics(actor);
            UpdateTargetRotation(actor);
            HandleVitality(actor);
        } else {
            actor->navigation.mVelocity = glm::vec3(0.0f);
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
        }
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        {
            std::lock_guard<std::mutex> lock(Renderer.mux);
            
            // Keep animations and gene rendering active so edits appear immediately
            UpdateAnimationState(actor);
            
            float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
            if (distance > mActorUpdateDistance / 2.0f) 
                continue;
            
            UpdateActorGenetics(actor);
            ExpressActorGenetics(actor);
            UpdateActorInventory(actor);
        }
        
        // Skip physical locomotion and body yaw turning while frozen
        if (actor->state.mode != ActorState::Mode::Frozen) {
            HandleMovementMechanics(actor);
            UpdateTargetRotation(actor);
            HandleVitality(actor);
        }
    }
}

void ActorSystem::UpdateTick(void) {
    unsigned int numberOfActors = mActors.Size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        std::lock_guard<std::mutex> lock(actor->mux);
        
        if (actor->isGarbage || !actor->isActive) 
            continue;
        
        // Cull updates for far away actors
        float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
        if (distance > mActorUpdateDistance) 
            continue;
        
        // Update tick divider
        
        actor->mUpdateCounter++;
        
        if ((actor->mUpdateCounter % 5) == 0) {
            actor->physical.mAge++;
            
            // Trigger physical expression if the expression age was achieved
            unsigned int numberOfGenes = actor->genetics.GetNumberOfGenes();
            for (unsigned int a = 0; a < numberOfGenes; a++) {
                if (actor->physical.mAge == actor->genetics.mGenes[a].expressionAge) 
                    actor->RebuildGeneticExpression();
            }
        }
        
        if ((actor->mUpdateCounter % 20) == 0) {
            HandleCooldownCounters(actor);
            
            // Target observation
            if (actor->state.mode != ActorState::Mode::Frozen) {
                UpdateGazeTarget(actor);
            }
        }
        
        if (actor->mUpdateCounter > 40) {
            
            actor->RebuildGeneticExpression();
            actor->mUpdateCounter = 0;
        }
        
    }
    
    // Garbage collection pass
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActors[i];
        UpdateGarbageCollection(actor);
    }
}

void ActorSystem::UpdateThinking(void) {
    unsigned int numberOfActors = mActiveActors.size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* actor = mActiveActors[i];
        std::lock_guard<std::mutex> lock(actor->mux);
        
        if (actor->isGarbage || !actor->isActive || actor->state.mode == ActorState::Mode::Frozen)
            continue;
        
        actor->mThoughtCounter++;
        if (actor->mThoughtCounter > 40) {
            actor->mThoughtCounter = 0;
            
            // Get actor sentience score and emotional state
            const std::vector<MemoryTrigger>& sentienceList = actor->memories.mMemoryTriggers["sentience"];
            float sentientScore = !sentienceList.empty() ? sentienceList[0].value : 0.0f;
            EmotionalEmbedding& emotion = actor->emotions.current;
            
            // Compile list of nearby actors
            UpdateTargetingMechanics(actor);
            
            // Run neural state update
            UpdateActorState(actor, emotion, sentientScore);
            
            // Evaluate Thought Matrix across targets
            UpdateThoughtMatrix(actor, emotion, sentientScore);
            
            // Process behavioral idiosyncrasies
            ProcessMemoryTriggers(actor, emotion);
            
            // Check update memories
            actor->memories.UpdateMemories();
        }
    }
}
