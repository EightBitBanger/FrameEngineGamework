#ifndef _NEURAL_NETWORK_SYSTEM__
#define _NEURAL_NETWORK_SYSTEM__

#include <cstdlib>
#include <vector>

struct NeuralLayer {
    
    std::vector<float> neurons;
    
    std::vector<std::vector<float>> weights;
    
    std::vector<float> biases;
    
};


class NeuralNetwork {
    
public:
    
    void FeedForward(const std::vector<float>& input);
    
    std::vector<float> GetResults(void);
    
    void AddNeuralLayer(int numNeurons, int numInputs);
    
    void ClearTopology(void);
    
    NeuralNetwork();
    
private:
    
    std::vector<NeuralLayer> mTopology;
    
};

#endif
