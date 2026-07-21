#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;

static const std::vector<std::pair<std::string, ActorState::Mode>> BehavioralAssociation = {
    {"anger",     ActorState::Mode::MoveAttack},
    {"fear",      ActorState::Mode::MoveFlee},
    {"libido",    ActorState::Mode::MoveBreed},
    {"curiosity", ActorState::Mode::MoveRandom}
};

extern EngineSystemManager Engine;

void ActorSystem::UpdateActorState(Actor* actor) {
    EmotionalEmbedding& emotion = actor->emotions.current;
    
    // Fetch baseline sentience score
    const std::vector<MemoryTrigger>& sentienceList = actor->memories.mMemoryTriggers["sentience"];
    float sentientScore = !sentienceList.empty() ? sentienceList[0].value : 0.0f;
    
    ProcessMemoryTriggers(actor, emotion);
    
    // Evaluate target relationship and pick the most intense stimulus
    std::vector<EmotionalEmbedding> thoughtMatrix;
    int bestTargetIndex = EvaluateThoughtMatrix(actor, emotion, sentientScore, thoughtMatrix);
    
    // Commit results to the thought matrix
    if (bestTargetIndex != -1) {
        Actor* chosenTarget = actor->mTargets[bestTargetIndex];
        EvaluateEmotionalBehavior(actor, chosenTarget, UniversalConst.emotionalThreshold, thoughtMatrix[bestTargetIndex]);
        
        emotion = thoughtMatrix[bestTargetIndex];
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    } else {
        actor->memories.thoughtProcessProjectionMatrix.push_back(emotion);
    }
    
    // Flatten out the history matrix into the final active mood state
    ProjectEmotionalHistory(actor, emotion, sentientScore);
}

void ActorSystem::ProcessMemoryTriggers(Actor* actor, EmotionalEmbedding& emotion) {
    const std::vector<MemoryTrigger>& behaviorList = actor->memories.mMemoryTriggers["behavior"];
    if (behaviorList.empty()) return;
    
    for (const MemoryTrigger& trigger : behaviorList) {
        float chance = trigger.value;
        if (Random.Range(0.0f, 1.0f) >= glm::pow(chance, 0.7f)) continue;
        
        if (trigger.name == "curiosity") {
            if (emotion.curiosity > UniversalConst.emotionalThreshold) emotion.curiosity = 0.0f;
            emotion.curiosity += chance;
        } 
        else if (trigger.name == "libido") {
            if (actor->counters.mBreedingCoolDownCounter == 0 && actor->physical.mAge >= actor->physical.mAgeAdult) {
                emotion.libido += chance;
            }
        } 
        else if (trigger.name == "social") {
            if (actor->counters.mSocialCoolDownCounter == 0) {
                emotion.social += chance;
            }
        } 
        else if (trigger.name == "speak") {
            std::string voiceName = actor->memories.Get("speak_voice"); 
            Sound* voice = actor->voice.GetVoice(voiceName);
            if (voice != nullptr) {
                Playback* playback = Audio.Play(voice);
                playback->isGarbage = true;
            }
        }
        
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
                ApplyEmotionThresholds(trigger.name, trigger.value, outThoughtMatrix[i]);
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
                        ApplyEmotionThresholds(trigger.name, trigger.value, outThoughtMatrix[i]);
                    }
                }
            }
        }
        
        // Calculate intensity to track best target
        float intensity = glm::max(
            glm::max(outThoughtMatrix[i].anger, outThoughtMatrix[i].fear),
            glm::max(outThoughtMatrix[i].libido, outThoughtMatrix[i].curiosity)
        );
        
        if (intensity > highestIntensity) {
            highestIntensity = intensity;
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

void ActorSystem::ApplyEmotionThresholds(const std::string& emotionType, float value, EmotionalEmbedding& embedding) {
    float& emotion = *embedding.GetEmotionByName(emotionType);
    emotion = glm::max(emotion, value);
}



bool ActorSystem::EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& emotion) {
    // Handle socializing
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
        return false;
    }
    
    for (const std::pair<std::string, ActorState::Mode>& behavior : BehavioralAssociation) {
        const std::string& name = behavior.first;
        ActorState::Mode mode = behavior.second;
        
        float* emotionValPtr = emotion.GetEmotionByName(name);
        if (!emotionValPtr) continue;
        float value = *emotionValPtr;
        
        // General validation
        if (value <= threshold || 
            value <= emotion.anger || 
            value <= emotion.fear) 
            continue;
        
        // Contextual rule enforcement
        if (mode == ActorState::Mode::MoveAttack && (actor->state.mode == ActorState::Mode::MoveAttack || actor->counters.mAttackCoolDownCounter != 0)) continue;
        if (mode == ActorState::Mode::MoveBreed && actor->counters.mBreedingCoolDownCounter != 0) continue;
        if (mode == ActorState::Mode::MoveRandom && (actor->state.mode == ActorState::Mode::MoveAttack || actor->state.mode == ActorState::Mode::MoveFlee)) continue;
        
        // Execute state transition
        if (Random.Range(0.0f, 1.0f) < glm::pow(value, threshold)) {
            actor->state.mode = mode;
            
            if (mode == ActorState::Mode::MoveRandom) {
                CalculateRandomLocalPoint(actor);
                actor->navigation.mDistanceToTarget = glm::distance(
                    glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                    glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
                );
            } else if (mode == ActorState::Mode::MoveBreed) {
                if (!actor->mTargets.empty()) actor->navigation.mTargetActor = actor->mTargets[0];
            } else {
                actor->navigation.mTargetActor = targetActor;
                if (mode == ActorState::Mode::MoveAttack) {
                    actor->navigation.mDistanceToTarget = glm::distance(
                        glm::vec3(targetActor->navigation.mPosition.x, 0.0f, targetActor->navigation.mPosition.z),
                        glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z)
                    );
                }
            }
            return true;
        }
        break; 
    }
    
    return false;
}
