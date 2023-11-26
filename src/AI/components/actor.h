#ifndef _AI_ACTOR__
#define _AI_ACTOR__

#include <vector>
#include <mutex>

#include <string>
#include <iostream>

#include <glm/glm.hpp>

#include "../Genetics.h"

#include "../../Physics/PhysicsSystem.h"
#include "../../Renderer/RenderSystem.h"


class __declspec(dllexport) Actor {
    
public:
    
    friend class ActorSystem;
    
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
    
    /// Set the actor position in the world.
    void SetPosition(glm::vec3 newPosition);
    
    /// Get the actor position in the world.
    glm::vec3 GetPosition(void);
    
    /// Set the chance the entity will start moving it its forward direction.
    void SetChanceToMove(int chance);
    
    /// Set the chance the entity will look at the player.
    void SetChanceToLookAtPlayer(int chance);
    
    /// Set the chance the entity will look at another entity.
    void SetChanceToLookAtEntity(int chance);
    
    /// Set the chance the entity will stop moving.
    void SetChanceToStopMoving(int chance);
    
    /// Set the distance the entity will move on average.
    void SetDistanceToMove(float distance);
    
    /// Set the distance the entity will look at the player.
    void SetDistanceToLookAtPlayer(float distance);
    
    /// Set the distance the entity will look another entity.
    void SetDistanceToLookAtEntity(float distance);
    
    /// Set the distance the entity will stop moving if its reached.
    void SetDistanceToStopMoving(float distance);
    
    /// Get the chance the entity will start moving it its forward direction.
    int GetChanceToMove(void);
    
    /// Get the chance the entity will look at the player.
    int GetChanceToLookAtPlayer(void);
    
    /// Get the chance the entity will look at another entity.
    int GetChanceToLookAtEntity(void);
    
    /// Get the chance the entity will stop moving.
    int GetChanceToStopMoving(void);
    
    /// Get the distance the entity will move on average.
    float GetDistanceToMove(void);
    
    /// Get the distance the entity will look at the player.
    float GetDistanceToLookAtPlayer(void);
    
    /// Get the distance the entity will look another entity.
    float GetDistanceToLookAtEntity(void);
    
    /// get the distance the entity will stop moving if its reached.
    float GetDistanceToStopMoving(void);
    
    /// Get the velocity this actor wishes to output.
    glm::vec3 GetVelocity(void);
    
    /// Add a new gene to the actors genome. The index location will be returned.
    void AddGene(Gene& newGene);
    
    /// Remove a gene from the actors genome.
    void RemoveGene(unsigned int index);
    
    /// Get the number of genes in the genome of this actor.
    unsigned int GetNumberOfGenes(void);
    
    /// Get a gene from the genome.
    Gene GetGeneFromGenome(unsigned int index);
    
    Actor();
    
private:
    
    std::string name;
    
    bool isActive;
    bool doUpdateGenetics;
    
    unsigned long int age;
    
    // Environmental sensory (Inputs)
    glm::vec3 worldPosition;
    
    // Personality parameters (Decision making)
    int chanceToMove;
    int chanceToLookAtPlayer;
    int chanceToLookAtEntity;
    int chanceToStopMoving;
    
    float distanceToMove;
    float distanceToLookAtPlayer;
    float distanceToLookAtEntity;
    float distanceToStopMoving;
    
    // Action parameters (Outputs)
    glm::vec3 velocity;
    
    
    // Genetic expression
    std::vector<Gene> mGenes;
    
    std::mutex mux;
    
};





#endif
