#ifndef AI_ACTOR_VOCALS
#define AI_ACTOR_VOCALS

#include <GameEngineFramework/configuration.h>

class ENGINE_API VocalSynthesizer {
public:
    
    /// Set a voice by its associated name.
    void AddVoice(const std::string& name, Sound* sound);
    
    /// Get a voice by its associated name.
    Sound* GetVoice(const std::string& name);
    
    VocalSynthesizer();
    
private:
    
    std::unordered_map<std::string, Sound*> mVocals;
};

#endif
