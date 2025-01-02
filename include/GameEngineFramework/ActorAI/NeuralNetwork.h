#ifndef _NEURAL_NETWORK_SYSTEM__
#define _NEURAL_NETWORK_SYSTEM__

#include <cstdlib>
#include <vector>

struct NeuralLayer {
    
    std::vector<float> layer;
    
};



class NeuralNetwork {
    
public:
    
    /// Feed a set of input values into the network.
    void FeedForward(const std::vector<float>& input);
    
    /// Get the resulting values after an input has been feed through.
    std::vector<float> GetResults(void);
    
    /// Add a neural layer into the network.
    void AddNeuralLayer(const std::vector<float>& layer);
    
    /// Clear all the neural layers resetting the network.
    void ClearTopology(void);
    
    NeuralNetwork();
    
    
private:
    
    std::vector<NeuralLayer> mTopology;
    
};

#endif
