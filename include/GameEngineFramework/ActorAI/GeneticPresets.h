#ifndef _AI_GENETIC_PRESETS__
#define _AI_GENETIC_PRESETS__

#include <GameEngineFramework/ActorAI/components/actor.h>


class ENGINE_API GeneticPresets {
    
public:
    
    /// Extract the genome from the entity and return it as a string.
    std::string ExtractGenome(Actor* sourceActor);
    
    /// Inject a genome string into an actor.
    bool InjectGenome(Actor* targetActor, std::string genome);
    
    /// Blends two parent genomes into a single offspring genome.
    bool BlendGenomes(Actor* parentA, Actor* parentB, Actor* offspring);
    
    /// Clear all the genes from an actor genome.
    void ClearGenes(Actor* actorPtr);
    
    /// Expose the genome to random changes.
    void ExposeToRadiation(Actor* actorPtr, float radiationAmount);
    
    /// Linearly interpolate between two genes via the bias value.
    Gene Lerp(Gene geneA, Gene geneB, float bias);
    
    
    /// Preset mental states.
    class ENGINE_API PsychologicalPresets {
        
    public:
        
        void PreyBase(Actor* targetActor);
        
        void PredatorBase(Actor* targetActor);
        
    } mental;
    
    
    /// Preset actor definitions.
    class ENGINE_API ActorPresets {
        
    public:
        
        void Human(Actor* targetActor);
        
        void Sheep(Actor* targetActor);
        
        void Bear(Actor* targetActor);
        
        void Dog(Actor* targetActor);
        
        void Bovine(Actor* targetActor);
        
        void Horse(Actor* targetActor);
        
    private:
        
        void Creepy(Actor* targetActor);
        
    } presets;
    
    
};


#endif
