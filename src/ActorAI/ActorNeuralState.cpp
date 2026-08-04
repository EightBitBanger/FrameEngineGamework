#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

static const std::vector<std::pair<TriggerType, ActorState::Mode>> BehavioralAssociation = {
    {TriggerType::Anger,     ActorState::Mode::MoveAttack},
    {TriggerType::Fear,      ActorState::Mode::MoveFlee},
    {TriggerType::Libido,    ActorState::Mode::MoveBreed},
    {TriggerType::Curiosity, ActorState::Mode::MoveRandom}
};

void ActorSystem::UpdateActorState(Actor* actor) {
    EmotionalEmbedding& emotion = actor->emotions.current;

    // Check if fleeing target has escaped beyond flee distance
    if (actor->state.mode == ActorState::Mode::MoveFlee && actor->navigation.mTargetActor != nullptr) {
        float dist = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition);
        if (dist > actor->behavior.GetDistanceToFlee()) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->navigation.mTargetActor = nullptr;
        }
    }

    // De-escalate and unequip weapon when anger drops below threshold
    if (emotion.anger < UniversalConst.emotionalThreshold && 
        emotion.comfort > 0.0f) {
        if (actor->state.mode == ActorState::Mode::MoveAttack) {
            actor->state.mode = ActorState::Mode::Idle;
            actor->navigation.mTargetActor = nullptr;
        }
        if (!actor->inventory.inHandItemClass.empty()) {
            actor->inventory.UnequipItem();
        }
    }

    // De-escalate flee mode when fear drops below threshold
    if (actor->state.mode == ActorState::Mode::MoveFlee && emotion.fear < UniversalConst.emotionalThreshold) {
        actor->state.mode = ActorState::Mode::Idle;
        actor->navigation.mTargetActor = nullptr;
    }
    
    // Fetch baseline sentience score
    const std::vector<MemoryTrigger>& sentienceList = actor->memories.mMemoryTriggers["sentience"];
    float sentientScore = !sentienceList.empty() ? sentienceList[0].value : 0.0f;
    
    ProcessMemoryTriggers(actor, emotion);
    
    std::vector<EmotionalEmbedding> thoughtMatrix;
    int bestTargetIndex = EvaluateThoughtMatrix(actor, emotion, sentientScore, thoughtMatrix);
    
    if (bestTargetIndex != -1) {
        Actor* chosenTarget = actor->mTargets[bestTargetIndex];
        if (actor->state.mode == ActorState::Mode::MoveSocialize && actor->navigation.mTargetActor != nullptr) {
            float maxThreat = glm::max(thoughtMatrix[bestTargetIndex].anger, thoughtMatrix[bestTargetIndex].fear);
            if (maxThreat >= UniversalConst.emotionalThreshold) {
                actor->state.mode = ActorState::Mode::Idle;
            } else if (actor->navigation.mTargetActor->isActive && !actor->navigation.mTargetActor->isGarbage) {
                chosenTarget = actor->navigation.mTargetActor;
            } else {
                actor->navigation.mTargetActor = nullptr;
                actor->state.mode = ActorState::Mode::Idle;
            }
        }
        
        EvaluateEmotionalBehavior(actor, chosenTarget, UniversalConst.emotionalThreshold, thoughtMatrix[bestTargetIndex]);
        emotion = thoughtMatrix[bestTargetIndex];
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    } else {
        
        actor->navigation.mTargetActor = nullptr;
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    }
    
    ProjectEmotionalHistory(actor, emotion, sentientScore);
}

void ActorSystem::ProcessMemoryTriggers(Actor* actor, EmotionalEmbedding& emotion) {
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
                std::string voiceName = actor->memories.Get("speak_voice");
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

int ActorSystem::EvaluateThoughtMatrix(Actor* actor, const EmotionalEmbedding& baselineEmotion, float sentientScore, std::vector<EmotionalEmbedding>& outThoughtMatrix) {
    unsigned int numberOfTargets = actor->mTargets.size();
    outThoughtMatrix.resize(numberOfTargets, baselineEmotion);
    
    int bestTargetIndex = -1;
    float highestIntensity = -1.0f;
    for (unsigned int i = 0; i < numberOfTargets; i++) {
        Actor* targetActor = actor->mTargets[i];
        if (!targetActor->isActive || targetActor->isGarbage) continue;
        
        // Apply personal memory triggers with target
        std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator targetMemIt;
        targetMemIt = actor->memories.mMemoryTriggers.find(targetActor->GetName());
        
        if (targetMemIt != actor->memories.mMemoryTriggers.end()) {
            for (const MemoryTrigger& trigger : targetMemIt->second) {
                ApplyEmotionThresholds(trigger, outThoughtMatrix[i]);
            }
        }
        
        // Apply high-sentience kingdom relationships
        if (sentientScore >= UniversalConst.sentienceThreshold) {
            std::string kingdom = actor->memories.Get("kingdom");
            std::string targetKingdom = targetActor->memories.Get("kingdom");
            
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
        
        // Calculate base emotional intensity
        float intensity = glm::max(
            glm::max(outThoughtMatrix[i].anger, outThoughtMatrix[i].fear),
            glm::max(outThoughtMatrix[i].libido, outThoughtMatrix[i].curiosity)
        );
        
        float effectiveScore = intensity;
        
        // If anger is driving the action, prefer male combatants over females
        if (outThoughtMatrix[i].anger >= UniversalConst.emotionalThreshold) {
            bool isMaleTarget = targetActor->physical.GetSexualOrientation();
            if (isMaleTarget) {
                effectiveScore += 0.2f;
            }
        }
        
        if (effectiveScore > highestIntensity) {
            highestIntensity = effectiveScore;
            bestTargetIndex = i;
        }
    }
    
    return bestTargetIndex;
}

void ActorSystem::ProjectEmotionalHistory(Actor* actor, EmotionalEmbedding& currentEmotion, float sentientScore) {
    // Cap thought history size based on sentience depth
    float maxHistorySize = (UniversalConst.sentienceThreshold - sentientScore) * 2.0f;
    
    if (actor->memories.thoughtProcessProjectionMatrix.size() > maxHistorySize) {
        actor->memories.thoughtProcessProjectionMatrix.pop_front();
    }
    
    // Apply weighted moving average to project the emotional state
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

bool ActorSystem::EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& emotion) {
    // Check socialization state trigger
    if (actor->state.mode != ActorState::Mode::MoveSocialize && 
        emotion.social > emotion.anger && emotion.social > emotion.fear && emotion.social >= threshold && 
        actor->counters.mSocialCoolDownCounter == 0 &&
        emotion.stress < UniversalConst.emotionalThreshold && 
        emotion.anger < UniversalConst.emotionalThreshold && 
        emotion.comfort >= UniversalConst.emotionalThreshold) {
        
        if (Random.Range(0.0f, 1.0f) < glm::pow(emotion.social, threshold)) {
            actor->navigation.mTargetActor = targetActor;
            actor->state.mode = ActorState::Mode::MoveSocialize;
            
            glm::vec3 pos = actor->navigation.mPosition;
            glm::vec3 tarPos = targetActor->navigation.mPosition;
            actor->navigation.mDistanceToTarget = glm::distance(glm::vec3(tarPos.x, 0.0f, tarPos.z), glm::vec3(pos.x, 0.0f, pos.z));
            return true;
        }
    }
    
    for (const auto& behavior : BehavioralAssociation) {
        TriggerType type = behavior.first;
        ActorState::Mode mode = behavior.second;
        
        float* emotionValPtr = emotion.GetEmotionByTrigger(type);
        if (!emotionValPtr) continue;
        float value = *emotionValPtr;
        
        if (value <= threshold) continue;
        
        // Allow fear to take precedence over anger if fear >= anger
        if (type == TriggerType::Anger && value < emotion.fear) continue;
        if (type != TriggerType::Anger && type != TriggerType::Fear && value <= emotion.anger) continue;
        if (type != TriggerType::Anger && type != TriggerType::Fear && value <= emotion.fear) continue;
        if (mode == ActorState::Mode::MoveAttack && (actor->state.mode == ActorState::Mode::MoveAttack || actor->counters.mAttackCoolDownCounter != 0)) continue;
        if (mode == ActorState::Mode::MoveBreed && actor->counters.mBreedingCoolDownCounter != 0) continue;
        
        if (mode == ActorState::Mode::MoveRandom && (actor->state.mode == ActorState::Mode::MoveAttack || 
            actor->state.mode == ActorState::Mode::MoveFlee || 
            actor->state.mode == ActorState::Mode::MoveSocialize || 
            actor->state.mode == ActorState::Mode::MoveBreed)) 
            continue;
            
        // Execute state transition
        if (Random.Range(0.0f, 1.0f) < glm::pow(value, threshold)) {
            actor->state.mode = mode;
            if (mode == ActorState::Mode::MoveFlee) {
                actor->inventory.UnequipItem(); // Ensure weapon is put away when fleeing
            }
            
            if (mode == ActorState::Mode::MoveRandom) {
                CalculateRandomLocalPoint(actor);
                actor->navigation.mDistanceToTarget = glm::distance(
                    glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                    glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
                );
            } 
            else if (mode == ActorState::Mode::MoveBreed) {
                if (!actor->mTargets.empty()) actor->navigation.mTargetActor = actor->mTargets[0];
            } else {
                actor->navigation.mTargetActor = targetActor;
                if (mode == ActorState::Mode::MoveAttack) {
                    emotion.comfort *= 0.3f;
                    actor->memories.ScaleEmotion(TriggerType::Comfort, 0.3f);
                    
                    if (actor->inventory.inHandItemClass.empty()) 
                        actor->inventory.EquipWeapon();
                    
                    actor->navigation.mDistanceToTarget = glm::distance(
                        glm::vec3(targetActor->navigation.mPosition.x, 0.0f, targetActor->navigation.mPosition.z),
                        glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
                    );
                }
            }
            return true;
        }
    }
    
    return false;
}
