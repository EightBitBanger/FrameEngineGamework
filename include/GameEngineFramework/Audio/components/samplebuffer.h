#ifndef AUDIO_SAMPLE_COMPONENT
#define AUDIO_SAMPLE_COMPONENT

#include <GameEngineFramework/configuration.h>
#include <vector>
#include <stdint.h>

class ENGINE_API AudioSample {
    
public:
    
    AudioSample();
    
    /// Rate at which the data should be sampled.
    unsigned int sample_rate;
    
    /// Array of audio sample data.
    std::vector<int32_t> sampleBuffer;
    
};

#endif
