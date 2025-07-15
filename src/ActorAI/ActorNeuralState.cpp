#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorState(Actor* actor) {
    
    // Tick up the actor age
    actor->physical.mAge++;
    
    if (actor->state.mode != ActorState::Mode::Idle) 
        return;
    
    if (actor->behavior.mProximityList.empty()) {
        
        // No actor in the proximity list, select a random point
        if (Random.Range(0, 100) > 80) 
            HandleWalkState(actor);
        
    } else {
        
        // Check proximity list for near by actors to interact with
        for (unsigned int i=0; i < actor->behavior.mProximityList.size(); i++) {
            Actor* targetActor = actor->behavior.mProximityList[i];
            if (!targetActor->isActive) {
                actor->behavior.mProximityList.erase(actor->behavior.mProximityList.begin() + i);
                return;
            }
            
            float distanceToProximityTarget = glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition);
            
            if (HandleAttackState(actor, targetActor, distanceToProximityTarget)) 
                continue;
            
            if (HandleFleeState(actor, targetActor, distanceToProximityTarget)) 
                continue;
            
            if (Random.Range(0, 100) > 80) 
                if (HandleFocusState(actor, targetActor, distanceToProximityTarget)) 
                    continue;
            
            if (Random.Range(0, 100) > 80) 
                if (HandleWalkState(actor)) 
                    continue;
            
        }
    }
    return;
}

bool ActorSystem::HandleWalkState(Actor* actor) {
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee) 
        return false;
    
    actor->state.current = ActorState::State::None;
    actor->state.mode    = ActorState::Mode::MoveRandom;
    
    return true;
}

bool ActorSystem::HandleAttackState(Actor* actor, Actor* target, float distance) {
    if (actor->counters.mAttackCoolDownCounter > 0) 
        return false;
    
    // Actor must be a predator
    if (!actor->behavior.mIsPredator) return false;
    
    // Target must be prey
    if (!target->behavior.mIsPrey) return false;
    
    // Check attack distance
    if (distance > actor->behavior.GetDistanceToAttack()) return false;
    
    // Attack the target
    actor->state.mode    = ActorState::Mode::RunTo;
    actor->state.current = ActorState::State::Attack;
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    
    return true;
}


bool ActorSystem::HandleFleeState(Actor* actor, Actor* target, float distance) {
    // Actor must be prey
    if (!actor->behavior.mIsPrey) return false;
    
    // Target must be a predator
    if (!target->behavior.mIsPredator) return false;
    
    // Check flee distance
    if (distance > actor->behavior.GetDistanceToFlee()) return false;
    
    // Target should flee
    actor->state.mode    = ActorState::Mode::RunTo;
    actor->state.current = ActorState::State::Flee;
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    
    return true;
}


bool ActorSystem::HandleFocusState(Actor* actor, Actor* target, float distance) {
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee) 
        return false;
    
    // Observation cool down
    if (actor->counters.mObservationCoolDownCounter > 0) return false;
    
    // Focus on the selected target
    actor->state.mode    = ActorState::Mode::Idle;
    actor->state.current = ActorState::State::Look;
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    
    actor->counters.mObservationCoolDownCounter = actor->behavior.GetCooldownObserve();
    return true;
}
