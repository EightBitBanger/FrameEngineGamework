#ifndef AI_ACTOR_GENETICS
#define AI_ACTOR_GENETICS

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API GeneticsSystem {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Add a new gene to the actors genome. The index location will be returned.
    unsigned int AddGene(Gene& newGene);
    /// Remove a gene from the actors genome.
    void RemoveGene(unsigned int index);
    
    /// Clear all the genes in this actors genome.
    void ClearGenome(void);
    /// Clear all the phenotype data.
    void ClearPhenome(void);
    
    /// Set the generation of the actor.
    void SetGeneration(unsigned int newAge);
    /// Get the generation of the actor.
    unsigned int GetGeneration(void);
    
    /// Get the number of genes in the genome.
    unsigned int GetNumberOfGenes(void);
    
    /// Get a gene from the genome.
    Gene GetGeneFromGenome(unsigned int index);
    
    /// Get a phenotype from the actor. The number of phenotypes
    /// should match the number of genes.
    Phen GetPhenFromPhenotype(unsigned int index);
    
    /// Get the number of mesh renderers associated with this actor.
    unsigned int GetNumberOfMeshRenderers(void);
    
    /// Return a mesh renderer at the given index.
    MeshRenderer* GetMeshRendererAtIndex(unsigned int index);
    
    GeneticsSystem();
    
private:
    
    bool mDoUpdateGenetics;       // Should the genetics be re constructed
    bool mDoReexpressGenetics;    // Should the genetics be re-expressed
    unsigned int mGeneration;     // Current position in the generational sequence over time
    
    std::vector<Gene> mGenes;     // Genetic blueprints
    std::vector<Phen> mPhen;      // Phenotypic expression
    
    // List of render components representing genetic expression
    std::vector<MeshRenderer*> mGeneticRenderers;
    
};

#endif
