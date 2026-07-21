#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;

void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    actor->mTargets.clear();
    std::vector<float> distances;
    
    unsigned int numberOfActors = (unsigned int)mActiveActors.size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* targetActor = mActiveActors[i];
        
        if (!targetActor->isActive || targetActor->isGarbage)
            continue;
        
        if (targetActor == actor)
            continue;
        
        float distance = glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition);
        if (distance > actor->behavior.mDistanceToFocus)
            continue;
        
        actor->mTargets.push_back(targetActor);
        distances.push_back(distance);
    }
    
    unsigned int targetCount = (unsigned int)actor->mTargets.size();
    if (targetCount <= 1)
        return;
    
    // Sort an index list by distance
    std::vector<unsigned int> order;
    order.reserve(targetCount);
    for (unsigned int idx = 0; idx < targetCount; idx++)
        order.push_back(idx);
    
    std::sort(order.begin(), order.end(), [&](unsigned int lhsIdx, unsigned int rhsIdx) {return distances[lhsIdx] < distances[rhsIdx];});
    
    std::vector<Actor*> sortedTargets;
    sortedTargets.reserve(targetCount);
    for (unsigned int sortedIdx = 0; sortedIdx < targetCount; sortedIdx++)
        sortedTargets.push_back(actor->mTargets[order[sortedIdx]]);
    
    actor->mTargets.swap(sortedTargets);
}


void ActorSystem::HandleCooldownCounters(Actor* actor) {
    if (actor->counters.mMovementCoolDownCounter > 0) 
        actor->counters.mMovementCoolDownCounter--;
    
    if (actor->counters.mAttackCoolDownCounter > 0) 
        actor->counters.mAttackCoolDownCounter--;
    
    if (actor->counters.mObservationCoolDownCounter > 0) 
        actor->counters.mObservationCoolDownCounter--;
    
    if (actor->counters.mBreedingCoolDownCounter > 0) 
        actor->counters.mBreedingCoolDownCounter--;
    
    if (actor->counters.mSocialCoolDownCounter > 0) 
        actor->counters.mSocialCoolDownCounter--;
}


glm::vec3 ActorSystem::CalculateForwardVelocity(Actor* actor) {
    glm::vec3 forward(0);
    forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    // TODO should actors fly???
    //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) ); // Handle landing/flight/other
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


bool ActorSystem::HandleHomeLocation(Actor* actor) {
    const std::string& homePositionString = actor->memories.mMemories["home"];
    if (homePositionString != "") {
        std::vector<std::string> homeSplit = String.Explode(homePositionString, ',');
        if (homeSplit.size() == 5) {
            float timeBegin = String.ToFloat(homeSplit[3]);
            float timeEnd = String.ToFloat(homeSplit[4]);
            
            if (mTimeOfDay >= timeBegin && 
                mTimeOfDay <= timeEnd) {
                glm::vec3 homePosition;
                homePosition.x = String.ToFloat(homeSplit[0]);
                homePosition.y = String.ToFloat(homeSplit[1]);
                homePosition.z = String.ToFloat(homeSplit[2]);
                
                if (glm::distance(actor->navigation.mPosition, homePosition) > actor->behavior.mDistanceToWalk) {
                    
                    actor->navigation.mTargetPoint = homePosition;
                    actor->state.mode = ActorState::Mode::MoveTo;
                    return true;
                }
            }
        }
    }
    return false;
}


bool ActorSystem::HandleTargetDistance(Actor* actor) {
    glm::vec3 posA = actor->navigation.mPosition;
    glm::vec3 posB = actor->navigation.mTargetPoint;
    posA.y = 0.0f;
    posB.y = 0.0f;
    float distance = glm::distance(posA, posB);
    
    AI.DebugRenderDrawLine(posA, posB, Colors.white);
    
    // Check target over shoot
    
    // NOTE actors freezing before moving
    
    /*
    if (distance > actor->navigation.mDistanceToTarget) 
        actor->state.mode = ActorState::Mode::Idle;
    
    actor->navigation.mDistanceToTarget = distance;
    */
    
    // Check arrived at the target
    if (distance < actor->behavior.GetDistanceToInflict()) {
        
        // Hold at idle until we repeat the attack
        actor->state.mode = ActorState::Mode::Idle;
        return true;
    }
    return false;
}


bool ActorSystem::HandleInflictDamage(Actor* actor, Actor* target) {
    glm::vec3 posA = actor->navigation.mPosition;
    glm::vec3 posB = actor->navigation.mTargetPoint;
    posA.y = 0.0f;
    posB.y = 0.0f;
    
    // Check target over shoot
    float distance = glm::distance(posA, posB);
    if (distance > actor->navigation.mDistanceToTarget && 
        target->state.mode != ActorState::Mode::MoveFlee) {
        
        actor->state.mode = ActorState::Mode::Idle;
    }
    actor->navigation.mDistanceToTarget = distance;
    
    // Check arrived at the target
    
    if (distance > actor->behavior.GetDistanceToInflict()) 
        return false;
    
    actor->counters.mAttackCoolDownCounter = actor->behavior.GetCooldownAttack();
    
    // Hold at idle until we repeat the attack
    actor->state.mode = ActorState::Mode::Idle;
    actor->counters.SetCoolDownAttack( actor->behavior.GetCooldownAttack() );
    
    // Divide the effect if the target defense is greater them my strength
    if (target->biological.defense > (actor->biological.strength * actor->inventory.damageMul)) {
        target->biological.health -= (target->biological.defense / actor->biological.strength) - 1.0f;
    } else {
        // Inflict normal damage
        float damage = actor->biological.strength - target->biological.defense;
        if (damage > 0.0f) 
            target->biological.health -= damage * actor->inventory.damageMul;
    }
    
    // Check if we killed the target
    if (target->biological.health <= 0.0f) {
        target->biological.health = 0.0f;
        
        actor->state.mode = ActorState::Mode::Idle;
        actor->navigation.mTargetActor = nullptr;
    }
    return true;
}


bool ActorSystem::HandleEscapeEvade(Actor* actor, Actor* target) {
    /*
    float currentDist = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (currentDist > actor->behavior.GetDistanceToFlee()) {
        actor->state.mode = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        actor->navigation.mTargetActor = nullptr;
    }
    
    // Check if the actor starts moving away, will avoid the actor overshooting its target
    currentDist = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (currentDist > actor->navigation.mDistanceToTarget) {
        actor->state.mode = ActorState::Mode::Idle;
        actor->state.current = ActorState::State::None;
        actor->navigation.mTargetActor = nullptr;
        return;
    }
    actor->navigation.mDistanceToTarget = currentDist;
    */
    return true;
}

bool ActorSystem::HandleBreedWith(Actor* actor, Actor* target) {
    if (actor->counters.GetCoolDownBreeding() > 0 || target->counters.GetCoolDownBreeding() > 0) 
        return false;
    if (actor->physical.mAge < actor->physical.mAgeAdult || target->physical.mAge >= target->physical.mAgeSenior) 
        return false;
    
    // Check to ensure actors are of opposite sexes
    if (actor->physical.mSexualOrientation == target->physical.mSexualOrientation) 
        return false;
    
    // Check interaction distance
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (distance > actor->behavior.mDistanceToInflict) 
        return false;
    
    glm::vec3 spawnPoint = Math.Lerp(actor->navigation.mPosition, target->navigation.mPosition, 0.5f);
    
    Actor* offspring = CreateActor();
    offspring->Reset();
    
    // Genetic recombination
    genomes.BlendGenomes(actor, target, offspring);
    
    // Behavioral recombination
    CombineParentMemories(actor, target, offspring);
    
    std::string homePosition = Float.ToString(spawnPoint.x) + "," +
                               Float.ToString(spawnPoint.y) + "," +
                               Float.ToString(spawnPoint.z);
    offspring->memories.Add("home", homePosition);
    
    offspring->navigation.SetPosition(spawnPoint);
    offspring->navigation.SetTargetPoint(spawnPoint);
    
    offspring->physical.SetAge( Random.Range(1, 100) );
    offspring->RebuildGeneticExpression();
    
    offspring->isActive = true;
    
    mActiveActors.push_back(offspring);
    
    actor->counters.SetCoolDownBreeding(actor->behavior.GetCooldownBreed());
    target->counters.SetCoolDownBreeding(target->behavior.GetCooldownBreed());
    
    actor->emotions.current.libido = 0.0f;
    target->emotions.current.libido = 0.0f;
    
    actor->state.mode = ActorState::Mode::Idle;
    target->state.mode = ActorState::Mode::Idle;
    
    offspring->state.mode = ActorState::Mode::Idle;
    return true;
}

bool ActorSystem::HandleSocializeWith(Actor* actor, Actor* target) {
    // Change mObservationCoolDownCounter to mSocialCoolDownCounter
    if (actor->counters.mSocialCoolDownCounter > 0 || 
        target->counters.mSocialCoolDownCounter > 0) 
        return false;
    
    // Don't let someone socialize with an actor who is terrified or furious
    if (target->emotions.current.fear > UniversalConst.emotionalThreshold || 
        target->emotions.current.anger > UniversalConst.emotionalThreshold) 
        return false; 
    
    if (target->emotions.current.social < UniversalConst.emotionalThreshold) 
        return false; 
    
    // Check interaction distance
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (distance > actor->behavior.GetDistanceToInflict()) 
        return false;
    
    // Satisfy curiosity and boost comfort
    actor->emotions.SetComfort(glm::min(actor->emotions.GetComfort() + 0.2f, 1.0f));
    target->emotions.SetComfort(glm::min(target->emotions.GetComfort() + 0.2f, 1.0f));
    
    actor->emotions.SetCuriosity(0.0f);
    target->emotions.SetCuriosity(0.0f);
    
    actor->counters.mSocialCoolDownCounter  = actor->behavior.mCooldownObserve;
    target->counters.mSocialCoolDownCounter = actor->behavior.mCooldownObserve;
    
    actor->state.mode = ActorState::Mode::Idle;
    target->state.mode = ActorState::Mode::Idle;
    
    return true;
}
