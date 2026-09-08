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
    
    // Reset the emotional state.
    void Reset();
    
    // Set all emotions to zero.
    void Clear();
    
    // Set all emotions to random values.
    void Randomize();
    
    // Randomize appropriate emotions and zero the rest.
    void Initiate();
    
    float* GetEmotionByName(std::string name);
    float* GetEmotionByTrigger(const TriggerType& type);
    
    EmotionalEmbedding& operator=(const EmotionalEmbedding& rhs);
    EmotionalEmbedding& operator=(float scalar);
    
    EmotionalEmbedding& operator+=(const EmotionalEmbedding& rhs);
    EmotionalEmbedding& operator*=(const EmotionalEmbedding& rhs);
    EmotionalEmbedding& operator*=(float scalar);
    EmotionalEmbedding& operator/=(float scalar);
    
    EmotionalEmbedding operator*(float scalar) const;
};

#endif
