#ifndef _WEIGHTED_NEURAL_NETWORKING__
#define _WEIGHTED_NEURAL_NETWORKING__

#include <GameEngineFramework/configuration.h>

#define NEURAL_LAYER_WIDTH  3

class ENGINE_API WeightedLayer {
    
public:
    
    /// Amount to which the neural layer will adapt its weights to the data.
    float plasticity;
    
    /// Layer state values.
    float node[NEURAL_LAYER_WIDTH];
    
    /// Layer weight values.
    float weight[NEURAL_LAYER_WIDTH];
    
    WeightedLayer();
    
};

#endif
