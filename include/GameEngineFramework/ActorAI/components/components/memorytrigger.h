#ifndef AI_ACTOR_MEMORY_TRIGGER
#define AI_ACTOR_MEMORY_TRIGGER

#include <GameEngineFramework/ActorAI/ActorStates.h>
#include <GameEngineFramework/configuration.h>
#include <string>

enum class TriggerType {
    Unknown = 0,
    
    // Emotional / Drive Triggers
    Curiosity,
    Libido,
    Social,
    Speak,
    Anger,
    Fear,
    Comfort,
    Fatigue,
    Stress,
    
    // Internal System & Identity Keys
    Home,
    Sentience,
    SpeakVoice,
    Behavior
};

class ENGINE_API MemoryTrigger {
public:
    
    TriggerType type = TriggerType::Unknown;
    
    std::string name;
    
    float value = 0.0f;
    glm::vec3 vector = glm::vec3(0.0f);
};

#endif
