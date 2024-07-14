#ifndef _NEURAL_NETWORKING__
#define _NEURAL_NETWORKING__

#include <GameEngineFramework/configuration.h>


class ENGINE_API NeuralLayer {
    
public:
    
    /// Unweighted layer node values.
    float node[NEURAL_LAYER_WIDTH];
    
    NeuralLayer();
    
};

#endif
