#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Types/Types.h>

extern StringType String;
extern EngineSystemManager Engine;

TriggerType StringToTriggerType(const std::string& str);
std::string TriggerTypeToString(TriggerType type);

MemorySystem::MemorySystem() : 
    mDoUpdateMemories(false)
{}

void MemorySystem::Add(const std::string& name, const std::string& memory) {
    mMemories[name] = memory;
    mDoUpdateMemories = true;
}

void MemorySystem::Remove(const std::string& name) {
    mMemories.erase(name);
    mDoUpdateMemories = true;
}

std::string MemorySystem::Get(const std::string& name) {
    if (mMemories.find(name) != mMemories.end()) 
        return mMemories[name];
    return "";
}

std::string MemorySystem::GetMemoryNameByIndex(unsigned int index) {
    unsigned int counter;
    for (std::unordered_map<std::string, std::string>::iterator it = mMemories.begin(); it != mMemories.end(); ++it) {
        if (counter == index) 
            return it->first;
        counter++;
    }
    return "";
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

void MemorySystem::UpdateMemories(void) {
    std::unordered_map<std::string, std::string>::iterator begin = mMemories.begin();
    std::unordered_map<std::string, std::string>::iterator end   = mMemories.end();
    mMemoryTriggers.clear();
    for (std::unordered_map<std::string, std::string>::iterator it = begin; it != end; it++) {
        std::pair<std::string, std::string> memory = *it;
        std::vector<MemoryTrigger> triggers;
        std::vector<std::string> values = String.Explode(memory.second, ',');
        for (unsigned int i=0; i < values.size(); i++) {
            std::string& value = values[i];
            String.RemoveWhiteSpace(value);
            
            std::vector<std::string> kvPair = String.Explode(value, ':');
            if (kvPair.size() != 2) 
                continue;
            MemoryTrigger trigger;
            trigger.name  = kvPair[0];
            trigger.type  = StringToTriggerType(trigger.name);
            trigger.value = String.ToFloat(kvPair[1]);
            
            triggers.push_back(trigger);
        }
        
        mMemoryTriggers[memory.first] = triggers;
    }
}
