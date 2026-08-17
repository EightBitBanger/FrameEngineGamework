#ifndef _AI_GENETIC_PRESETS__
#define _AI_GENETIC_PRESETS__

#include <GameEngineFramework/ActorAI/components/actor.h>
class Actor;

class ENGINE_API GeneticPresets {
    
public:
    
    /// Extract the genome from the entity and return it as a string.
    std::string ExtractGenome(Actor* sourceActor);
    
    /// Inject a genome string into an actor.
    bool InjectGenome(Actor* targetActor, const std::string& genome);
    
    /// Blends two parent genomes into a single offspring genome.
    bool BlendGenomes(Actor* parentA, Actor* parentB, Actor* offspring);
    
    /// Clear all the genes from an actor genome.
    void ClearGenes(Actor* actorPtr);
    
    /// Expose the genome to random changes.
    void ExposeToRadiation(Actor* actorPtr, float radiationAmount);
    
    /// Linearly interpolate between two genes via a bias value.
    Gene Lerp(Gene geneA, Gene geneB, float bias);
    
    /// Preset actor definitions.
    class ENGINE_API ActorPresets {
        
    public:
        // Sapien
        void HumanWhite(Actor* targetActor);
        void HumanRed(Actor* targetActor);
        void HumanBrown(Actor* targetActor);
        void HumanBlack(Actor* targetActor);
        
        void Human(Actor* targetActor, Color& limb, Color& body);
        
        // Animal
        
        void Bear(Actor* targetActor);
        void Dog(Actor* targetActor);
        void Cat(Actor* targetActor);
        void Rabbit(Actor* targetActor);
        
        void Sheep(Actor* targetActor);
        void Boar(Actor* targetActor);
        void Bovine(Actor* targetActor);
        void Horse(Actor* targetActor);
        
        void Elk(Actor* targetActor);
        void Camel(Actor* targetActor);
        
        // Insect
        
        void Spider(Actor* targetActor);
        void Ladybug(Actor* targetActor);
        
        // Fantasy
        
        void Dwarf(Actor* targetActor);
        void Elf(Actor* targetActor);
        void Goblin(Actor* targetActor);
        void Orc(Actor* targetActor);
        void Halfling(Actor* targetActor);
        void Troll(Actor* targetActor);
        
    private:
        
        void Creepy(Actor* targetActor);
        
        void Raptor(Actor* targetActor);
        
    } presets;
    
    
    class ENGINE_API GeneticBuilder {
        
    public:
        
        Gene BuildAddLeg(float xx, float yy, float zz, float length, float width, Color color, bool invert, bool side, float canter, float tilt, float offset);
        
    } build;
    
};


#endif
