#include <GameEngineFramework/ActorAI/neural/EmotionalEmbedding.h>

EmotionalEmbedding::EmotionalEmbedding() {
    Clear();
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
