#ifndef AUDIO_SAMPLE_COMPONENT
#define AUDIO_SAMPLE_COMPONENT

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include <vector>

extern NumberGeneration Random;


class ENGINE_API AudioSample {
    
public:
    
    unsigned int sample_rate;
    
    std::vector<int16_t> sampleBuffer;
    
};


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
