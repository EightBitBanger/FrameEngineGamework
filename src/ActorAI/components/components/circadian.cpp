#include <GameEngineFramework/ActorAI/components/components/circadian.h>
#include <cmath>

static inline float WrapCycle(float value, float maxVal) {
    value = std::fmod(value, maxVal);
    if (value < 0.0f) value += maxVal;
    return value;
}

static inline float ShortestCycleDelta(float from, float to, float maxVal) {
    float half = maxVal * 0.5f;
    float d = WrapCycle(to, maxVal) - WrapCycle(from, maxVal);
    if (d > half)  d -= maxVal;
    if (d < -half) d += maxVal;
    return d;
}

Circadian::Circadian() {
    Reset();
}

void Circadian::Reset(void) {
    mPreferredSleepStart = 22000.0f;
    mPreferredDuration   = 8000.0f;
    
    mCurrentSleepStart   = 22000.0f;
    mCurrentDuration     = 8000.0f;
    
    mEntrainmentRate     = 25.0f; // Ticks pulled back toward baseline per entrain step
}

bool Circadian::IsSleepPeriod(float timeOfDay, float dayLength) const {
    float current = WrapCycle(timeOfDay, dayLength);
    float start   = WrapCycle(mCurrentSleepStart, dayLength);
    float end     = WrapCycle(mCurrentSleepStart + mCurrentDuration, dayLength);
    
    if (start <= end) {
        return (current >= start && current < end);
    }
    // Window wraps over the day boundary (e.g. 22000 -> 6000)
    return (current >= start || current < end);
}

void Circadian::ShiftRhythm(float deltaTicks, float dayLength) {
    mCurrentSleepStart = WrapCycle(mCurrentSleepStart + deltaTicks, dayLength);
}

void Circadian::Entrain(float factor, float dayLength) {
    float delta = ShortestCycleDelta(mCurrentSleepStart, mPreferredSleepStart, dayLength);
    float step = mEntrainmentRate * factor;
    
    if (std::abs(delta) <= step) {
        mCurrentSleepStart = mPreferredSleepStart;
    } else {
        mCurrentSleepStart = WrapCycle(mCurrentSleepStart + std::copysign(step, delta), dayLength);
    }
}

void Circadian::SetPreferredSleepStart(float time) { mPreferredSleepStart = time; }
float Circadian::GetPreferredSleepStart() const { return mPreferredSleepStart; }

void Circadian::SetPreferredDuration(float duration) { mPreferredDuration = duration; }
float Circadian::GetPreferredDuration() const { return mPreferredDuration; }

void Circadian::SetCurrentSleepStart(float time) { mCurrentSleepStart = time; }
float Circadian::GetCurrentSleepStart() const { return mCurrentSleepStart; }

void Circadian::SetCurrentDuration(float duration) { mCurrentDuration = duration; }
float Circadian::GetCurrentDuration() const { return mCurrentDuration; }

void Circadian::SetEntrainmentRate(float rate) { mEntrainmentRate = rate; }
float Circadian::GetEntrainmentRate() const { return mEntrainmentRate; }
