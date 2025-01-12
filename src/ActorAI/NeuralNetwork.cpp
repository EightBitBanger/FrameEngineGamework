#include <GameEngineFramework/ActorAI/NeuralNetwork.h>


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

