#include <GameEngineFramework/ActorAI/ActorSystem.h>

EmotionalState::EmotionalState() {}

void EmotionalState::Reset() {
    current.Initiate();
}

void EmotionalState::SetFear(float fear) {
    current.fear = fear;
}

float EmotionalState::GetFear(void) {
    return current.fear;
}

void EmotionalState::AddFear(float additive) {
    current.fear += additive;
}


void EmotionalState::SetAnger(float anger) {
    current.anger = anger;
}

float EmotionalState::GetAnger(void) {
    return current.anger;
}

void EmotionalState::AddAnger(float additive) {
    current.anger += additive;
}


void EmotionalState::SetFatigue(float fatigue) {
    current.fatigue = fatigue;
}

float EmotionalState::GetFatigue(void) {
    return current.fatigue;
}

void EmotionalState::AddFatigue(float additive) {
    current.fatigue += additive;
}


void EmotionalState::SetStress(float stress) {
    current.stress = stress;
}

float EmotionalState::GetStress(void) {
    return current.stress;
}

void EmotionalState::AddStress(float additive) {
    current.stress += additive;
}


void EmotionalState::SetCuriosity(float curiosity) {
    current.curiosity = curiosity;
}

float EmotionalState::GetCuriosity(void) {
    return current.curiosity;
}

void EmotionalState::AddCuriosity(float additive) {
    current.curiosity += additive;
}


void EmotionalState::SetComfort(float comfort) {
    current.comfort = comfort;
}

float EmotionalState::GetComfort(void) {
    return current.comfort;
}

void EmotionalState::AddComfort(float additive) {
    current.comfort += additive;
}


void EmotionalState::SetLibido(float libido) {
    current.libido = libido;
}

float EmotionalState::GetLibido(void) {
    return current.libido;
}

void EmotionalState::AddLibido(float additive) {
    current.libido += additive;
}
