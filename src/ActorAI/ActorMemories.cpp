#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

TriggerType StringToTriggerType(const std::string& str);
std::string TriggerTypeToString(TriggerType type);

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

void ActorSystem::ShareMemories(Actor* source, Actor* target) {
    if (source == nullptr || target == nullptr) 
        return;

    for (auto it = source->memories.mMemories.begin(); it != source->memories.mMemories.end(); ++it) {
        const std::string& key = it->first;
        const std::string& value = it->second;
        
        TriggerType type = StringToTriggerType(key);
        
        if (IsMemoryShareable(type) && !target->memories.CheckExists(key)) {
            target->memories.Add(key, value);
        }
    }
}

bool ActorSystem::HandleSocializeWith(Actor* actor, Actor* target) {
    if (actor == nullptr || target == nullptr)
        return false;
    
    if (actor->counters.mSocialCoolDownCounter > 0) 
        return false;
    
    float distance = glm::distance(actor->navigation.mPosition, target->navigation.mPosition);
    if (distance > actor->behavior.GetDistanceToInflict()) 
        return false;
    
    // Extract emotional values for the acting actor
    float comfort = actor->emotions.GetComfort();
    float anger = actor->emotions.current.anger;
    
    // Comfort increases success chance, while anger acts as a direct penalty
    float shareProbability = glm::clamp(comfort - anger, 0.0f, 1.0f);
    
    if (Random.Range(0.0f, 1.0f) < shareProbability) {
        ShareMemories(actor, target);
        ShareMemories(target, actor);
    }
    
    // Boost comfort
    actor->emotions.SetComfort(glm::min(actor->emotions.GetComfort() + 0.2f, 1.0f));
    target->emotions.SetComfort(glm::min(target->emotions.GetComfort() + 0.2f, 1.0f));
    
    // Zero out social & curiosity drives for both actors
    actor->emotions.current.curiosity  = 0.0f;
    target->emotions.current.curiosity = 0.0f;
    actor->emotions.current.social  = 0.0f;
    target->emotions.current.social = 0.0f;
    
    actor->memories.ScaleEmotion(TriggerType::Social, 0.1f);
    target->memories.ScaleEmotion(TriggerType::Social, 0.1f);
    
    actor->counters.mSocialCoolDownCounter  = actor->behavior.mCooldownObserve;
    target->counters.mSocialCoolDownCounter = actor->behavior.mCooldownObserve;
    
    actor->state.mode = ActorState::Mode::Idle;
    target->state.mode = ActorState::Mode::Idle;
    return true;
}

TriggerType StringToTriggerType(const std::string& str) {
    static const std::unordered_map<std::string, TriggerType> lookup = {
        {"curiosity",   TriggerType::Curiosity},
        {"libido",      TriggerType::Libido},
        {"social",      TriggerType::Social},
        {"speak",       TriggerType::Speak},
        {"anger",       TriggerType::Anger},
        {"fear",        TriggerType::Fear},
        {"comfort",     TriggerType::Comfort},
        {"fatigue",     TriggerType::Fatigue},
        {"stress",      TriggerType::Stress},
        {"home",        TriggerType::Home},
        {"sentience",   TriggerType::Sentience},
        {"speak_voice", TriggerType::SpeakVoice},
        {"behavior",    TriggerType::Behavior}
    };
    auto it = lookup.find(str);
    return (it != lookup.end()) ? it->second : TriggerType::Unknown;
}

std::string TriggerTypeToString(TriggerType type) {
    switch (type) {
        case TriggerType::Curiosity:  return "curiosity";
        case TriggerType::Libido:     return "libido";
        case TriggerType::Social:     return "social";
        case TriggerType::Speak:      return "speak";
        case TriggerType::Anger:      return "anger";
        case TriggerType::Fear:       return "fear";
        case TriggerType::Comfort:    return "comfort";
        case TriggerType::Fatigue:    return "fatigue";
        case TriggerType::Stress:     return "stress";
        case TriggerType::Home:       return "home";
        case TriggerType::Sentience:  return "sentience";
        case TriggerType::SpeakVoice: return "speak_voice";
        case TriggerType::Behavior:   return "behavior";
        default:                      return "unknown";
    }
}
