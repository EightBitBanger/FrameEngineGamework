#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

extern bool isActorThreadActive;
extern bool doUpdate;

extern int actorCounter;
extern int tickCounter;


void ActorSystem::Update(void) {
    
    // Update animation states (Runs hot!)
    if (mAnimationTimer.Update()) {
        unsigned int numberOfActors = mActiveActors.size();
        for (unsigned int i = 0; i < numberOfActors; i++) {
            Actor* actor = mActiveActors[i];
            
            // Cycle the animation states
            UpdateAnimationState(actor);
            
            // Target tracking orientations
            UpdateTargetRotation(actor);
            
            // Update mechanical / locomotion state
            UpdateActorMechanics(actor);
            
        }
    }
    
    // Update AI states
    if (mMainTimer.Update()) {
        unsigned int numberOfActors = mActiveActors.size();
        if (numberOfActors == 0) 
            return;
        
        unsigned int numberOfActorsPerCycle = (numberOfActors > 10) ? (numberOfActors / 10) : 1;
        
        for (unsigned int i = 0; i < numberOfActorsPerCycle; i++) {
            
            if (actorCounter >= numberOfActors) {
                actorCounter = 0;
                doUpdate = false;
                break;
            }
            
            Actor* actor = mActiveActors[actorCounter++];
            if (!actor->state.mIsActive) 
                continue;
            
            // Cull updates for far away actors
            float distance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
            if (distance > mActorUpdateDistance) 
                continue;
            
            // Update states
            UpdateActorState(actor);
            
            // Update genetics if requested
            UpdateActorGenetics(actor);
            
            // Express genetic phenotypes
            ExpressActorGenetics(actor);
            
        }
        
    }
    
    return;
}

