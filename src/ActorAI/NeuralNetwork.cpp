#include <vector>
#include <cmath>
#include <algorithm>
#include <GameEngineFramework/ActorAI/NeuralNetwork.h>
#include <cstdlib>

NeuralNetwork::NeuralNetwork() {}

void NeuralNetwork::FeedForward(const std::vector<float>& input) {
    if (mTopology.empty()) return;
    
    // Set input layer
    mTopology[0].neurons = input;
    
    // Forward propagate
    for (size_t i = 1; i < mTopology.size(); ++i) {
        
        NeuralLayer& previousLayer = mTopology[i - 1];
        NeuralLayer& currentLayer = mTopology[i];
        
        for (size_t j = 0; j < currentLayer.neurons.size(); ++j) {
            
            float sum = currentLayer.biases[j];
            
            for (size_t k = 0; k < previousLayer.neurons.size(); ++k) {
                sum += previousLayer.neurons[k] * currentLayer.weights[j][k];
            }
            
            currentLayer.neurons[j] = tanh(sum);
        }
    }
}

std::vector<float> NeuralNetwork::GetResults(void) {
    if (mTopology.empty()) return {};
    return mTopology.back().neurons;
}

void NeuralNetwork::AddNeuralLayer(int numNeurons, int numInputs) {
    NeuralLayer layer;
    
    layer.neurons.resize(numNeurons);
    layer.biases.resize(numNeurons);
    layer.weights.resize(numNeurons, std::vector<float>(numInputs));
    
    // Initialize weights and biases
    for (int i = 0; i < numNeurons; ++i) {
        layer.biases[i] = static_cast<float>(rand()) / RAND_MAX;
        for (int j = 0; j < numInputs; ++j) {
            layer.weights[i][j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
    
    mTopology.push_back(layer);
}

void NeuralNetwork::ClearTopology(void) {
    mTopology.clear();
}
