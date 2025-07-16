#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::UpdateActorMechanics(Actor* actor) {
    
    HandleMovementMechanics(actor);
    HandleTargettingMechanics(actor);
    HandleBreedingMechanics(actor);
    
    HandleVitality(actor);
    
    return;
}

void ActorSystem::HandleMovementMechanics(Actor* actor) {
    glm::vec3 forward(0);
    glm::vec3 position(0);
    
    switch (actor->state.mode) {
        
        case ActorState::Mode::Sleeping:
        case ActorState::Mode::Idle:
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            // Cancel movement & ignore falling
            actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
            break;
            
        case ActorState::Mode::MoveRandom:
            // Calculate random near by point
            position = CalculateRandomLocalPoint(actor);
            
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            // Shift mode to move to point
            actor->state.mode = ActorState::Mode::MoveTo;
            
        case ActorState::Mode::MoveTo:
            forward = CalculateForwardVelocity(actor);
            forward *= actor->physical.mSpeed * 0.01f;
            
            actor->state.mIsWalking = true;
            actor->state.mode = ActorState::Mode::WalkTo;
            break;
            
        case ActorState::Mode::WalkTo:
            forward = CalculateForwardVelocity(actor);
            forward *= actor->physical.mSpeed * 0.01f;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            break;
            
        case ActorState::Mode::RunTo:
            forward = CalculateForwardVelocity(actor);
            forward *= actor->physical.mSpeed * 0.01f;
            forward *= actor->physical.mSpeedMul;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            break;
            
    }
    
    actor->navigation.mVelocity = forward;
    return;
}


void ActorSystem::HandleCooldownCounters(Actor* actor) {
    if (actor->counters.mMovementCoolDownCounter > 0) 
        actor->counters.mMovementCoolDownCounter--;
    
    if (actor->counters.mAttackCoolDownCounter > 0) 
        actor->counters.mAttackCoolDownCounter--;
    
    if (actor->counters.mObservationCoolDownCounter > 0) 
        actor->counters.mObservationCoolDownCounter--;
    
    // Reset actors that have lost the target
    if (actor->counters.mAttackCoolDownCounter == 0 && 
        actor->state.current == ActorState::State::Attack && 
        actor->state.mode == ActorState::Mode::Idle) {
        
        actor->state.current = ActorState::State::None;
        actor->state.mode = ActorState::Mode::Idle;
    }
        
    
    return;
}


void ActorSystem::HandleBreedingMechanics(Actor* actor) {
    /*
    // Count down breeding counter
    if (actor->counters.mBreedingCoolDownCounter > 0) 
        actor->counters.mBreedingCoolDownCounter--;
    
    if (actor->navigation.mTargetBreeding == nullptr) 
        return;
    
    // Check breeding is withing the prefered range
    if ((actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax) | 
        (actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin)) 
        return;
    
    actor->navigation.mTargetPoint = actor->navigation.mTargetBreeding->navigation.mPosition;
    // The physics system will update this position, until then we
    // ignore it. When it updates we will know the current height of the actor
    actor->navigation.mTargetPoint.y = -100000.0f;
    
    // Walk the male towards the female target actor
    if (actor->physical.GetSexualOrientation() == true) 
        actor->state.mIsWalking = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetBreeding->navigation.mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING;
    
    // Check arrived at breeding partner
    if (actor->navigation.mTargetBreeding != nullptr) {
        
        if (glm::distance( actor->navigation.mTargetBreeding->navigation.mPosition, actor->navigation.mPosition ) < 1.0f) {
            actor->state.mIsWalking = false;
            actor->navigation.mTargetPoint = actor->navigation.mPosition;
            
            // Null the target if we arrived
            if (actor->navigation.mTargetBreeding->navigation.mTargetBreeding == actor->navigation.mTargetBreeding) 
                actor->navigation.mTargetBreeding->navigation.mTargetBreeding = nullptr;
            
            actor->navigation.mTargetBreeding = nullptr;
        }
    }
    */
    return;
}



glm::vec3 ActorSystem::CalculateForwardVelocity(Actor* actor) {
    glm::vec3 forward(0);
    forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    // TODO: Should actors fly???
    //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) );
    return forward;
}


glm::vec3 ActorSystem::CalculateRandomLocalPoint(Actor* actor) {
    glm::vec3 position(0);
    position.x = actor->navigation.mPosition.x;
    position.z = actor->navigation.mPosition.z;
    
    position.x += Random.Range(0.0f, actor->behavior.GetDistanceToWalk()) - Random.Range(0.0f, actor->behavior.GetDistanceToWalk());
    position.z += Random.Range(0.0f, actor->behavior.GetDistanceToWalk()) - Random.Range(0.0f, actor->behavior.GetDistanceToWalk());
    
    actor->navigation.mTargetPoint.x = position.x;
    actor->navigation.mTargetPoint.y = 0.0f;
    actor->navigation.mTargetPoint.z = position.z;
    return position;
}

