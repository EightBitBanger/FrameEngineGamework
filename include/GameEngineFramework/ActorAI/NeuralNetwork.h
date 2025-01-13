#ifndef _NEURAL_NETWORK_SYSTEM__
#define _NEURAL_NETWORK_SYSTEM__

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>


#include <GameEngineFramework/configuration.h>

struct ENGINE_API TrainingSet {
    
    std::vector<float> input;
    
    std::vector<float> target;
    
};


struct ENGINE_API DataSet {
    
    std::vector<float> input;
    
};


struct ENGINE_API NeuralLayer {
    
    unsigned int numberOfInputs;
    unsigned int numberOfNeurons;
    
    std::vector<float> neurons;
    
    std::vector<std::vector<float>> weights;
    
    std::vector<float> biases;
    
};


class ENGINE_API NeuralNetwork {
    
public:
    
    /// Feed a dataset through the network.
    void FeedForward(const std::vector<float>& input);
    
    /// Get the output state of the network after a dataset
    /// has been fed through the network.
    std::vector<float> GetResults(void);
    
    /// Add a neural layer to the network. The number of inputs should match
    /// the previous layer`s neuron count.
    void AddNeuralLayer(int numberOfNeurons, int numberOfInputs);
    
    /// Clear all the layers, resetting the network.
    void ClearTopology(void);
    
    /// Modify the weights to fit a given training set of data.
    void Train(TrainingSet& trainingSet, float learningRate);
    
    /// Save the neural state of the network.
    std::vector<std::string> SaveState(void);
    
    /// Load a neural state into the network.
    void LoadState(std::vector<std::string>& state);
    
    NeuralNetwork();
    
private:
    
    // Neural layers
    std::vector<NeuralLayer> mTopology;
    
    // Calculate the error rate
    std::vector<std::vector<float>> CalculateDeltas(const std::vector<float>& target);
    
    // Update the weights by a given rate of learning
    void UpdateWeights(const std::vector<float>& input, const std::vector<std::vector<float>>& deltas, float learningRate);
    
    // Back propagation training activation function
    float ActivationFunctionDerivative(float value);
    
};

#endif
