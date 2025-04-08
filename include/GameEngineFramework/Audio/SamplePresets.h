#ifndef AUDIO_RENDER_SAMPLE_PRESETS
#define AUDIO_RENDER_SAMPLE_PRESETS

#include <SDL3/SDL.h>

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Audio/components/sound.h>
#include <GameEngineFramework/Audio/components/samplebuffer.h>

//
// Audio rendering

class ENGINE_API AudioPreset {
    
public:
    
    void RenderBlankSpace(AudioSample* sample, float duration);
    
    void RenderWhiteNoise(AudioSample* sample, float duration);
    
    void RenderSquareWave(AudioSample* sample, float frequency, float duration);
    
    void RenderSweepingSineWave(AudioSample* sample, double startFreq, double endFreq, double duration);
    
};

#endif
