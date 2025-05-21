#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::UpdateActorMechanics(Actor* actor) {
    
    HandleMovementMechanics(actor);
    HandleTargettingMechanics(actor);
    HandleBreedingMechanics(actor);
    
    // Check actor vital state
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

void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    glm::vec3 position(0);
    if (actor->navigation.mTargetActor != nullptr) 
        position = actor->navigation.mTargetActor->navigation.mPosition;
    
    switch (actor->state.current) {
            
        case ActorState::State::Attack:
            if (actor->counters.mAttackCoolDownCounter > 0) 
                break;
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            
            // Inflict attack damage
            HandleInflictDamage(actor, actor->navigation.mTargetActor);
            break;
            
        case ActorState::State::Flee:
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = false;
            break;
            
        case ActorState::State::Focus:
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            break;
            
        case ActorState::State::Look:
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            break;
            
    }
    
    glm::vec3 posA = actor->navigation.mPosition;
    glm::vec3 posB = actor->navigation.mTargetPoint;
    posA.y = 0.0f;
    posB.y = 0.0f;
    
    // Check arrived at target actor
    actor->navigation.mDistanceToTarget = glm::distance(posA, posB);
    if (actor->navigation.mDistanceToTarget < actor->behavior.GetDistanceToInflict()) {
        
        actor->state.mode = ActorState::Mode::Idle;
    }
    
    // Check dead target
    if (actor->navigation.mTargetActor != nullptr) {
        
        if (!actor->navigation.mTargetActor->isActive) 
            actor->navigation.mTargetActor = nullptr;
        
    }
    return;
}

void ActorSystem::HandleInflictDamage(Actor* actor, Actor* target) {
    if (glm::distance(actor->navigation.mPosition, target->navigation.mPosition) > actor->behavior.mDistanceToInflict) 
        return;
    
    actor->counters.mAttackCoolDownCounter = actor->behavior.GetCooldownAttack();
    actor->state.mode = ActorState::Mode::Idle;
    
    // Check if the target defense is greater them my strength
    if (target->biological.defense >= actor->biological.strength) 
        return;
    
    target->biological.health -= actor->biological.strength - target->biological.defense;
    return;
}

void ActorSystem::HandleVitality(Actor* actor) {
    
    // Check death
    if (actor->biological.health <= 0.0f) {
        actor->isActive = false;
        actor->isGarbage = true;
    }
    
    return;
}

void ActorSystem::HandleCooldownCounters(Actor* actor) {
    if (actor->counters.mMovementCoolDownCounter > 0) 
        actor->counters.mMovementCoolDownCounter--;
    
    if (actor->counters.mAttackCoolDownCounter > 0) 
        actor->counters.mAttackCoolDownCounter--;
    
    if (actor->counters.mObservationCoolDownCounter > 0) 
        actor->counters.mObservationCoolDownCounter--;
    
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


void ActorSystem::UpdateProximityList(Actor* actor) {
    
    // Check if any actor should be dropped from the list
    for (int i = actor->behavior.mProximityList.size() - 1; i >= 0; --i) {
        Actor* actorPtr = actor->behavior.mProximityList[i];
        
        // Check actor focal range
        if (glm::distance(actor->navigation.mPosition, actorPtr->navigation.mPosition) > actor->behavior.mDistanceToFocus * 2.0f) {
            actor->behavior.mProximityList.erase(actor->behavior.mProximityList.begin() + i);
            continue;
        }
        
        // Check actor has been disabled or destroyed
        if (actorPtr->isGarbage) {
            actor->behavior.mProximityList.erase(actor->behavior.mProximityList.begin() + i);
            continue;
        }
    }
    
    // Limit list size
    if (actor->behavior.mProximityList.size() > 8) 
        return;
    
    unsigned int index = Random.Range(0, mActors.Size()-1);
    if (index >= mActors.Size()) 
        return;
    
    // Select a random actor
    Actor* targetActor = mActors[ index ];
    
    // Check bad actor
    if (targetActor->isGarbage) 
        return;
    
    // Actor cannot target itself
    if (targetActor == actor) 
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
