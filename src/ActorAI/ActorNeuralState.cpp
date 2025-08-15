#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorState(Actor* actor) {
    actor->physical.mAge++;
    if (actor->state.mode != ActorState::Mode::Idle) 
        return;
    
    if (Random.Range(0, 100) > 80) 
        if (HandleWalkState(actor)) 
            return;
    
    if (actor->navigation.mTargetActor == nullptr) 
        return;
    float distanceToProximityTarget = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition);
    actor->navigation.mDistanceToTarget = distanceToProximityTarget;
    
    if (HandleAttackState(actor, actor->navigation.mTargetActor, distanceToProximityTarget)) 
        return;
    if (HandleFleeState(actor, actor->navigation.mTargetActor, distanceToProximityTarget)) 
        return;
    
    if (Random.Range(0, 100) > 80) 
        if (HandleFocusState(actor, actor->navigation.mTargetActor, distanceToProximityTarget)) 
            return;
    
    if (Random.Range(0, 100) > 80) 
        if (HandleBreedingState(actor, actor->navigation.mTargetActor, distanceToProximityTarget)) 
            return;
    
    return;
}

bool ActorSystem::HandleWalkState(Actor* actor) {
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee || 
        actor->state.mode != ActorState::Mode::Idle) 
        return false;
    
    actor->state.current = ActorState::State::None;
    actor->state.mode    = ActorState::Mode::MoveRandom;
    
    return true;
}

bool ActorSystem::HandleAttackState(Actor* actor, Actor* target, float distance) {
    if (actor->counters.mAttackCoolDownCounter > 0) 
        return false;
    
    // Actor must be a predator
    if (!actor->behavior.mIsPredator) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Target must be prey
    if (!target->behavior.mIsPrey) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Check attack distance
    if (distance > actor->behavior.GetDistanceToAttack()) {
        actor->state.mode    = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Attack the target
    actor->state.mode    = ActorState::Mode::RunTo;
    actor->state.current = ActorState::State::Attack;
    
    target->state.mode    = ActorState::Mode::RunTo;
    target->state.current = ActorState::State::Attack;
    
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    return true;
}


bool ActorSystem::HandleFleeState(Actor* actor, Actor* target, float distance) {
    // Actor must be prey
    if (!actor->behavior.mIsPrey) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Target must be a predator
    if (!target->behavior.mIsPredator) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Check flee distance
    if (distance > actor->behavior.GetDistanceToFlee()) {
        actor->state.mode    = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        return false;
    }
    
    // Target should flee
    actor->state.mode    = ActorState::Mode::RunTo;
    actor->state.current = ActorState::State::Flee;
    return true;
}


bool ActorSystem::HandleFocusState(Actor* actor, Actor* target, float distance) {
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


bool ActorSystem::HandleBreedingState(Actor* actor, Actor* target, float distance) {
    if (!actor->isActive || !target->isActive) 
        return false;
    
    if (mActors.Size() > 500) 
        return false;
    
    if (actor->counters.mBreedingCoolDownCounter > 0 || target->counters.mBreedingCoolDownCounter > 0) 
        return false;
    
    if (actor->physical.mAge < actor->physical.mAgeAdult) return false;
    
    if (actor->state.current == ActorState::State::Breed) 
        return false;
    
    if (actor->navigation.mPosition.y > actor->behavior.mHeightPreferenceMax || 
        actor->navigation.mPosition.y < actor->behavior.mHeightPreferenceMin) 
        return false;
    
    if (distance > actor->behavior.GetDistanceToAttack()) {
        actor->state.current = ActorState::State::None;
        return false;
    }
    if (actor->physical.GetSexualOrientation() == target->physical.GetSexualOrientation()) 
        return false;
    
    // Focus on the selected target
    actor->state.mode    = ActorState::Mode::WalkTo;
    actor->state.current = ActorState::State::Breed;
    
    target->state.mode    = ActorState::Mode::WalkTo;
    target->state.current = ActorState::State::Breed;
    
    actor->navigation.mTargetActor = target;
    target->navigation.mTargetActor = actor;
    return true;
}


void ActorSystem::UpdateProximityList(Actor* actor) {
    // Is the actor engaging with its target
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee || 
        actor->state.current == ActorState::State::Observe) 
        return;
    
    // Select a random actor
    unsigned int index = Random.Range((unsigned int)0, mActors.Size()-1);
    if (index >= mActors.Size()) 
        return;
    Actor* targetActor = mActors[index];
    
    if (!targetActor->isActive || targetActor->isGarbage) 
        return;
    
    // Actor cannot target itself
    if (targetActor == actor) 
        return;
    
    // Can the actor see the target
    if (glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition) > actor->behavior.mDistanceToFocus) 
        return;
    
    // Target the actor
    actor->navigation.mTargetActor = targetActor;
}


void ActorSystem::UpdateActorMemories(Actor* actor) {
    unsigned int numberOfMemores = actor->memories.GetNumberOfMemories();
    
    for (unsigned int i=0; i < numberOfMemores; i++) {
        std::string name = actor->memories.GetMemoryNameByIndex(i);
        
        // Stay nearby a home position in the world
        if (name == "home") {
            std::string value = actor->memories.GetMemoryValueByIndex(i);
            
            std::vector<std::string> split = String.Explode(value, ',');
            if (split.size() == 3) {
                glm::vec3 homePosition;
                homePosition.x = String.ToFloat(split[0]);
                homePosition.y = String.ToFloat(split[1]);
                homePosition.z = String.ToFloat(split[2]);
                
                if (glm::distance(actor->navigation.mPosition, homePosition) > actor->behavior.mDistanceToWalk) {
                    actor->navigation.mTargetPoint = homePosition;
                    
                    actor->state.current = ActorState::State::None;
                    actor->state.mode = ActorState::Mode::MoveTo;
                }
                
                continue;
            }
        }
        
    }
    
}

