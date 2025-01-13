#include <GameEngineFramework/ActorAI/NeuralNetwork.h>
#include <GameEngineFramework/Types/Types.h>

extern FloatType   Float;
extern UIntType    UInt;


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
    return;
}

std::vector<float> NeuralNetwork::GetResults(void) {
    if (mTopology.empty()) return {};
    return mTopology.back().neurons;
}

void NeuralNetwork::AddNeuralLayer(int numberOfNeurons, int numberOfInputs) {
    NeuralLayer layer;
    
    layer.numberOfInputs = numberOfInputs;
    layer.numberOfNeurons = numberOfNeurons;
    
    layer.neurons.resize(numberOfNeurons);
    layer.biases.resize(numberOfNeurons);
    layer.weights.resize(numberOfNeurons, std::vector<float>(numberOfInputs));
    
    // Initialize weights and biases
    for (int i = 0; i < numberOfNeurons; ++i) {
        
        layer.biases[i] = static_cast<float>(rand()) / RAND_MAX;
        
        for (int j = 0; j < numberOfInputs; ++j) {
            
            layer.weights[i][j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
    
    mTopology.push_back(layer);
    return;
}

void NeuralNetwork::ClearTopology(void) {
    mTopology.clear();
    return;
}

void NeuralNetwork::Train(TrainingSet& trainingSet, float learningRate) {
    FeedForward(trainingSet.input);
    std::vector<std::vector<float>> deltas = CalculateDeltas(trainingSet.target);
    UpdateWeights(trainingSet.input, deltas, learningRate);
    return;
}

std::vector<std::vector<float>> NeuralNetwork::CalculateDeltas(const std::vector<float>& target) {
    std::vector<std::vector<float>> deltas(mTopology.size());
    
    // Calculate output layer deltas
    NeuralLayer& outputLayer = mTopology.back();
    deltas.back().resize(outputLayer.neurons.size());
    for (size_t i = 0; i < outputLayer.neurons.size(); ++i) {
        float error = target[i] - outputLayer.neurons[i];
        deltas.back()[i] = error * ActivationFunctionDerivative(outputLayer.neurons[i]);
    }
    
    // Calculate hidden layer deltas
    for (int i = mTopology.size() - 2; i >= 0; --i) {
        
        NeuralLayer& currentLayer = mTopology[i];
        NeuralLayer& nextLayer = mTopology[i + 1];
        
        deltas[i].resize(currentLayer.neurons.size());
        
        for (size_t j = 0; j < currentLayer.neurons.size(); ++j) {
            
            float error = 0.0f;
            
            for (size_t k = 0; k < nextLayer.neurons.size(); ++k) {
                error += nextLayer.weights[k][j] * deltas[i + 1][k];
            }
            
            deltas[i][j] = error * ActivationFunctionDerivative(currentLayer.neurons[j]);
            
        }
    }
    
    return deltas;
}

void NeuralNetwork::UpdateWeights(const std::vector<float>& input, const std::vector<std::vector<float>>& deltas, float learningRate) {
    std::vector<float> previousLayerOutputs = input;
    
    for (size_t i = 0; i < mTopology.size(); ++i) {
        NeuralLayer& currentLayer = mTopology[i];
        
        for (size_t j = 0; j < currentLayer.neurons.size(); ++j) {
            currentLayer.biases[j] += learningRate * deltas[i][j];
            for (size_t k = 0; k < previousLayerOutputs.size(); ++k) {
                currentLayer.weights[j][k] += learningRate * deltas[i][j] * previousLayerOutputs[k];
            }
        }
        
        previousLayerOutputs = currentLayer.neurons;
    }
    return;
}

float NeuralNetwork::ActivationFunctionDerivative(float value) {
    return 1.0f - std::pow(tanh(value), 2);
}


void NeuralNetwork::LoadState(std::vector<std::string>& state) {
    
    mTopology.clear();
    
    // Setup the layers
    for (unsigned int i=0; i < state.size(); i++) {
        
        std::string& layerString = state[i];
        
    }
    
    return;
}


std::vector<std::string> NeuralNetwork::SaveState(void) {
    
    // Each layer
    unsigned int numberOfLayers = mTopology.size();
    
    std::vector<std::string> state;
    std::string layerString;
    
    for (unsigned int i=0; i < numberOfLayers; i++) {
        
        NeuralLayer& layer = mTopology[i];
        
        unsigned int numberOfNeurons     = layer.neurons.size();
        unsigned int numberOfBiases      = layer.biases.size();
        unsigned int numberOfWeightLists = layer.weights.size();
        
        layerString  = UInt.ToString(layer.numberOfInputs) + " ";
        layerString += UInt.ToString(layer.numberOfNeurons) + " ";
        
        layerString += UInt.ToString(numberOfNeurons) + " ";
        layerString += UInt.ToString(numberOfBiases) + " ";
        layerString += UInt.ToString(numberOfWeightLists) + " ";
        
        // Each bias
        for (unsigned int b=0; b < numberOfBiases; b++) {
            
            layerString += Float.ToString( layer.biases[b] ) + " ";
            
        }
        
        // Each set of weights
        for (unsigned int ws=0; ws < numberOfWeightLists; ws++) {
            
            std::vector<float>& weights = layer.weights[ws];
            
            unsigned int numberOfWeights = weights.size();
            
            // Each weight
            for (unsigned int w=0; w < numberOfWeights; w++) 
                layerString += Float.ToString( weights[w] ) + " ";
            
        }
        
        state.push_back(layerString);
    }
    
    return state;
}

