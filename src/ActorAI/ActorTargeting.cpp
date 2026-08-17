#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
std::vector<float> tDistances;
std::vector<float> tAngerScores;
std::vector<unsigned int> tOrder;
std::vector<Actor*> tSortedTargets;

void ActorSystem::UpdateTargetingMechanics(Actor* actor) {
    ScopeProfile profile("AI-targeting");
    
    actor->mTargets.clear();
    tDistances.clear();
    tAngerScores.clear();
    
    unsigned int numberOfActors = (unsigned int)mActiveActors.size();
    for (unsigned int i = 0; i < numberOfActors; i++) {
        Actor* targetActor = mActiveActors[i];
        if (!targetActor->isActive || targetActor->isGarbage || targetActor == actor)
            continue;
        
        float distance = glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition);
        if (distance > actor->behavior.mDistanceToFocus)
            continue;
        
        float targetAnger = 0.0f;
        
        // ====================================================
        // Name-based anger trigger lookup
        
        std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator nameMemIt = actor->memories.mMemoryTriggers.find(targetActor->GetName());
        if (nameMemIt != actor->memories.mMemoryTriggers.end()) {
            for (const MemoryTrigger& trigger : nameMemIt->second) {
                if (trigger.type == TriggerType::Anger) {
                    targetAnger = std::max(targetAnger, trigger.value);
                }
            }
        }
        
        // ====================================================
        // Kingdom-based trigger lookup
        
        const std::string& targetKingdom = targetActor->memories.Get("kingdom");
        if (!targetKingdom.empty()) {
            std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator kingdomMemIt = actor->memories.mMemoryTriggers.find(targetKingdom);
            if (kingdomMemIt != actor->memories.mMemoryTriggers.end()) {
                for (const MemoryTrigger& trigger : kingdomMemIt->second) {
                    if (trigger.type == TriggerType::Anger) {
                        targetAnger = std::max(targetAnger, trigger.value);
                    }
                }
            }
        }
        
        actor->mTargets.push_back(targetActor);
        tDistances.push_back(distance);
        tAngerScores.push_back(targetAnger);
    }
    
    unsigned int targetCount = (unsigned int)actor->mTargets.size();
    if (targetCount <= 1)
        return;
    
    tOrder.resize(targetCount);
    for (unsigned int idx = 0; idx < targetCount; idx++)
        tOrder[idx] = idx;
    
    // Check if the actor is currently enraged
    bool isEnraged = actor->emotions.current.anger >= UniversalConst.emotionalThreshold;
    
    std::sort(tOrder.begin(), tOrder.end(), [&](unsigned int lhs, unsigned int rhs) {
        if (isEnraged) {
            // Prioritize the target that causes the highest anger
            if (std::abs(tAngerScores[lhs] - tAngerScores[rhs]) > 0.01f) {
                return tAngerScores[lhs] > tAngerScores[rhs];
            }
        }
        // Fallback or calm state: sort strictly by closest distance
        return tDistances[lhs] < tDistances[rhs];
    });
    
    tSortedTargets.clear();
    tSortedTargets.reserve(targetCount);
    for (unsigned int sortedIdx = 0; sortedIdx < targetCount; sortedIdx++)
        tSortedTargets.push_back(actor->mTargets[tOrder[sortedIdx]]);
    
    actor->mTargets.swap(tSortedTargets);
}

float ActorSystem::ApplyApproachSlowdown(Actor* actor, const glm::vec3& targetPosition, float currentSpeedScaler, float targetStopDistance) {
    if (targetStopDistance <= 0.0f) {
        targetStopDistance = actor->behavior.GetDistanceToInflict();
    }
    
    float distToTarget = glm::distance(actor->navigation.mPosition, targetPosition);
    float slowdownRadius = targetStopDistance * 3.0f;
    
    if (distToTarget < slowdownRadius && slowdownRadius > 0.0f) {
        float slowdownFactor = glm::clamp(distToTarget / slowdownRadius, 0.25f, 1.0f);
        return currentSpeedScaler * slowdownFactor;
    }
    
    return currentSpeedScaler;
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

void ActorSystem::CalculateTargetOffsetting(Actor* actor, float distanceScale) {
    if (actor->navigation.mTargetActor == nullptr) 
        return;
    
    glm::vec3 targetPos = actor->navigation.mTargetActor->navigation.mPosition;
    
    // Bitwise hash actor & target pointers to break pool alignment clustering
    uintptr_t actorId = reinterpret_cast<uintptr_t>(actor);
    uintptr_t targetId = reinterpret_cast<uintptr_t>(actor->navigation.mTargetActor);
    uintptr_t hash = actorId ^ (targetId + 0x9e3779b9 + (actorId << 6) + (actorId >> 2));
    
    // Uniform angle distribution (0 to 359 degrees)
    float angleDeg = static_cast<float>(hash % 360);
    float angleRad = glm::radians(angleDeg);
    
    // Expanded base radius with layered depth (varies distance from 80% to 140%)
    float baseRadius = actor->behavior.GetDistanceToInflict() * distanceScale;
    float depthStagger = 0.8f + (static_cast<float>((hash / 360) % 60) / 100.0f); // 0.80 to 1.40
    float radius = baseRadius * depthStagger;
    
    // Position offset around the target ring
    actor->navigation.mTargetPoint.x = targetPos.x + std::cos(angleRad) * radius;
    actor->navigation.mTargetPoint.y = targetPos.y;
    actor->navigation.mTargetPoint.z = targetPos.z + std::sin(angleRad) * radius;
    
    // Keep looking at the target
    actor->navigation.mTargetLook = targetPos;
}

bool ActorSystem::HandleHomeLocation(Actor* actor) {
    std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator homeIt = actor->memories.mMemoryTriggers.find("home");
    if (homeIt != actor->memories.mMemoryTriggers.end() && !homeIt->second.empty()) {
        const MemoryTrigger& homeTrigger = homeIt->second[0];
        glm::vec3 homePosition = homeTrigger.vector;
        
        // Check if an actor is outside their village 'home' radius
        if (glm::distance(actor->navigation.mPosition, homePosition) > actor->behavior.mDistanceToWalk) {
            actor->navigation.mTargetPoint = homePosition;
            actor->state.mode = ActorState::Mode::MoveTo;
            
            return true;
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
    
    //AI.DebugRenderDrawLine(posA, posB, Colors.white);
    
    // Check arrived at the target
    if (distance < actor->behavior.GetDistanceToInflict()) {
        actor->memories.ScaleEmotion(TriggerType::Curiosity, 0.1f);
        
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
    
    float distance = glm::distance(posA, posB);
    
    // Check arrived at the target
    if (distance > actor->behavior.GetDistanceToInflict()) 
        return false;
    
    actor->counters.mAttackCoolDownCounter = actor->behavior.GetCooldownAttack();
    
    // Hold at idle until we repeat the attack
    actor->state.mode = ActorState::Mode::Idle;
    actor->counters.SetCoolDownAttack( actor->behavior.GetCooldownAttack() );
    
    // Divide the effect if the target defense is greater than my strength
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
        
        actor->emotions.current.anger *= 0.1f;
        actor->memories.ScaleEmotion(TriggerType::Anger, 0.1f);
    }
    
    actor->emotions.current.comfort *= 0.5f;
    actor->memories.ScaleEmotion(TriggerType::Comfort, 0.5f);
    
    return true;
}

void ActorSystem::UpdateGazeTarget(Actor* actor) {
    // High Priority: Combat, Fleeing, or Socializing
    if (actor->navigation.mTargetActor != nullptr && 
        (actor->state.mode == ActorState::Mode::MoveAttack || 
        actor->state.mode == ActorState::Mode::MoveFlee || 
        actor->state.mode == ActorState::Mode::MoveSocialize)) {
        
        actor->navigation.mTargetLook = actor->navigation.mTargetActor->navigation.mPosition;
        return;
    }
    
    // Check gaze hold timer
    if (actor->counters.mObservationCoolDownCounter > 0) {
        return; // Keep looking at current mTargetLook until timer expires
    }
    
    // Reset gaze timer
    actor->counters.mObservationCoolDownCounter = static_cast<unsigned int>(Random.Range(1.0f, 8.0f));
    
    // Roll a chance for ambient gaze selection
    float roll = Random.Range(0.0f, 1.0f);
    float distToPlayer = glm::distance(mPlayerPosition, actor->navigation.mPosition);
    
    // 40% Chance: Look at Player if close enough & curious
    if (roll < 0.40f && distToPlayer <= actor->behavior.GetDistanceToFocus()) {
        actor->navigation.mTargetLook = mPlayerPosition;
        return;
    }
    
    // 30% Chance: Look at a nearby actor from mTargets list
    if (roll < 0.70f && !actor->mTargets.empty()) {
        size_t randomIndex = static_cast<size_t>(Random.Range(0.0f, static_cast<float>(actor->mTargets.size() - 1)));
        Actor* nearbyActor = actor->mTargets[randomIndex];
        if (nearbyActor && nearbyActor->isActive) {
            actor->navigation.mTargetLook = nearbyActor->navigation.mPosition;
            return;
        }
    }
    
    // 15% Chance: Random ambient glance (head turn offset)
    if (roll < 0.85f) {
        glm::vec3 randomOffset(
            Random.Range(-10.0f, 10.0f),
            Random.Range(-2.0f, 2.0f),
            Random.Range(-10.0f, 10.0f)
        );
        actor->navigation.mTargetLook = actor->navigation.mPosition + CalculateForwardVelocity(actor) * 5.0f + randomOffset;
        return;
    }
    
    // 15% Chance: Default - Look straight ahead in walking direction
    actor->navigation.mTargetLook = actor->navigation.mPosition + CalculateForwardVelocity(actor) * 10.0f;
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
