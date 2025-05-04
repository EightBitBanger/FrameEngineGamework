#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorState(Actor* actor) {
    // Tick up the actor age
    actor->physical.mAge++;
    
    UpdateProximityList(actor);
    
    if (actor->state.mode == ActorState::Mode::Idle) {
        
        HandleIdleState(actor);
        return;
    }
    
    return;
}

void ActorSystem::UpdateProximityList(Actor* actor) {
    // Select a random actor
    Actor* targetActor = mActors[ Random.Range(0, mActors.Size()-1) ];
    
    // Actor cannot target itself
    if (actor == targetActor) 
        return;
    
    // Check actor focal range
    if (glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition) > actor->behavior.mDistanceToFocus) 
        return;
    
    // Check if the actor is already on the list
    for (unsigned int i=0; i < actor->behavior.mProximityList.size(); i++) {
        if (actor->behavior.mProximityList[i] == targetActor) 
            return;
    }
    
    // Add the target to the list
    actor->behavior.mProximityList.push_back(targetActor);
    
    return;
}

void ActorSystem::HandleIdleState(Actor* actor) {
    
    HandleWalkState(actor);
    
    // Get a random target actor
    for (unsigned int i=0; i < actor->behavior.mProximityList.size(); i++) {
        Actor* targetActor = actor->behavior.mProximityList[i];
        
        float distanceToTarget = glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition);
        
        HandleAttackState(actor, targetActor, distanceToTarget);
        HandleFleeState(actor, targetActor, distanceToTarget);
        
        HandleFocusState(actor, targetActor, distanceToTarget);
    }
    
    
    return;
}

void ActorSystem::HandleWalkState(Actor* actor) {
    
    return;
}

void ActorSystem::HandleAttackState(Actor* actor, Actor* target, float distance) {
    // Actor must be a predator
    if (!actor->behavior.mIsPredator) 
        return;
    
    // Target must be prey
    if (!target->behavior.mIsPrey) 
        return;
    
    // Check attack distance
    if (distance > actor->behavior.GetDistanceToAttack()) 
        return;
    
    // Attack the target
    actor->state.mode    = ActorState::Mode::MoveTo;
    actor->state.current = ActorState::State::Attack;
    actor->navigation.mTargetActor = target;
    
    return;
}


void ActorSystem::HandleFleeState(Actor* actor, Actor* target, float distance) {
    // Actor must be prey
    if (!actor->behavior.mIsPrey) 
        return;
    
    // Target must be a predator
    if (!target->behavior.mIsPredator) 
        return;
    
    // Check flee distance
    if (distance > actor->behavior.GetDistanceToFlee()) 
        return;
    
    // Target should flee
    actor->state.mode    = ActorState::Mode::MoveTo;
    actor->state.current = ActorState::State::Flee;
    actor->navigation.mTargetActor = target;
    
    return;
}


void ActorSystem::HandleFocusState(Actor* actor, Actor* target, float distance) {
    return;
    if (actor->state.current == ActorState::State::Attack || actor->state.current == ActorState::State::Flee) 
        return;
    
    if (distance > actor->behavior.GetDistanceToFocus()) 
        return;
    
    // Focus on the selected target
    actor->state.mode    = ActorState::Mode::Idle;
    actor->state.current = ActorState::State::Look;
    actor->navigation.mTargetActor = target;
    
    return;
}
