#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Types/Types.h>

extern StringType String;
extern EngineSystemManager Engine;
extern UniversalConstants UniversalConst;

const std::string memory_blank = "";

TriggerType StringToTriggerType(const std::string& str);
std::string TriggerTypeToString(TriggerType type);

MemorySystem::MemorySystem() : 
    mDoUpdateMemories(false)
{}

void MemorySystem::Reset() {
    mDoUpdateMemories = false;
    
    mMemories.clear();
    mMemoryTriggers.clear();
    thoughtProcessProjectionMatrix.clear();
}

void MemorySystem::Add(const std::string& name, const std::string& memory) {
    mMemories[name] = memory;
    mDoUpdateMemories = true;
}

void MemorySystem::Remove(const std::string& name) {
    mMemories.erase(name);
    mDoUpdateMemories = true;
}

const std::string& MemorySystem::Get(const std::string& name) {
    if (mMemories.find(name) != mMemories.end()) 
        return mMemories[name];
    return memory_blank;
}

std::string MemorySystem::GetMemoryNameByIndex(unsigned int index) {
    unsigned int counter;
    for (std::unordered_map<std::string, std::string>::iterator it = mMemories.begin(); it != mMemories.end(); ++it) {
        if (counter == index) 
            return it->first;
        counter++;
    }
    return memory_blank;
}

std::string MemorySystem::GetMemoryValueByIndex(unsigned int index) {
    unsigned int counter;
    for (std::unordered_map<std::string, std::string>::iterator it = mMemories.begin(); it != mMemories.end(); ++it) {
        if (counter == index) 
            return it->second;
        counter++;
    }
    return "";
}

unsigned int MemorySystem::GetNumberOfMemories(void) {
    return mMemories.size();
}

void MemorySystem::Clear(void) {
    mMemories.clear();
    mMemoryTriggers.clear();
    thoughtProcessProjectionMatrix.clear();
}

bool MemorySystem::CheckExists(const std::string& name) {
    std::unordered_map<std::string, std::string>::iterator it = mMemories.find(name);
    if (it != mMemories.end()) 
        return true;
    return false;
}

void MemorySystem::ClearEmotion(const std::string& emotionName) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByName(emotionName);
        if (valPtr != nullptr) {
            *valPtr = 0.0f;
        }
    }
}

void MemorySystem::SetEmotion(const std::string& emotionName, float value) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByName(emotionName);
        if (valPtr != nullptr) {
            *valPtr = value;
        }
    }
}

void MemorySystem::ScaleEmotion(const std::string& emotionName, float factor) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByName(emotionName);
        if (valPtr != nullptr) {
            *valPtr *= factor;
        }
    }
}

void MemorySystem::ClearEmotion(TriggerType type) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByTrigger(type);
        if (valPtr != nullptr) {
            *valPtr = 0.0f;
        }
    }
}

void MemorySystem::SetEmotion(TriggerType type, float value) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByTrigger(type);
        if (valPtr != nullptr) {
            *valPtr = value;
        }
    }
}

void MemorySystem::ScaleEmotion(TriggerType type, float factor) {
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* valPtr = embedding.GetEmotionByTrigger(type);
        if (valPtr != nullptr) {
            *valPtr *= factor;
        }
    }
}

void MemorySystem::DegradeEmotionalState(float amount) {
    if (amount <= 0.0f) {
        return;
    }
    
    for (EmotionalEmbedding& embedding : thoughtProcessProjectionMatrix) {
        float* emotions[] = {
            &embedding.fear,
            &embedding.anger,
            &embedding.comfort,
            &embedding.curiosity,
            &embedding.fatigue,
            &embedding.stress,
            &embedding.libido,
            &embedding.social
        };
        
        for (float* emotion : emotions) {
            // Apply exponential decay over time if emotion is over the threshold
            if (*emotion > UniversalConst.emotionalThreshold) 
                *emotion *= amount;
            
            if (*emotion < 0.001f) 
                *emotion = 0.0f;
        }
    }
}

void MemorySystem::DegradeEmotionalState(const EmotionalEmbedding& embedding, float threshold) {
    for (EmotionalEmbedding& target : thoughtProcessProjectionMatrix) {
        // Map component pointers for target and values for source decay
        float* targetEmotions[] = {
            &target.fear,
            &target.anger,
            &target.comfort,
            &target.curiosity,
            &target.fatigue,
            &target.stress,
            &target.libido,
            &target.social
        };
        
        const float decayFactors[] = {
            embedding.fear,
            embedding.anger,
            embedding.comfort,
            embedding.curiosity,
            embedding.fatigue,
            embedding.stress,
            embedding.libido,
            embedding.social
        };
        
        // Apply component-wise decay based on thresholds
        for (size_t i = 0; i < 8; ++i) {
            if (*targetEmotions[i] > threshold) {
                *targetEmotions[i] *= decayFactors[i];
            }
            
            if (*targetEmotions[i] < 0.001f) {
                *targetEmotions[i] = 0.0f;
            }
        }
    }
}

void MemorySystem::UpdateMemories(void) {
    if (!mDoUpdateMemories) 
        return;
    mDoUpdateMemories = false;
    
    mMemoryTriggers.clear();
    for (auto it = mMemories.begin(); it != mMemories.end(); ++it) {
        std::vector<MemoryTrigger> triggers;
        // Split individual memory triggers using '^'
        std::vector<std::string> values = String.Explode(it->second, '^'); 
        
        for (unsigned int i = 0; i < values.size(); i++) {
            std::string& value = values[i];
            String.RemoveWhiteSpace(value);
            
            std::vector<std::string> kvPair = String.Explode(value, ':');
            if (kvPair.size() != 2) 
                continue;
            
            MemoryTrigger trigger;
            trigger.name = kvPair[0];
            trigger.type = StringToTriggerType(trigger.name);
            
            // Check if payload contains a comma-separated 3D vector
            std::vector<std::string> vecSplit = String.Explode(kvPair[1], ',');
            if (vecSplit.size() >= 3) {
                trigger.vector.x = String.ToFloat(vecSplit[0]);
                trigger.vector.y = String.ToFloat(vecSplit[1]);
                trigger.vector.z = String.ToFloat(vecSplit[2]);
                trigger.value    = trigger.vector.x;
            } else {
                trigger.value    = String.ToFloat(kvPair[1]);
                trigger.vector   = glm::vec3(trigger.value, 0.0f, 0.0f);
            }
            
            triggers.push_back(trigger);
        }
        
        mMemoryTriggers[it->first] = triggers;
    }
}
