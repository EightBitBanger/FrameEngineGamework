#ifndef _EMOTIONAL_EMBEDDING__
#define _EMOTIONAL_EMBEDDING__

#include <GameEngineFramework/ActorAI/components/components/memorytrigger.h>
#include <GameEngineFramework/configuration.h>
#include <string>

class ENGINE_API EmotionalEmbedding {
public:
    
    float fear;
    float anger;
    float comfort;
    float curiosity;
    float fatigue;
    float stress;
    float libido;
    float social;
    
    EmotionalEmbedding();
    
    void Clear();
    
    float* GetEmotionByName(std::string name);
    
    float* GetEmotionByTrigger(TriggerType type);
    
    EmotionalEmbedding& operator+=(const EmotionalEmbedding& rhs);
    EmotionalEmbedding& operator/=(float scalar);
    EmotionalEmbedding operator*(float scalar) const;
};

#endif
