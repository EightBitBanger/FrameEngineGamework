#ifndef _AI_GENETIC_PRESETS__
#define _AI_GENETIC_PRESETS__

#include <GameEngineFramework/ActorAI/components/actor.h>

class ENGINE_API GeneticPresets {
    
public:
    
    /// Extract the genome from the entity and return it as a string.
    std::string ExtractGenome(Actor* actorSource);
    
    /// Inject a genome string into an actor.
    bool InjectGenome(Actor* actorSource, std::string genome);
    
    /// Clear all the genes from an actor genome.
    void ClearGenes(Actor* actorPtr);
    
    /// Expose the genome to random changes.
    void ExposeToRadiation(Actor* actorPtr, float radiationMultiplier);
    
    // Mental models
    
    void PreyBase(Actor* targetActor);
    
    // Genomes
    
    void Sheep(Actor* targetActor);
    
    void Bear(Actor* targetActor);
    
};


#endif
