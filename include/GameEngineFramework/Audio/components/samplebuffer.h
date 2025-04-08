#ifndef AUDIO_SAMPLE_COMPONENT
#define AUDIO_SAMPLE_COMPONENT

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include <vector>

extern NumberGeneration Random;


class ENGINE_API AudioSample {
    
public:
    
    AudioSample();
    
    unsigned int sample_rate;
    
    std::vector<int32_t> sampleBuffer;
    
};

#endif
