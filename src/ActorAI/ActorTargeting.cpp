#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    glm::vec3 position(0.0f);
    
    if (actor->navigation.mTargetActor != nullptr) 
        position = actor->navigation.mTargetActor->navigation.mPosition;
    
    switch (actor->state.current) {
            
        case ActorState::State::None: 
            break;
            
        case ActorState::State::Defend: 
            break;
            
        case ActorState::State::Attack:
            if (actor->counters.mAttackCoolDownCounter > 0) 
                break;
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            
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
    
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee) 
        return;
    
    // Check arrived at target actor
    glm::vec3 posA = actor->navigation.mPosition;
    glm::vec3 posB = actor->navigation.mTargetPoint;
    posA.y = 0.0f;
    posB.y = 0.0f;
    
    actor->navigation.mDistanceToTarget = glm::distance(posA, posB);
    if (actor->navigation.mDistanceToTarget < actor->behavior.GetDistanceToInflict()) {
        
        // Hold at idle until we repeat the attack
        actor->state.mode = ActorState::Mode::Idle;
        
    }
    return;
}


void ActorSystem::HandleInflictDamage(Actor* actor, Actor* target) {
    if (glm::distance(actor->navigation.mPosition, target->navigation.mPosition) > actor->behavior.mDistanceToInflict) 
        return;
    
    actor->counters.mAttackCoolDownCounter = actor->behavior.GetCooldownAttack();
    actor->state.mode = ActorState::Mode::Idle;
    
    // No effect if the target defense is greater them my strength
    if (target->biological.defense >= actor->biological.strength) 
        return;
    // Inflict damage 
    target->biological.health -= actor->biological.strength - target->biological.defense;
    
    if (target->biological.health <= 0.0f) {
        target->biological.health = 0.0f;
        
        wtf
        /*
        if (!target->isActive) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->state.current = ActorState::State::None;
        }
        */
        
    }
    
    return;
}


void ActorSystem::HandleVitality(Actor* actor) {
    // Check if the target is.. no longer with us
    if (actor->biological.health > 0.0f) 
        return;
    
    for (unsigned int i=0; i < actor->genetics.mGeneticRenderers.size(); i++) {
        MeshRenderer* actorRenderer = actor->genetics.mGeneticRenderers[i];
        actorRenderer->isActive = false;
    }
    
    actor->navigation.mTargetActor = nullptr;
    
    actor->isActive = false;
    actor->isGarbage = true;
    return;
}


void ActorSystem::UpdateProximityList(Actor* actor) {
    
    // Is the actor engaging with its target
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee || 
        actor->state.current == ActorState::State::Focus) 
        return;
    
    // Select a random actor
    unsigned int index = Random.Range(0, mActors.Size()-1);
    if (index >= mActors.Size()) 
        return;
    Actor* targetActor = mActors[ index ];
    
    // Check bad actor
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
    
    return;
}
