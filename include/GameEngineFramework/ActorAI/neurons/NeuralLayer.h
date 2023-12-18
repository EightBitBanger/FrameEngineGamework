#ifndef _NEURAL_NETWORKING__
#define _NEURAL_NETWORKING__

#include <GameEngineFramework/configuration.h>

#define NEURAL_LAYER_WIDTH  3

class ENGINE_API NeuralLayer {
    
public:
    
    /// Unweighted layer node values.
    float node[NEURAL_LAYER_WIDTH];
    
    NeuralLayer();
    
};

#endif
