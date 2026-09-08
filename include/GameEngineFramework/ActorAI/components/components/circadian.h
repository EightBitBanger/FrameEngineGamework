#ifndef AI_ACTOR_CIRCADIAN
#define AI_ACTOR_CIRCADIAN

#include <GameEngineFramework/configuration.h>

class ENGINE_API Circadian {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    static constexpr float kDefaultDayLength = 24000.0f;
    
    Circadian();
    void Reset();
    
    bool IsSleepPeriod(float timeOfDay, float dayLength = kDefaultDayLength) const;
    void ShiftRhythm(float deltaTicks, float dayLength = kDefaultDayLength);
    void Entrain(float factor = 1.0f, float dayLength = kDefaultDayLength);
    
    void SetPreferredSleepStart(float time);
    float GetPreferredSleepStart() const;
    
    void SetPreferredDuration(float duration);
    float GetPreferredDuration() const;
    
    void SetCurrentSleepStart(float time);
    float GetCurrentSleepStart() const;
    
    void SetCurrentDuration(float duration);
    float GetCurrentDuration() const;
    
    void SetEntrainmentRate(float rate);
    float GetEntrainmentRate() const;
    
private:
    float mPreferredSleepStart;    // Circadian preference
    float mPreferredDuration;
    
    float mCurrentSleepStart;      // Current circadian state
    float mCurrentDuration;
    
    float mEntrainmentRate;
};

#endif
