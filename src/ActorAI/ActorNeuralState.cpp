#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorState(Actor* actor) {
    
    actor->physical.mAge++;
    
    // Chance to select a random action
    if (actor->state.current == NeuralState::idle) {
        
        //if (Random.Range(0.0f, 100.0f) < actor->behavior.mChanceToWalk)    actor->state.current = NeuralState::walk;
        //if (Random.Range(0.0f, 100.0f) < actor->behavior.mChanceToFocus)   actor->state.current = NeuralState::focus;
        
        //if (Random.Range(0.0f, 100.0f) < actor->behavior.mChanceToAttack)  actor->state.current = NeuralState::attack;
        //if (Random.Range(0.0f, 100.0f) < actor->behavior.mChanceToFlee)    actor->state.current = NeuralState::flee;
        
        
        // TEST
        if (mActors.Size() > 0) {
            
            unsigned int index = Random.Range(0, mActors.Size()-1);
            Actor* targetActor = mActors[ index ];
            
            if (actor->navigation.mTargetActor == targetActor) 
                return;
            
            actor->state.current = NeuralState::flee;
            actor->navigation.mTargetActor = targetActor;
            
            targetActor->navigation.mTargetActor = actor;
            targetActor->state.current = NeuralState::attack;
            
        }
        
    }
    
    
    
    switch (actor->state.current) {
        
        case NeuralState::idle:     HandleIdleState(actor); break;
        case NeuralState::walk:     HandleWalkState(actor); break;
        case NeuralState::focus:    HandleFocusState(actor); break;
        case NeuralState::attack:   HandleAttackState(actor); break;
        case NeuralState::flee:     HandleFleeState(actor); break;
        
    }
    
    return;
}

void ActorSystem::SwitchActorState(Actor* actor, NeuralState state) {
    actor->state.current = state;
    return;
}


void ActorSystem::HandleIdleState(Actor* actor) {
    
    return;
}

void ActorSystem::HandleWalkState(Actor* actor) {
    
    //SwitchActorState(actor, NeuralState::idle);
    
    
    /*
    if (actor->navigation.mTargetPoint == actor->navigation.mPosition) 
        HandleTargetRandomPoint(actor);
    
    if (actor->state.mIsWalking) 
        return;
    
    // Target height will be updated by the physics system.
    // When this occurs, the height will be changed, triggering the
    // actor to begin walking
    if (actor->navigation.mTargetPoint.y != -100000.0f) {
        
        // Check outside prefered area to find the correct height range
        if (actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax || 
            actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin) {
            
            // Increment distance expansion
            if (actor->counters.mMovementCoolDownCounter < 100) 
                actor->counters.mMovementCoolDownCounter++;
            actor->counters.mObservationCoolDownCounter++;
            
            if (actor->counters.mObservationCoolDownCounter == 10) {
                actor->counters.mObservationCoolDownCounter = 0;
                HandleTargetRandomPoint(actor);
            }
        }
        
        // Check within prefered range
        HandleMovementMechanics(actor);
    }
    */
    return;
}

void ActorSystem::HandleFocusState(Actor* actor) {
    if (actor->counters.mObservationCoolDownCounter > 0) {
        SwitchActorState(actor, NeuralState::idle);
        return;
    }
    
    // Check focal range
    if (actor->navigation.mDistance < actor->behavior.mDistanceToFocus) {
        // Focus on player
        actor->navigation.mTargetPoint.x = mPlayerPosition.x;
        actor->navigation.mTargetPoint.z = mPlayerPosition.z;
        actor->counters.mObservationCoolDownCounter = 10;
    } else {
        // Focus on nearby actor
        //if (actor->navigation.mTargetActor != nullptr) {
        //    actor->navigation.mTargetPoint.x = actor->navigation.mTargetActor->navigation.mPosition.x;
        //    actor->navigation.mTargetPoint.z = actor->navigation.mTargetActor->navigation.mPosition.z;
        //}
    }
    
    SwitchActorState(actor, NeuralState::idle);
    return;
}

void ActorSystem::HandleAttackState(Actor* actor) {
    
    return;
}

void ActorSystem::HandleFleeState(Actor* actor) {
    //if (actor->navigation.mTargetActor == nullptr) 
    //    return;
    
    //SwitchActorState(actor, NeuralState::walk);
    
    return;
}

void ActorSystem::HandleDefendState(Actor* actor) {
    
    return;
}

