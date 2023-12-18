#include <GameEngineFramework/ActorAI/neurons/NeuralLayer.h>


NeuralLayer::NeuralLayer() {
    
    for (int i=0; i < NEURAL_LAYER_WIDTH; i++) node[i] = 0;
    
    return;
}
