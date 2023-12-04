#ifndef _NEURAL_NETWORKING__
#define _NEURAL_NETWORKING__

#include <GameEngineFramework/configuration.h>

struct ENGINE_API Neuron {
    
    float in   [3];
    float layerA[4];
    float layerB[4];
    float out  [2];
    
    Neuron();
    
};

#endif
