#include <GameEngineFramework/ActorAI/ActorSystem.h>

VocalSynthesizer::VocalSynthesizer() {};

void VocalSynthesizer::AddVoice(const std::string& name, Sound* sound) {
    if (name == "") 
        return;
    mVocals[name] = sound;
}

Sound* VocalSynthesizer::GetVoice(const std::string& name) {
    return mVocals[name];
}
