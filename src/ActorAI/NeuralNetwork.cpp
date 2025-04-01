#include <GameEngineFramework/ActorAI/NeuralNetwork.h>
#include <GameEngineFramework/Types/Types.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <sstream>

extern FloatType    Float;
extern UIntType     UInt;
extern StringType   String;


NeuralNetwork::NeuralNetwork() {}

void NeuralNetwork::FeedForward(const std::vector<float>& input) {
    
    if (mTopology.empty()) 
        return;
    
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
            
            //currentLayer.neurons[j] = ActivationReLU(sum);
            
        }
    }
    return;
}

std::vector<float> NeuralNetwork::GetResults(void) {
    if (mTopology.empty()) return {0.0f};
    return mTopology.back().neurons;
}

unsigned int NeuralNetwork::GetNumberOfLayers(void) {
    
    return mTopology.size();
}

void NeuralNetwork::AddNeuralLayer(int numberOfNeurons, int numberOfInputs) {
    NeuralLayer layer;
    
    layer.numberOfInputs = numberOfInputs;
    layer.numberOfNeurons = numberOfNeurons;
    
    layer.neurons.resize(numberOfNeurons);
    layer.biases.resize(numberOfNeurons);
    layer.weights.resize(numberOfNeurons, std::vector<float>(numberOfInputs));
    
    
    for (int i = 0; i < numberOfNeurons; ++i) {
        for (int j = 0; j < numberOfInputs; ++j) {
            layer.weights[i][j] = ((Random.Range(0, 100) * 0.01) / 2) * 0.5f;
        }
    }
    
    // Xavier initialization
    /*
    float range = sqrt(6.0f / (numberOfInputs + numberOfNeurons));
    
    for (int i = 0; i < numberOfNeurons; ++i) {
        for (int j = 0; j < numberOfInputs; ++j) {
            layer.weights[i][j] = (static_cast<float>(rand()) / RAND_MAX) * 2 * range - range;
        }
    }
    */
    
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
        
        // This activation function seems to yield more accurate results
        deltas.back()[i] = error * ActivationFunctionDerivative( outputLayer.neurons[i] );
        
        //deltas.back()[i] = error * ActivationReLUDerivative( outputLayer.neurons[i] );
        
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
            
            deltas[i][j] = error * ActivationFunctionDerivative( currentLayer.neurons[j] );
            
            //deltas[i][j] = error * ActivationReLUDerivative( currentLayer.neurons[j] );
            
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

// Activation functions

float NeuralNetwork::ActivationReLU(float value) {
    return std::max(0.0f, value);
}

float NeuralNetwork::ActivationReLUDerivative(float value) {
    return value > 0 ? 1.0f : 0.0f;
}

float NeuralNetwork::ActivationFunctionDerivative(float value) {
    return 1.0f - std::pow(tanh(value), 2);
}

// Save/load neural states

void NeuralNetwork::LoadState(std::vector<std::string>& state) {
    mTopology.clear();
    
    // Setup the layers
    for (const std::string& layerString : state) {
        std::istringstream stream(layerString);
        
        NeuralLayer layer;
        stream >> layer.numberOfInputs;
        stream >> layer.numberOfNeurons;
        
        unsigned int numberOfNeurons, numberOfBiases, numberOfWeightLists;
        stream >> numberOfNeurons;
        stream >> numberOfBiases;
        stream >> numberOfWeightLists;
        
        // Resize the vectors to hold the biases and weights
        layer.neurons.resize(numberOfNeurons);
        layer.biases.resize(numberOfBiases);
        layer.weights.resize(numberOfWeightLists, std::vector<float>(layer.numberOfInputs));
        
        // Load biases
        for (unsigned int b = 0; b < numberOfBiases; ++b) {
            std::string biasString;
            stream >> biasString;
            
            layer.biases[b] = String.ToFloat(biasString);
        }
        
        // Load weights
        for (unsigned int ws = 0; ws < numberOfWeightLists; ++ws) {
            
            for (unsigned int w = 0; w < layer.numberOfInputs; ++w) {
                std::string weightString;
                stream >> weightString;
                
                layer.weights[ws][w] = String.ToFloat(weightString);
            }
            
        }
        
        mTopology.push_back(layer);
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

void NeuralNetwork::LoadStateBin(const std::vector<float>& state) {
    mTopology.clear();
    
    size_t index = 0;
    
    while (index < state.size()) {
        NeuralLayer layer;
        layer.numberOfInputs = static_cast<int>(state[index++]);
        layer.numberOfNeurons = static_cast<int>(state[index++]);
        
        unsigned int numberOfNeurons = static_cast<unsigned int>(state[index++]);
        unsigned int numberOfBiases = static_cast<unsigned int>(state[index++]);
        unsigned int numberOfWeightLists = static_cast<unsigned int>(state[index++]);
        
        layer.neurons.resize(numberOfNeurons);
        layer.biases.resize(numberOfBiases);
        layer.weights.resize(numberOfWeightLists, std::vector<float>(layer.numberOfInputs));
        
        for (unsigned int b = 0; b < numberOfBiases; ++b) {
            layer.biases[b] = state[index++];
        }
        
        for (unsigned int ws = 0; ws < numberOfWeightLists; ++ws) {
            for (unsigned int w = 0; w < layer.numberOfInputs; ++w) {
                layer.weights[ws][w] = state[index++];
            }
        }
        
        mTopology.push_back(std::move(layer));
    }
    
    return;
}

std::vector<float> NeuralNetwork::SaveStateBin(void) {
    std::vector<float> state;
    
    for (const NeuralLayer& layer : mTopology) {
        
        state.push_back(static_cast<float>(layer.numberOfInputs));
        state.push_back(static_cast<float>(layer.numberOfNeurons));
        state.push_back(static_cast<float>(layer.neurons.size()));
        state.push_back(static_cast<float>(layer.biases.size()));
        state.push_back(static_cast<float>(layer.weights.size()));
        
        for (const float bias : layer.biases) {
            state.push_back(bias);
        }
        
        for (const auto& weights : layer.weights) {
            for (const float weight : weights) {
                state.push_back(weight);
            }
        }
    }
    
    return state;
}
