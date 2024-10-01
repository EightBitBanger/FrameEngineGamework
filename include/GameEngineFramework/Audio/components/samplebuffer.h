#ifndef AUDIO_SAMPLE_COMPONENT
#define AUDIO_SAMPLE_COMPONENT

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include "../../../../vendor/AL/al.h"
#include "../../../../vendor/AL/alc.h"

#include <vector>

extern NumberGeneration Random;


class ENGINE_API AudioSample {
    
public:
    
    unsigned int sample_rate;
    
    std::vector<ALshort> sampleBuffer;
    
};


//
// Audio rendering

class ENGINE_API AudioPreset {
    
public:
    
    void renderBlankSpace(AudioSample* sample, float duration);
    
    void renderWhiteNoise(AudioSample* sample, float duration);
    
    void renderSquareWave(AudioSample* sample, float frequency, float duration);
    
    void renderSweepingSineWave(AudioSample* sample, double startFreq, double endFreq, double duration);
    
};

#endif
