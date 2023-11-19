#ifndef _AI_ACTOR__
#define _AI_ACTOR__

#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

#include "../Genetics.h"
#include "../../Physics/PhysicsSystem.h"
#include "../../Renderer/RenderSystem.h"


class __declspec(dllexport) Actor {
    
public:
    
    std::string name;
    
    bool isActive;
    
    unsigned long int age;
    
    // AI personality parameters
    
    int chanceToMove;
    int chanceToLookAtPlayer;
    int chanceToLookAtEntity;
    int chanceToStopMoving;
    
    float distanceToMove;
    float distanceToLookAtPlayer;
    float distanceToLookAtEntity;
    float distanceToStopMoving;
    
    
    Actor();
    
    void Update(void);
    
    /// Add a new gene to the actors genome. The gene index location will returned.
    unsigned int AddGene(Gene& newGene);
    /// Remove a gene from the actors genome.
    void RemoveGene(unsigned int index);
    
    
    
private:
    
    // Actor genome
    std::vector<Gene> mGenes;
    
};





#endif
