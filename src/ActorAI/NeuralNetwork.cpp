#include <vector>
#include <cmath>
#include <algorithm>

#include <GameEngineFramework/ActorAI/NeuralNetwork.h>
#include <cstdlib>


void NeuralNetwork::FeedForward(const std::vector<float>& input) {
    
    return;
}

std::vector<float> NeuralNetwork::GetResults(void) {
    
    unsigned int lastIndex = mTopology.size() - 1;
    
    return mTopology[lastIndex].layer;
}

void NeuralNetwork::AddNeuralLayer(const std::vector<float>& layer) {
    
    NeuralLayer neuralLayer;
    
    for (unsigned int i=0; i < layer.size(); i++) 
        neuralLayer.layer.push_back( layer[i] );
    
    mTopology.push_back( neuralLayer );
    
    return;
}

void NeuralNetwork::ClearTopology(void) {
    
    mTopology.clear();
    
    return;
}


NeuralNetwork::NeuralNetwork() 
    
{
}

