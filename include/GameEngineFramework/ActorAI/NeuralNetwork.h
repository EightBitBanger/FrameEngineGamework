#ifndef _NEURAL_NETWORK_SYSTEM__
#define _NEURAL_NETWORK_SYSTEM__

#include <vector>
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
    
    std::vector<float> neurons;
    
    std::vector<std::vector<float>> weights;
    
    std::vector<float> biases;
    
};


class ENGINE_API NeuralNetwork {
    
public:
    
    void FeedForward(const std::vector<float>& input);
    
    std::vector<float> GetResults(void);
    
    void AddNeuralLayer(int numNeurons, int numInputs);
    
    void ClearTopology(void);
    
    void Train(TrainingSet& trainingSet, float learningRate);
    
    NeuralNetwork();
    
private:
    
    std::vector<NeuralLayer> mTopology;
    
    std::vector<std::vector<float>> CalculateDeltas(const std::vector<float>& target);
    
    void UpdateWeights(const std::vector<float>& input, const std::vector<std::vector<float>>& deltas, float learningRate);
    
    float ActivationFunctionDerivative(float value);
    
};

#endif
