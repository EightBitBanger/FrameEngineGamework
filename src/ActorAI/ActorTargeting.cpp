#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    HandleTargetDistance(actor);
    
    if (actor->navigation.mTargetActor == nullptr) 
        return;
    
    glm::vec3 position = actor->navigation.mTargetActor->navigation.mPosition;
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
            
            HandleEscapeEvade(actor, actor->navigation.mTargetActor);
            break;
            
        case ActorState::State::Observe:
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            break;
            
        case ActorState::State::Look:
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            break;
            
        case ActorState::State::Breed:
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook = position;
            actor->state.mIsFacing = true;
            
            HandleBreedWith(actor, actor->navigation.mTargetActor);
            break;
            
    }
    return;
}


void ActorSystem::HandleTargetDistance(Actor* actor) {
    if (actor->state.current == ActorState::State::Attack || 
        actor->state.current == ActorState::State::Flee) 
        return;
    
    if (actor->navigation.mTargetPoint == glm::vec3(0)) 
        return;
    
    // Check arrived at target actor
    glm::vec3 posA = actor->navigation.mPosition;
    glm::vec3 posB = actor->navigation.mTargetPoint;
    posA.y = 0.0f;
    posB.y = 0.0f;
    
    float distance = glm::distance(posA, posB);
    if (distance > actor->navigation.mDistanceToTarget) 
        actor->state.mode = ActorState::Mode::Idle;
    
    actor->navigation.mDistanceToTarget = distance;
    if (actor->navigation.mDistanceToTarget < actor->behavior.GetDistanceToInflict()) {
        
        // Hold at idle until we repeat the attack
        actor->state.mode = ActorState::Mode::Idle;
        
    }
    
}

void ActorSystem::HandleInflictDamage(Actor* actor, Actor* target) {
    float currentDist = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (currentDist > actor->navigation.mDistanceToTarget) {
        actor->state.mode = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        actor->navigation.mTargetActor = nullptr;
        return;
    }
    actor->navigation.mDistanceToTarget = currentDist;
    if (actor->navigation.mDistanceToTarget > actor->behavior.mDistanceToInflict) 
        return;
    
    actor->counters.mAttackCoolDownCounter = actor->behavior.GetCooldownAttack();
    actor->state.mode = ActorState::Mode::MoveTo;
    
    // No effect if the target defense is greater them my strength
    if (target->biological.defense >= actor->biological.strength) 
        return;
    // Inflict damage 
    target->biological.health -= actor->biological.strength - target->biological.defense;
    
    if (target->biological.health <= 0.0f) {
        target->biological.health = 0.0f;
        
        actor->state.mode = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        actor->navigation.mTargetActor = nullptr;
    }
}

void ActorSystem::HandleEscapeEvade(Actor* actor, Actor* target) {
    float currentDist = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (currentDist > actor->navigation.mDistanceToTarget) {
        actor->state.mode = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        actor->navigation.mTargetActor = nullptr;
        return;
    }
    actor->navigation.mDistanceToTarget = currentDist;
    
}


void ActorSystem::HandleBreedWith(Actor* actor, Actor* target) {
    if (actor->counters.GetCoolDownBreeding() > 0 || target->counters.GetCoolDownBreeding() > 0) 
        return;
    if (glm::distance(actor->navigation.mPosition, target->navigation.mPosition) > actor->behavior.mDistanceToInflict) 
        return;
    
    glm::vec3 spawnPoint = Math.Lerp(actor->navigation.mPosition, target->navigation.mPosition, 0.5f);
    
    Actor* offspring = CreateActor();
    offspring->Reset();
    
    std::string homePosition = Float.ToString(spawnPoint.x) + "," +
                               Float.ToString(spawnPoint.y) + "," +
                               Float.ToString(spawnPoint.z);
    actor->memories.Add("home", homePosition);
    
    offspring->navigation.SetPosition(spawnPoint);
    offspring->navigation.SetTargetPoint(spawnPoint);
    
    offspring->physical.SetAge( Random.Range(20, 80) );
    genomes.BlendGenomes(actor, target, offspring);
    offspring->RebuildGeneticExpression();
    
    offspring->isActive = true;
    offspring->physical.UpdatePhysicalCollider();
    
    mActiveActors.push_back(offspring);
    
    actor->counters.SetCoolDownBreeding(120);
    target->counters.SetCoolDownBreeding(120);
    
    actor->state.current = ActorState::State::None;
    actor->state.mode = ActorState::Mode::MoveRandom;
    
    target->state.current = ActorState::State::None;
    target->state.mode = ActorState::Mode::MoveRandom;
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
}

