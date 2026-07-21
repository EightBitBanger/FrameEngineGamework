#ifndef AI_ACTOR
#define AI_ACTOR

#include <GameEngineFramework/ActorAI/ActorStates.h>

#include <GameEngineFramework/ActorAI/genetics/Gene.h>
#include <GameEngineFramework/ActorAI/genetics/Codon.h>
#include <GameEngineFramework/ActorAI/genetics/Phen.h>
#include <GameEngineFramework/ActorAI/genetics/Bio.h>
#include <GameEngineFramework/ActorAI/neural/EmotionalEmbedding.h>

#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Audio/AudioSystem.h>
#include <GameEngineFramework/Math/Math.h>

#include <GameEngineFramework/ActorAI/components/components/inventory.h>
#include <GameEngineFramework/ActorAI/components/components/navigation.h>
#include <GameEngineFramework/ActorAI/components/components/behavior.h>
#include <GameEngineFramework/ActorAI/components/components/vocal.h>
#include <GameEngineFramework/ActorAI/components/components/state.h>
#include <GameEngineFramework/ActorAI/components/components/animation.h>
#include <GameEngineFramework/ActorAI/components/components/idiosync.h>
#include <GameEngineFramework/ActorAI/components/components/genetics.h>
#include <GameEngineFramework/ActorAI/components/components/biological.h>
#include <GameEngineFramework/ActorAI/components/components/emotional.h>
#include <GameEngineFramework/ActorAI/components/components/physical.h>
#include <GameEngineFramework/ActorAI/components/components/cooldown.h>
#include <GameEngineFramework/ActorAI/components/components/user.h>

#include <unordered_map>
#include <vector>
#include <mutex>

#include <string>

class ENGINE_API Actor {
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    // Flags
    
    bool isGarbage;     // Mark for internal destruction and garbage collection
    bool isActive;      // Update the actor in the world
    bool isSaved;       // Used to track saving state
    
    // Name
    
    /// Set the name of the actor.
    void SetName(const std::string& newName);
    /// Get the name of the actor.
    const std::string& GetName(void);
    
    // Bounding box
    
    /// Set bounding box region.
    void SetBoundingBox(const glm::vec3& min, const glm::vec3& max);
    /// Get the minimum bounding box corner.
    glm::vec3 GetBoundingBoxMin(void);
    /// Get the maximum bounding box corner.
    glm::vec3 GetBoundingBoxMax(void);
    
    // State
    
    /// Trigger a rebuild of the genome.
    void RebuildGeneticExpression(void);
    
    /// Update the bounding area by the positional offsets of the genetic elements.
    void CalculateBoundingRegionFromGenome(void);
    
    /// Re-initiate the state of the actor. The mesh renderers will not be
    /// reset as they are owned and must be reset by the engine system.
    void Reset(void);
    
    Actor();
    
    InventoryManager     inventory;
    NavigationSystem     navigation;
    Behavior             behavior;
    VocalSynthesizer     voice;
    State                state;
    AnimationState       animation;
    MemorySystem         memories;
    GeneticsSystem       genetics;
    BiologicalSystem     biological;
    EmotionalState       emotions;
    PhysicalAttributes   physical;
    CooldownCounters     counters;
    UserVariables        user;
    
private:
    
    // Actor name string
    std::string mName;
    
    // Bounding box area for hit detection
    glm::vec3 mBoundingBoxMin;
    glm::vec3 mBoundingBoxMax;
    
    // Counter to offset the update calls
    unsigned int mUpdateCounter;
    
    std::vector<Actor*> mTargets;
};


#endif
