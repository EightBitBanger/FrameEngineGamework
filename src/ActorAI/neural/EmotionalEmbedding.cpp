#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/neural/EmotionalEmbedding.h>
#include <GameEngineFramework/Math/Random.h>

extern UniversalConstants UniversalConst;
extern NumberGeneration Random;

EmotionalEmbedding::EmotionalEmbedding() {
    Clear();
}

void EmotionalEmbedding::Reset() {
    Initiate();
}

float* EmotionalEmbedding::GetEmotionByName(std::string name) {
    if (name == "fear")      return &fear; else 
    if (name == "anger")     return &anger; else 
    if (name == "comfort")   return &comfort; else 
    if (name == "curiosity") return &curiosity; else 
    if (name == "fatigue")   return &fatigue; else 
    if (name == "stress")    return &stress; else 
    if (name == "libido")    return &libido; else 
    if (name == "social")    return &social;
    return nullptr;
}

void EmotionalEmbedding::Clear() {
    fear      = 0.0f;
    anger     = 0.0f;
    comfort   = 0.0f;
    curiosity = 0.0f;
    fatigue   = 0.0f;
    stress    = 0.0f;
    libido    = 0.0f;
    social    = 0.0f;
}

void EmotionalEmbedding::Randomize() {
    float min = UniversalConst.emotionalThreshold * 0.2f;
    float max = UniversalConst.emotionalThreshold * 0.8f;
    fear      = Random.Range(min, max);
    anger     = Random.Range(min, max);
    comfort   = Random.Range(min, max);
    curiosity = Random.Range(min, max);
    fatigue   = Random.Range(min, max);
    stress    = Random.Range(min, max);
    libido    = Random.Range(min, max);
    social    = Random.Range(min, max);
}

void EmotionalEmbedding::Initiate() {
    float min = UniversalConst.emotionalThreshold * 0.2f;
    float max = UniversalConst.emotionalThreshold * 0.8f;
    fear      = 0.0f;
    anger     = 0.0f;
    comfort   = Random.Range(min, max);
    curiosity = Random.Range(min, max);
    fatigue   = 0.0f;
    stress    = 0.0f;
    libido    = 0.0f;
    social    = Random.Range(min, max);
}

// Assignment: Copy from another EmotionalEmbedding
EmotionalEmbedding& EmotionalEmbedding::operator=(const EmotionalEmbedding& rhs) {
    fear      = rhs.fear;
    anger     = rhs.anger;
    comfort   = rhs.comfort;
    curiosity = rhs.curiosity;
    fatigue   = rhs.fatigue;
    stress    = rhs.stress;
    libido    = rhs.libido;
    social    = rhs.social;
    return *this;
}

// Assignment: Set all components to a scalar
EmotionalEmbedding& EmotionalEmbedding::operator=(float scalar) {
    fear      = scalar;
    anger     = scalar;
    comfort   = scalar;
    curiosity = scalar;
    fatigue   = scalar;
    stress    = scalar;
    libido    = scalar;
    social    = scalar;
    return *this;
}

// Compound Addition (Component-wise)
EmotionalEmbedding& EmotionalEmbedding::operator+=(const EmotionalEmbedding& rhs) {
    fear      += rhs.fear;
    anger     += rhs.anger;
    comfort   += rhs.comfort;
    curiosity += rhs.curiosity;
    fatigue   += rhs.fatigue;
    stress    += rhs.stress;
    libido    += rhs.libido;
    social    += rhs.social;
    return *this;
}

// Compound Multiplication (Component-wise)
EmotionalEmbedding& EmotionalEmbedding::operator*=(const EmotionalEmbedding& rhs) {
    fear      *= rhs.fear;
    anger     *= rhs.anger;
    comfort   *= rhs.comfort;
    curiosity *= rhs.curiosity;
    fatigue   *= rhs.fatigue;
    stress    *= rhs.stress;
    libido    *= rhs.libido;
    social    *= rhs.social;
    return *this;
}

// Compound Multiplication (Scalar)
EmotionalEmbedding& EmotionalEmbedding::operator*=(float scalar) {
    fear      *= scalar;
    anger     *= scalar;
    comfort   *= scalar;
    curiosity *= scalar;
    fatigue   *= scalar;
    stress    *= scalar;
    libido    *= scalar;
    social    *= scalar;
    return *this;
}

// Compound Division (Scalar)
EmotionalEmbedding& EmotionalEmbedding::operator/=(float scalar) {
    if (scalar != 0.0f) {
        fear      /= scalar;
        anger     /= scalar;
        comfort   /= scalar;
        curiosity /= scalar;
        fatigue   /= scalar;
        stress    /= scalar;
        libido    /= scalar;
        social    /= scalar;
    }
    return *this;
}

// Binary Multiplication (Scalar)
EmotionalEmbedding EmotionalEmbedding::operator*(float scalar) const {
    EmotionalEmbedding result;
    result.fear      = this->fear      * scalar;
    result.anger     = this->anger     * scalar;
    result.comfort   = this->comfort   * scalar;
    result.curiosity = this->curiosity * scalar;
    result.fatigue   = this->fatigue   * scalar;
    result.stress    = this->stress    * scalar;
    result.libido    = this->libido    * scalar;
    result.social    = this->social    * scalar;
    return result;
}

float* EmotionalEmbedding::GetEmotionByTrigger(const TriggerType& type) {
    switch (type) {
        case TriggerType::Fear:      return &fear;
        case TriggerType::Anger:     return &anger;
        case TriggerType::Comfort:   return &comfort;
        case TriggerType::Curiosity: return &curiosity;
        case TriggerType::Fatigue:   return &fatigue;
        case TriggerType::Stress:    return &stress;
        case TriggerType::Libido:    return &libido;
        case TriggerType::Social:    return &social;
        default:                     return nullptr;
    }
}
