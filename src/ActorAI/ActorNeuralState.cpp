#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

thread_local std::vector<EmotionalEmbedding> tThoughtMatrix;

static const std::vector<std::pair<TriggerType, ActorState::Mode>> BehavioralAssociation = {
    {TriggerType::Anger,     ActorState::Mode::MoveAttack},
    {TriggerType::Fear,      ActorState::Mode::MoveFlee},
    {TriggerType::Libido,    ActorState::Mode::MoveBreed},
    {TriggerType::Curiosity, ActorState::Mode::MoveRandom}
};

void ActorSystem::UpdateActorState(Actor* actor, EmotionalEmbedding& emotion, float sentientScore) {
    ScopeProfile profile("AI-state");
    
    // Check if fleeing state should naturally terminate
    if (actor->state.mode == ActorState::Mode::MoveFlee) {
        bool targetEscaped = false;
        if (actor->navigation.mTargetActor != nullptr) {
            float dist = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition);
            if (dist > actor->behavior.GetDistanceToFlee()) {
                targetEscaped = true;
            }
        } else {
            targetEscaped = true;
        }
        
        if (targetEscaped) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->navigation.mTargetActor = nullptr;
            
            // Hard reset fear upon successful escape so history matrix decays quickly
            actor->emotions.current.fear = 0.0f;
            actor->memories.ClearEmotion(TriggerType::Fear);
        }
    }
    
    // De-escalate attack when anger cools down completely
    if (emotion.anger < UniversalConst.emotionalThreshold && actor->counters.mAttackCoolDownCounter == 0) {
        if (actor->state.mode == ActorState::Mode::MoveAttack) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->navigation.mTargetActor = nullptr;
        }
        
        // Unequip weapon
        if (!actor->inventory.inHandItemClass.empty()) {
            actor->inventory.UnequipItem();
        }
    }
    
    // Keep actor nearby its associated "home" location
    if (actor->state.mode != ActorState::Mode::MoveAttack && 
        actor->state.mode != ActorState::Mode::MoveFlee && 
        actor->state.mode != ActorState::Mode::MoveHunting) {
        HandleHomeLocation(actor);
    }
    
    // Degrade emotional state for non sentient actors
    float degradeFactor = 0.8f;
    if (sentientScore < UniversalConst.sentienceThreshold) 
        degradeFactor = 0.9f;
    
    EmotionalEmbedding degradeEmbedding;
    degradeEmbedding.fear       = degradeFactor;
    degradeEmbedding.anger      = degradeFactor;
    degradeEmbedding.comfort    = 1.0f;
    degradeEmbedding.curiosity  = 1.0f;
    degradeEmbedding.fatigue    = 1.0f;
    degradeEmbedding.stress     = 1.0f;
    degradeEmbedding.libido     = 1.0f;
    degradeEmbedding.social     = 1.0f;
    
    actor->memories.DegradeEmotionalState(degradeEmbedding, 0.3f);
    actor->emotions.current *= degradeEmbedding;
    
}

void ActorSystem::UpdateThoughtMatrix(Actor* actor, EmotionalEmbedding& emotion, float sentientScore) {
    ScopeProfile profile("AI-thinking");
    
    tThoughtMatrix.clear();
    int bestTargetIndex = EvaluateThoughtMatrix(actor, emotion, sentientScore, tThoughtMatrix);
    
    if (bestTargetIndex != -1) {
        Actor* chosenTarget = actor->mTargets[bestTargetIndex];
        
        // Preserve active social target unless a real threat interrupts
        if (actor->state.mode == ActorState::Mode::MoveSocialize && actor->navigation.mTargetActor != nullptr) {
            float maxThreat = glm::max(tThoughtMatrix[bestTargetIndex].anger, tThoughtMatrix[bestTargetIndex].fear);
            if (maxThreat >= UniversalConst.emotionalThreshold) {
                actor->state.mode = ActorState::Mode::Idle;
            } else if (actor->navigation.mTargetActor->isActive && !actor->navigation.mTargetActor->isGarbage) {
                chosenTarget = actor->navigation.mTargetActor;
            } else {
                actor->navigation.mTargetActor = nullptr;
                actor->state.mode = ActorState::Mode::Idle;
            }
        }
        
        EvaluateEmotionalBehavior(actor, chosenTarget, UniversalConst.emotionalThreshold, tThoughtMatrix[bestTargetIndex]);
        emotion = tThoughtMatrix[bestTargetIndex];
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    } else {
        // Reset state back to Idle when target matrix evaluation finds no targets
        if (actor->state.mode == ActorState::Mode::MoveFlee || actor->state.mode == ActorState::Mode::MoveAttack) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->emotions.current.fear = 0.0f;
            actor->memories.ClearEmotion(TriggerType::Fear);
        }
        
        actor->navigation.mTargetActor = nullptr;
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    }
    
    ProjectEmotionalHistory(actor, emotion, sentientScore);
}

void ActorSystem::ProcessMemoryTriggers(Actor* actor, EmotionalEmbedding& emotion) {
    ScopeProfile profile("AI-memories");
    
    const std::vector<MemoryTrigger>& behaviorList = actor->memories.mMemoryTriggers["behavior"];
    if (behaviorList.empty()) return;
    
    for (const MemoryTrigger& trigger : behaviorList) {
        float chance = trigger.value;
        if (Random.Range(0.0f, 1.0f) >= glm::pow(chance, 0.7f)) continue;
        
        switch (trigger.type) {
            case TriggerType::Curiosity:
                if (emotion.curiosity > UniversalConst.emotionalThreshold) emotion.curiosity = 0.0f;
                emotion.curiosity += chance;
                break;
                
            case TriggerType::Libido:
                if (actor->counters.mBreedingCoolDownCounter == 0 && actor->physical.mAge >= actor->physical.mAgeAdult) {
                    emotion.libido += chance;
                }
                break;
                
            case TriggerType::Social:
                if (actor->counters.mSocialCoolDownCounter == 0) {
                    emotion.social += chance;
                }
                break;
                
            case TriggerType::Speak: {
                const std::string& voiceName = actor->memories.Get("speak_voice");
                Sound* voice = actor->voice.GetVoice(voiceName);
                if (voice != nullptr) {
                    Playback* playback = Audio.Play(voice);
                    playback->isGarbage = true;
                }
                break;
            }
            default:
                break;
        }
    }
}

void ActorSystem::ApplyEmotionThresholds(const MemoryTrigger& trigger, EmotionalEmbedding& embedding) {
    float* emotion = (trigger.type != TriggerType::Unknown) 
                   ? embedding.GetEmotionByTrigger(trigger.type) 
                   : embedding.GetEmotionByName(trigger.name);
    if (emotion != nullptr) {
        *emotion = glm::max(*emotion, trigger.value);
    }
}

void ActorSystem::ProjectEmotionalHistory(Actor* actor, EmotionalEmbedding& currentEmotion, float sentientScore) {
    float maxHistorySize = 2.0f + (glm::clamp(sentientScore, 0.0f, 1.0f) * 8.0f);
    
    if (actor->memories.thoughtProcessProjectionMatrix.size() > maxHistorySize) {
        actor->memories.thoughtProcessProjectionMatrix.pop_front();
    }
    
    if (!actor->memories.thoughtProcessProjectionMatrix.empty()) {
        float totalWeight = 0.0f;
        float weightMultiplier = 1.0f;
        
        currentEmotion.Clear();
        for (const EmotionalEmbedding& historicalThought : actor->memories.thoughtProcessProjectionMatrix) {
            currentEmotion += historicalThought * weightMultiplier;
            totalWeight += weightMultiplier;
            weightMultiplier += 1.0f; 
        }
        currentEmotion /= totalWeight;
    }
}

int ActorSystem::GetSocialGroupSize(Actor* focalActor) {
    if (!focalActor) return 0;
    int groupSize = 1; // Count focalActor itself
    for (Actor* candidate : focalActor->mTargets) {
        if (candidate && candidate->isActive && !candidate->isGarbage) {
            if (candidate->state.mode == ActorState::Mode::MoveSocialize &&
               (candidate->navigation.mTargetActor == focalActor || focalActor->navigation.mTargetActor == candidate)) {
                groupSize++;
            }
        }
    }
    return groupSize;
}

int ActorSystem::EvaluateThoughtMatrix(Actor* actor, const EmotionalEmbedding& baselineEmotion, float sentientScore, std::vector<EmotionalEmbedding>& outThoughtMatrix) {
    unsigned int numberOfTargets = actor->mTargets.size();
    outThoughtMatrix.resize(numberOfTargets, baselineEmotion);
    
    const std::string& kingdom = actor->memories.Get("kingdom");
    
    int bestTargetIndex = -1;
    float highestIntensity = -1.0f;
    for (unsigned int i = 0; i < numberOfTargets; i++) {
        Actor* targetActor = actor->mTargets[i];
        if (!targetActor->isActive || targetActor->isGarbage) continue;
        
        float targetDist = glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition);
        // Apply personal memory triggers with target
        std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator targetMemIt;
        targetMemIt = actor->memories.mMemoryTriggers.find(targetActor->GetName());
        if (targetMemIt != actor->memories.mMemoryTriggers.end()) {
            for (const MemoryTrigger& trigger : targetMemIt->second) {
                // Ignore target fear triggers if target is outside fleeing distance
                if (trigger.type == TriggerType::Fear && targetDist > actor->behavior.GetDistanceToFlee()) {
                    continue;
                }
                ApplyEmotionThresholds(trigger, outThoughtMatrix[i]);
            }
        }
        
        // High-sentience kingdom relationships
        if (sentientScore >= UniversalConst.sentienceThreshold) {
            const std::string& targetKingdom = targetActor->memories.Get("kingdom");
            
            if (!kingdom.empty() && !targetKingdom.empty()) {
                if (kingdom == targetKingdom) {
                    outThoughtMatrix[i].anger   = 0.0f;
                    outThoughtMatrix[i].fear    = 0.0f;
                    outThoughtMatrix[i].comfort = glm::max(outThoughtMatrix[i].comfort, 0.8f);
                }
                
                std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator kingdomIt;
                kingdomIt = actor->memories.mMemoryTriggers.find(targetKingdom);
                if (kingdomIt != actor->memories.mMemoryTriggers.end()) {
                    for (const MemoryTrigger& trigger : kingdomIt->second) {
                        ApplyEmotionThresholds(trigger, outThoughtMatrix[i]);
                    }
                }
            }
        }
        
        // Target is available if Idle/Wandering, OR if already socializing in a group of less than 4
        bool isTargetAvailableForSocial = false;
        if (targetActor->state.mode == ActorState::Mode::Idle || 
            targetActor->state.mode == ActorState::Mode::MoveRandom) {
            isTargetAvailableForSocial = true;
        } else if (targetActor->state.mode == ActorState::Mode::MoveSocialize) {
            if (GetSocialGroupSize(targetActor) < 4) {
                isTargetAvailableForSocial = true;
            }
        }
        
        float effectiveSocialDrive = isTargetAvailableForSocial ? outThoughtMatrix[i].social : 0.0f;
        
        // Calculate base intensity across ALL active emotional drives
        float intensity = glm::max(
            glm::max(outThoughtMatrix[i].anger, outThoughtMatrix[i].fear),
            glm::max(
                glm::max(outThoughtMatrix[i].libido, outThoughtMatrix[i].curiosity),
                effectiveSocialDrive
            )
        );
        float effectiveScore = intensity;
        
        // Target hold in sights
        if (actor->navigation.mTargetActor != nullptr && targetActor == actor->navigation.mTargetActor) {
            effectiveScore += 0.4f;
        }
        
        // Gender preference in combat
        if (outThoughtMatrix[i].anger >= UniversalConst.emotionalThreshold) {
            bool isMaleTarget = targetActor->physical.GetSexualOrientation();
            if (isMaleTarget) {
                effectiveScore += 0.2f;
            }
        }
        
        // Deduct interest ONLY if target is engaged with someone else in combat/fleeing
        bool isEngagedWithOther = (targetActor->navigation.mTargetActor != nullptr && 
                                   targetActor->navigation.mTargetActor != actor && 
                                  (targetActor->state.mode == ActorState::Mode::MoveAttack || 
                                   targetActor->state.mode == ActorState::Mode::MoveFlee));
        if (isEngagedWithOther) {
            effectiveScore -= 0.5f;
        }
            
        if (effectiveScore > highestIntensity) {
            highestIntensity = effectiveScore;
            bestTargetIndex = i;
        }
    }
    
    return bestTargetIndex;
}

bool ActorSystem::EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& emotion) {
    // Lock on combat target while active or recovering from cooldown
    if ((actor->state.mode == ActorState::Mode::MoveAttack || actor->counters.mAttackCoolDownCounter > 0) && 
        actor->navigation.mTargetActor != nullptr && 
        actor->navigation.mTargetActor->isActive && 
        !actor->navigation.mTargetActor->isGarbage) {
        targetActor = actor->navigation.mTargetActor;
    }
    
    // =========================================================================
    // SURVIVAL & COMBAT (Fear > Anger)
    
    // Fear (Fleeing)
    if (emotion.fear >= threshold && emotion.fear >= emotion.anger) {
        emotion.comfort *= 0.1f;
        actor->memories.ScaleEmotion(TriggerType::Comfort, 0.1f);
        
        if (actor->state.mode != ActorState::Mode::MoveFlee) {
            if (targetActor != nullptr && glm::distance(actor->navigation.mPosition, targetActor->navigation.mPosition) <= actor->behavior.GetDistanceToFlee()) {
                if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.fear, threshold)) {
                    actor->state.mode = ActorState::Mode::MoveFlee;
                    actor->inventory.UnequipItem();
                    actor->navigation.mTargetActor = targetActor;
                    return true;
                }
            }
        } else {
            return true;
        }
    }
    
    // Anger (Attacking)
    bool isHostile = (emotion.anger >= threshold || actor->counters.mAttackCoolDownCounter > 0);
    if (emotion.anger >= threshold && emotion.anger > emotion.fear) {
        if (actor->counters.mAttackCoolDownCounter == 0) {
            actor->state.mode = ActorState::Mode::MoveAttack;
            emotion.comfort *= 0.3f;
            actor->memories.ScaleEmotion(TriggerType::Comfort, 0.3f);
            
            if (actor->inventory.inHandItemClass.empty()) 
                actor->inventory.EquipWeapon();
            actor->navigation.mTargetActor = targetActor;
            actor->navigation.mDistanceToTarget = glm::distance(
                glm::vec3(targetActor->navigation.mPosition.x, 0.0f, targetActor->navigation.mPosition.z),
                glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
            );
            return true;
        } else {
            return false;
        }
    }
    
    // Block non-combat behaviors completely while hostile
    if (isHostile) {
        return false;
    }
    
    // =========================================================================
    // SOCIALIZATION (Socialize > Breed)
    
    bool isSocialEligible = (emotion.social >= threshold && 
                            actor->counters.mSocialCoolDownCounter == 0 &&
                            emotion.stress < UniversalConst.emotionalThreshold && 
                            emotion.fear < UniversalConst.emotionalThreshold && 
                            emotion.anger < UniversalConst.emotionalThreshold);
    if (isSocialEligible && targetActor != nullptr) {
        // Verify target availability (Idle/Wandering OR social group size < 4)
        bool isTargetAvailable = false;
        if (targetActor->state.mode == ActorState::Mode::Idle || 
            targetActor->state.mode == ActorState::Mode::MoveRandom) {
            isTargetAvailable = true;
        } else if (targetActor->state.mode == ActorState::Mode::MoveSocialize) {
            if (GetSocialGroupSize(targetActor) < 4) {
                isTargetAvailable = true;
            }
        }
        
        if (isTargetAvailable && Random.Range(0.0f, 1.0f) < glm::pow(emotion.social, threshold)) {
            // Lock active actor into socializing toward targetActor
            actor->navigation.mTargetActor = targetActor;
            actor->state.mode = ActorState::Mode::MoveSocialize;
            
            glm::vec3 pos = actor->navigation.mPosition;
            glm::vec3 tarPos = targetActor->navigation.mPosition;
            float dist = glm::distance(glm::vec3(tarPos.x, 0.0f, tarPos.z), glm::vec3(pos.x, 0.0f, pos.z));
            actor->navigation.mDistanceToTarget = dist;
            
            // If target was unengaged, lock them back onto initiating actor
            if (targetActor->state.mode != ActorState::Mode::MoveSocialize) {
                targetActor->navigation.mTargetActor = actor;
                targetActor->state.mode = ActorState::Mode::MoveSocialize;
                targetActor->navigation.mDistanceToTarget = dist;
            }
            
            return true;
        }
        return false;
    }

    // =========================================================================
    // REPRODUCTION
    
    bool isBreedEligible = (emotion.libido >= threshold && 
                        actor->counters.mBreedingCoolDownCounter == 0 &&
                        emotion.fear < UniversalConst.emotionalThreshold &&
                        emotion.anger < UniversalConst.emotionalThreshold);
    if (isBreedEligible) {
        if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.libido, threshold)) {
            Actor* viableMate = nullptr;
            for (Actor* candidate : actor->mTargets) {
                if (!candidate || !candidate->isActive || candidate->isGarbage) continue;
                if (candidate->physical.GetSexualOrientation() != actor->physical.GetSexualOrientation() &&
                    candidate->physical.mAge >= candidate->physical.mAgeAdult &&
                    candidate->physical.mAge < candidate->physical.mAgeSenior &&
                    candidate->counters.GetCoolDownBreeding() == 0) {
                    viableMate = candidate;
                    break;
                }
            }
            
            if (viableMate != nullptr) {
                // Lock active actor onto viableMate
                actor->navigation.mTargetActor = viableMate;
                actor->state.mode = ActorState::Mode::MoveBreed;
                
                glm::vec3 pos = actor->navigation.mPosition;
                glm::vec3 tarPos = viableMate->navigation.mPosition;
                float dist = glm::distance(glm::vec3(tarPos.x, 0.0f, tarPos.z), glm::vec3(pos.x, 0.0f, pos.z));
                actor->navigation.mDistanceToTarget = dist;
                
                // Lock viableMate back onto the initiating actor
                if (viableMate->state.mode != ActorState::Mode::MoveBreed) {
                    viableMate->navigation.mTargetActor = actor;
                    viableMate->state.mode = ActorState::Mode::MoveBreed;
                    viableMate->navigation.mDistanceToTarget = dist;
                }
                
                return true;
            } else {
                actor->counters.mBreedingCoolDownCounter = actor->behavior.mCooldownObserve;
                actor->emotions.current.libido *= 0.5f;
                actor->memories.ScaleEmotion(TriggerType::Libido, 0.5f);
            }
        }
    }
    
    // =========================================================================
    // WANDER / EXPLORATION
    
    if (emotion.curiosity >= threshold && actor->state.mode == ActorState::Mode::Idle) {
        if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.curiosity, threshold)) {
            actor->state.mode = ActorState::Mode::MoveRandom;
            CalculateRandomLocalPoint(actor);
            actor->navigation.mDistanceToTarget = glm::distance(
                glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
            );
            return true;
        }
    }

    return false;
}
