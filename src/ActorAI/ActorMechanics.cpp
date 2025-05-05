#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


#define DECISION_CHANCE_TO_WALK                  1000
#define DECISION_CHANCE_TO_CHANGE_DIRECTION      1000
#define DECISION_CHANCE_TO_FOCUS_NEARBY          10000
#define DECISION_CHANCE_TO_STOP_MOVING           10000
#define COOLDOWN_OBSERVATION_MAX                 100
#define COOLDOWN_MOVEMENT_MAX                    100

#define DISTANCE_MINIMUM_TARGET_REACHED          1.0f
#define DISTANCE_MINIMUM_TARGET_BREEDING         1.0f


void ActorSystem::UpdateActorMechanics(Actor* actor) {
    
    HandleWalkingMechanics(actor);
    HandleTargettingMechanics(actor);
    HandleBreedingMechanics(actor);
    
    return;
}

void ActorSystem::HandleWalkingMechanics(Actor* actor) {
    
    switch (actor->state.mode) {
        
        case ActorState::Mode::Idle:
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            // Cancel movement ignoring falling
            actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
            break;
            
        case ActorState::Mode::RunTo:
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            
        case ActorState::Mode::WalkTo:
            actor->state.mIsWalking = true;
            
        case ActorState::Mode::MoveTo:
            glm::vec3 forward(0);
            forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
            forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90) ) );
            // TODO: Should actors fly???
            //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) );
            
            forward *= actor->physical.mSpeed * 0.01f;
            
            // Check running speed
            if (actor->state.mIsRunning) 
                forward *= actor->physical.mSpeedMul;
            
            // Apply directional velocity
            actor->navigation.mVelocity.x = forward.x;
            actor->navigation.mVelocity.z = forward.z;
            break;
            
    }
    
    return;
}


void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    glm::vec3 position(0);
    if (actor->navigation.mTargetActor != nullptr) 
        position = actor->navigation.mTargetActor->navigation.mPosition;
    
    switch (actor->state.current) {
            
        case ActorState::State::Attack:
            actor->navigation.mTargetPoint = position;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            break;
            
        case ActorState::State::Flee:
            actor->navigation.mTargetPoint = position;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = false;
            break;
            
        case ActorState::State::Focus:
            actor->navigation.mTargetPoint = position;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            break;
            
        case ActorState::State::Look:
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            break;
            
    }
    
    // Check destination was reached
    //if (glm::distance(actor->navigation.mTargetPoint, actor->navigation.mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) 
    //    actor->state.mode = ActorState::Mode::Idle;
    
    
    
    // Check focal range
    /*
    if (actor->state.attack == ActorState::State::Focusing) {
    if (glm::distance(actor->navigation.mPosition, mPlayerPosition) < actor->behavior.mDistanceToFocus) {
        // Focus on player
        actor->navigation.mTargetPoint.x = mPlayerPosition.x;
        actor->navigation.mTargetPoint.z = mPlayerPosition.z;
        actor->counters.mObservationCoolDownCounter = 10;
    }
    */
    
    
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

/*

void ActorSystem::HandleTargetRandomPoint(Actor* actor) {
    
    if (actor->counters.mObservationCoolDownCounter > 0) 
        actor->counters.mObservationCoolDownCounter--;
    
    
    float randA = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    float randB = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    randA -= Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    randB -= Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    
    actor->navigation.mTargetPoint.x = randA + actor->navigation.mPosition.x;
    // The physics system will update this position to the actors current height
    actor->navigation.mTargetPoint.y = -100000.0f;
    actor->navigation.mTargetPoint.z = randB + actor->navigation.mPosition.z;
    
    return;
}
*/

void ActorSystem::HandleMovementMechanics(Actor* actor) {
    
    
    
    /*
    // Height preference max below water suggests
    // the actor is an aquatic actor
    bool isAquatic = actor->behavior.mHeightPreferenceMax < mWorldWaterLevel;
    
    if (actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax && 
        actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin) 
        return;
    
    if (!isAquatic) {
        
        // Terrestrial
        if (actor->navigation.mTargetPoint.y > mWorldWaterLevel) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
        // Check under water, navigate to higher ground
        if (actor->navigation.mPosition.y < mWorldWaterLevel && 
            actor->navigation.mTargetPoint.y > actor->navigation.mPosition.y) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
    } else {
        
        // Aquatic
        if (actor->navigation.mTargetPoint.y < mWorldWaterLevel) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
        // Check above water, navigate to lower ground
        if (actor->navigation.mPosition.y > mWorldWaterLevel && actor->navigation.mTargetPoint.y < actor->navigation.mPosition.y) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
    }
    */
    return;
}

