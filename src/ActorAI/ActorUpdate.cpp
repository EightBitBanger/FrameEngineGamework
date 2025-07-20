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

extern int actorCounter;
extern int tickCounter;


void ActorSystem::Update(void) {
    std::lock_guard<std::mutex> (Engine.sceneMain->mux);
    
    // Update animation states (Runs hot!)
    if (mAnimationTimer.Update()) {
        unsigned int numberOfActors = mActiveActors.size();
        for (unsigned int i = 0; i < numberOfActors; i++) {
            Actor* actor = mActiveActors[i];
            
            if (actor->isGarbage || !actor->isActive) 
                continue;
            
            // Cycle the animation states
            UpdateAnimationState(actor);
            
            // Target tracking orientations
            UpdateTargetRotation(actor);
            
            // Update actor mechanical / locomotion state
            HandleMovementMechanics(actor);
            HandleTargettingMechanics(actor);
            
            HandleVitality(actor);
            
        }
    }
    
    // Update AI states
    if (mMainTimer.Update()) {
        unsigned int numberOfActors = mActors.Size();
        if (numberOfActors == 0) 
            return;
        
        for (unsigned int i = 0; i < numberOfActors; i++) {
            if (actorCounter >= numberOfActors) {
                actorCounter = 0;
                doUpdate = false;
                break;
            }
            
            Actor* actor = mActors[actorCounter];
            actorCounter++;
            
            // Check garbage actors
            if (UpdateGarbageCollection(actor)) 
                continue;
            
            if (actor->isGarbage || !actor->isActive) 
                continue;
            
            // Cull updates for far away actors
            float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
            if (distance > mActorUpdateDistance) 
                continue;
            
            UpdateProximityList(actor);
            UpdateActorState(actor);
            UpdateActorGenetics(actor);
            
            ExpressActorGenetics(actor);
            
            HandleCooldownCounters(actor);
        }
        
    }
    
    return;
}

