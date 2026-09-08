#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

const std::string memory_blank = "";
const std::vector<std::string> emotStringLookup = {
    "curiosity",
    "libido",
    "social",
    "speak",
    "anger",
    "fear",
    "comfort",
    "fatigue",
    "stress",
    "home",
    "sentience",
    "speak_voice",
    "behavior"
};

TriggerType StringToTriggerType(const std::string& str);
const std::string& TriggerTypeToString(TriggerType type);

bool ActorSystem::IsMemoryShareable(TriggerType type) {
    switch (type) {
        case TriggerType::Home:
        case TriggerType::Sentience:
        case TriggerType::SpeakVoice:
        case TriggerType::Behavior:
            return false;
        default:
            return true;
    }
}

thread_local std::vector<std::pair<std::string, std::string>> candidates;

void ActorSystem::ShareMemories(Actor* source, Actor* target) {
    if (source == nullptr || target == nullptr) 
        return;
    
    // Collect all shareable memories not already known by target
    candidates.clear();
    for (std::unordered_map<std::string, std::string>::iterator it = source->memories.mMemories.begin(); it != source->memories.mMemories.end(); ++it) {
        const std::string& key = it->first;
        
        // Prevent memories from spreading through conversation
        if (key == "trade" || 
            key == "name" || 
            key == "sentience" || 
            key == "family") {
            continue;
        }
        
        TriggerType type = StringToTriggerType(key);
        
        if (IsMemoryShareable(type) && !target->memories.CheckExists(key)) {
            candidates.push_back(*it);
        }
    }
    
    if (candidates.empty()) 
        return;
    
    // Shuffle candidates
    for (size_t i = candidates.size() - 1; i > 0; --i) {
        size_t j = static_cast<size_t>(Random.Range(0.0f, 0.999f) * (i + 1));
        std::swap(candidates[i], candidates[j]);
    }
    
    // Determine how many memories to share
    size_t countToShare = 1;
    if (candidates.size() > 1) {
        float roll = Random.Range(0.0f, 1.0f);
        if (roll > 0.66f && candidates.size() >= 3) {
            countToShare = 3;
        } else if (roll > 0.33f) {
            countToShare = 2;
        }
    }
    
    // Transfer the chosen candidates
    for (size_t i = 0; i < countToShare && i < candidates.size(); ++i) {
        target->memories.Add(candidates[i].first, candidates[i].second);
    }
}

bool ActorSystem::HandleSocializeWith(Actor* actor, Actor* target) {
    if (actor == nullptr || target == nullptr)
        return false;
    
    if (actor->counters.mSocialCoolDownCounter > 0) 
        return false;
    
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    float socialDistance = actor->behavior.GetDistanceToInflict() * 1.5f;
    if (distance > socialDistance) 
        return false;
    
    // Sentience differential comparison
    float sentienceA = 0.0f;
    float sentienceB = 0.0f;
    
    std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator itA = actor->memories.mMemoryTriggers.find("sentience");
    if (itA != actor->memories.mMemoryTriggers.end() && !itA->second.empty()) {
        sentienceA = itA->second[0].value;
    }
    
    std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator itB = target->memories.mMemoryTriggers.find("sentience");
    if (itB != target->memories.mMemoryTriggers.end() && !itB->second.empty()) {
        sentienceB = itB->second[0].value;
    }
    
    const float maxSentienceDelta = 0.1f; 
    
    if (std::abs(sentienceA - sentienceB) <= maxSentienceDelta) {
        float comfort = actor->emotions.GetComfort();
        float anger = actor->emotions.current.anger;
        
        float shareProbability = glm::clamp(comfort - anger, 0.0f, 1.0f);
        if (Random.Range(0.0f, 1.0f) < shareProbability) {
            ShareMemories(actor, target);
            ShareMemories(target, actor);
        }
    }
    
    // =========================================================================
    // INVENTORY / FOOD SHARING LOGIC
    // =========================================================================
    auto TransferFoodSurplus = [](Actor* giver, Actor* receiver) {
        const std::string& giverTrade = giver->memories.Get("trade");
        bool isFarmer = (!giverTrade.empty() && giverTrade.rfind("farmer:", 0) == 0);
        
        // Farmers reserve at least 2 crops as seed stock; others reserve at least 1 food item
        size_t reserveCount = isFarmer ? 2 : 1;
        
        std::vector<unsigned int> foodIndices;
        for (unsigned int i = 0; i < giver->inventory.itemClassList.size(); ++i) {
            const std::string& itemStr = giver->inventory.itemClassList[i];
            if (itemStr.find("crop") != std::string::npos || itemStr.find("saturation") != std::string::npos) {
                foodIndices.push_back(i);
            }
        }
        
        if (foodIndices.size() <= reserveCount) {
            return;
        }
        
        // Transfer up to half of surplus food
        size_t surplus = foodIndices.size() - reserveCount;
        size_t itemsToGive = std::max<size_t>(1, surplus / 2);
        
        for (size_t k = 0; k < itemsToGive && !foodIndices.empty(); ++k) {
            unsigned int itemIndex = foodIndices.back();
            foodIndices.pop_back();
        
            std::string itemClassification = giver->inventory.itemClassList[itemIndex];
            giver->inventory.RemoveItem(itemIndex);
            receiver->inventory.AddItem(itemClassification);
        }
    };
    
    size_t foodCountA = 0;
    for (const std::string& itemStr : actor->inventory.itemClassList) {
        if (itemStr.find("crop") != std::string::npos || itemStr.find("saturation") != std::string::npos) {
            foodCountA++;
        }
    }
    
    size_t foodCountB = 0;
    for (const std::string& itemStr : target->inventory.itemClassList) {
        if (itemStr.find("crop") != std::string::npos || itemStr.find("saturation") != std::string::npos) {
            foodCountB++;
        }
    }
    
    if (foodCountA > foodCountB + 1) {
        TransferFoodSurplus(actor, target);
    } else if (foodCountB > foodCountA + 1) {
        TransferFoodSurplus(target, actor);
    }
    
    // Boost comfort
    actor->emotions.SetComfort(glm::min(actor->emotions.GetComfort() + 0.2f, 1.0f));
    target->emotions.SetComfort(glm::min(target->emotions.GetComfort() + 0.2f, 1.0f));
    
    // Zero out social & curiosity drives for both actors
    actor->emotions.current.curiosity  = 0.0f;
    target->emotions.current.curiosity = 0.0f;
    actor->emotions.current.social     = 0.0f;
    target->emotions.current.social    = 0.0f;
    
    // Drop social
    actor->memories.ScaleEmotion(TriggerType::Social, 0.1f);
    target->memories.ScaleEmotion(TriggerType::Social, 0.1f);
    
    // Bump up curiosity
    actor->memories.ScaleEmotion(TriggerType::Curiosity, 1.3f);
    target->memories.ScaleEmotion(TriggerType::Curiosity, 1.3f);
    
    actor->counters.mSocialCoolDownCounter  = actor->behavior.mCooldownSocial;
    target->counters.mSocialCoolDownCounter = actor->behavior.mCooldownSocial;
    
    actor->state.mode = ActorState::Mode::Idle;
    target->state.mode = ActorState::Mode::Idle;
    return true;
}

TriggerType StringToTriggerType(const std::string& str) {
    static const std::unordered_map<std::string, TriggerType> lookup = {
        {emotStringLookup[0],   TriggerType::Curiosity},
        {emotStringLookup[1],   TriggerType::Libido},
        {emotStringLookup[2],   TriggerType::Social},
        {emotStringLookup[3],   TriggerType::Speak},
        {emotStringLookup[4],   TriggerType::Anger},
        {emotStringLookup[5],   TriggerType::Fear},
        {emotStringLookup[6],   TriggerType::Comfort},
        {emotStringLookup[7],   TriggerType::Fatigue},
        {emotStringLookup[8],   TriggerType::Stress},
        {emotStringLookup[9],   TriggerType::Home},
        {emotStringLookup[10],  TriggerType::Sentience},
        {emotStringLookup[11],  TriggerType::SpeakVoice},
        {emotStringLookup[12],  TriggerType::Behavior}
    };
    auto it = lookup.find(str);
    return (it != lookup.end()) ? it->second : TriggerType::Unknown;
}

const std::string& TriggerTypeToString(TriggerType type) {
    switch (type) {
        case TriggerType::Curiosity:  return emotStringLookup[0];
        case TriggerType::Libido:     return emotStringLookup[1];
        case TriggerType::Social:     return emotStringLookup[2];
        case TriggerType::Speak:      return emotStringLookup[3];
        case TriggerType::Anger:      return emotStringLookup[4];
        case TriggerType::Fear:       return emotStringLookup[5];
        case TriggerType::Comfort:    return emotStringLookup[6];
        case TriggerType::Fatigue:    return emotStringLookup[7];
        case TriggerType::Stress:     return emotStringLookup[8];
        case TriggerType::Home:       return emotStringLookup[9];
        case TriggerType::Sentience:  return emotStringLookup[10];
        case TriggerType::SpeakVoice: return emotStringLookup[11];
        case TriggerType::Behavior:   return emotStringLookup[12];
        default:                      return memory_blank;
    }
}
