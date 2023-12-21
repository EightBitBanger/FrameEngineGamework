#ifndef _AI_ACTOR__
#define _AI_ACTOR__

#include <GameEngineFramework/ActorAI/Genetics/Gene.h>
#include <GameEngineFramework/ActorAI/neurons/NeuralLayer.h>
#include <GameEngineFramework/ActorAI/neurons/WeightedLayer.h>

#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Renderer/RenderSystem.h>

#include <glm/glm.hpp>

#include <vector>
#include <mutex>

#include <string>
#include <iostream>


class ENGINE_API Actor {
    
public:
    
    friend class ActorSystem;
    friend class EngineSystemManager;
    
    /// Set the name of the actor.
    void SetName(std::string newName);
    
    /// Set the active state of the actor.
    void SetActive(bool state);
    
    /// Set the age of the actor.
    void SetAge(unsigned long int newAge);
    
    /// Get the name of the actor.
    std::string GetName(void);
    
    /// Get the active state of the actor.
    bool GetActive(void);
    
    /// Get the age of the actor.
    unsigned long int GetAge(void);
    
    
    /// Add a new gene to the actors genome. The index location will be returned.
    void AddGene(Gene& newGene);
    
    /// Remove a gene from the actors genome.
    void RemoveGene(unsigned int index);
    
    /// Get the number of genes in the genome.
    unsigned int GetNumberOfGenes(void);
    
    /// Get a gene from the genome.
    Gene GetGeneFromGenome(unsigned int index);
    
    
    /// Add a weighted layer to the neural network.
    void AddWeightedLayer(WeightedLayer& newNeuralLayer);
    
    /// Remove a weighted layer from the neural network by the given index location.
    void RemoveWeightedLayer(unsigned int index);
    
    /// Get a weighted layer from the neural network.
    WeightedLayer GetWeightedLayer(unsigned int index);
    
    /// Get the number of weighted layers in the neural network.
    unsigned int GetNumberOfWeightedLayers(void);
    
    /// Set the input data to be fed through the neural network.
    void SetNeuralInputLayer(NeuralLayer inputLayer);
    
    Actor();
    
private:
    
    std::string name;
    
    bool isActive;
    bool doUpdateGenetics;
    
    // Number of ticks this actor has accumulated in its lifetime.
    unsigned long int age;
    
    // Movement vector
    glm::vec3 velocity;
    
    // Position of the actor in the world
    glm::vec3 position;
    
    // Genetic expression
    std::vector<Gene> mGenes;
    
    // Layers of weighted neurological expression
    std::vector<WeightedLayer> mWeightedLayers;
    
    std::mutex mux;
    
};





#endif
