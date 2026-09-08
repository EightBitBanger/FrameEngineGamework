#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include <GameEngineFramework/ActorAI/ActorStates.h>
#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>

#include <GameEngineFramework/ActorAI/genetics/Gene.h>
#include <GameEngineFramework/ActorAI/genetics/Codon.h>
#include <GameEngineFramework/ActorAI/genetics/Phen.h>
#include <GameEngineFramework/ActorAI/genetics/Bio.h>

#include <GameEngineFramework/ActorAI/neural/EmotionalEmbedding.h>

#include <GameEngineFramework/ActorAI/components/actor.h>
#include <GameEngineFramework/Audio/AudioSystem.h>

#include <GameEngineFramework/Timer/Timer.h>

#include <thread>
#include <mutex>
#include <chrono>
#include <functional>

#include <fstream>
#include <vector>
#include <string>

class UniversalConstants {
    friend class ActorSystem;
public:
    
    const float sentienceThreshold   = 0.8f;
    const float emotionalThreshold   = 0.5f;
    const float biologicalThreshold  = 0.5f;
};

enum class GenealogyEventType : unsigned int {
    Birth = 0,
    Death = 1
};

class ENGINE_API Genealogy {
public:
    GenealogyEventType eventType;
    std::string        family;
    std::string        actor;      // Child at birth, or deceased actor name
    std::string        father;     // Father name (birth events)
    std::string        mother;     // Mother name (birth events)
    std::string        details;    // Cause of death or birth notes
    unsigned int       generation;
    unsigned long int  age;
    glm::vec3          position;

    Genealogy() 
        : eventType(GenealogyEventType::Birth), 
          family(""), 
          actor(""), 
          father(""), 
          mother(""), 
          details(""), 
          generation(0), 
          age(0), 
          position(0.0f) 
    {}
};

class ENGINE_API ActorSystem {
    friend class InventoryManager;
    friend class EngineSystemManager;
public:
    
    using WorldPickupQueryCallback = std::function< std::vector<std::pair<std::string, glm::vec3>>(const glm::vec3& position, float range) >;
    using WorldPickupPlaceCallback   = std::function< bool(const std::string& type, const glm::vec3& worldPosition, const glm::vec3& rotation) >;
    using WorldPickupRemoveCallback  = std::function< bool(const glm::vec3& worldPosition, std::string& collectedItem) >;
    
    using WorldStaticRaycastCallback = std::function< std::vector<std::pair<std::string, glm::vec3>>(const glm::vec3& position, float range) >;
    using WorldStaticPlaceCallback   = std::function< bool(const std::string& type, const glm::vec3& worldPosition, const glm::vec3& rotation) >;
    using WorldStaticRemoveCallback  = std::function< bool(const glm::vec3& worldPosition, std::string& collectedItem) >;
    
    using WorldGetNameCallback = std::function< std::string(int language, int namePart) >;
    
    ActorSystem();
    
    /// Create an actor and return its pointer.
    Actor* CreateActor(void);
    /// Destroy an actor.
    bool DestroyActor(Actor* actorPtr);
    
    /// Set the player position in the simulation.
    void SetPlayerWorldPosition(glm::vec3 position);
    /// Get the player position in the simulation.
    glm::vec3 GetPlayerWorldPosition(void);
    
    /// Get the number of actors in the simulation.
    unsigned int GetNumberOfActors(void);
    /// Get an actor from the simulation by its index.
    Actor* GetActor(unsigned int index);
    
    /// Set the update distance from the camera position.
    void SetActorUpdateDistance(float distance);
    /// Set the render distance from the camera position.
    void SetActorRenderDistance(float distance);
    
    /// Set the distance where actor detail will not be drawn
    void SetWaterLevel(float waterLevel);
    /// Get the distance where actor detail will not be drawn
    float GetWaterLevel(void);
    
    /// Set the current time of day for the actor system.
    void SetTimeOfDay(float time);
    /// Get the current time of day for the actor system.
    float GetTimeOfDay(void);
    
    /// Record a birth event to the genealogy log.
    void RecordBirth(Actor* child, Actor* father, Actor* mother, const std::string& familyName, const glm::vec3& position);

    /// Record a death event to the genealogy log.
    void RecordDeath(Actor* actor, const std::string& cause);

    /// Export the unified chronological genealogy log to a CSV file.
    bool DumpGenealogy(const std::string& filename);

    /// Get all genealogy records in chronological order.
    std::vector<Genealogy> GetGenealogy(void);

    /// Clear the genealogy log.
    void ClearGenealogy(void);
    
    
    // Internal
    
    /// Initiate the actor AI system.
    void Initiate(void);
    
    /// Shutdown the actor AI system.
    void Shutdown(void);
    
    /// Update the actors animation cycles.
    void UpdateThinking(void);
    
    /// Genetic entity definitions.
    GeneticPresets genomes;
    
    // Render components
    
    /// Scene where the actors should exist.
    Scene* sceneMain;
    
    /// Shader to render the actors.
    Shader* shader;
    
    /// Base mesh for genetic rendering.
    Mesh* baseMesh;
    
    /// Raycast against actors in the world.
    Actor* Raycast(const glm::vec3& position, const glm::vec3& direction, float maxDistance = -1.0f);
    
    /// Get the number of renderers from dead actors.
    unsigned int GetNumberOfDeadRenderers();
    
    /// Swap the list of dead actor renderers.
    void SwapDeadRendererList(std::vector<MeshRenderer*>& newList);
    
    /// Enable the debug line renderer.
    bool DebugRendererEnable(void);
    
    /// Disable the debug line renderer.
    void DebugRendererDisable(void);
    
    /// Draw a debug line in the debug renderer.
    void DebugRenderDrawLine(glm::vec3 from, glm::vec3 to, Color color);
    
    /// Register the pickup object interaction functions.
    void SetWorldPickupCallbacks(WorldPickupQueryCallback query, WorldPickupPlaceCallback place, WorldPickupRemoveCallback destroy);
    
    /// Register the static geometry world interaction functions.
    void SetWorldStaticCallbacks(WorldPickupQueryCallback query, WorldPickupPlaceCallback place, WorldPickupRemoveCallback destroy);
    
    /// Register the name generator for actor naming.
    void SetNameGenerator(WorldGetNameCallback getter);
    
    /// Set the simulation time multiplier.
    void SetTimeScale(float scale);
    /// Get the current simulation time multiplier.
    float GetTimeScale(void);
    
    // Master update timer
    Timer mainTimer;
    // Auxiliary fast update timer
    Timer auxiliaryTimer;
    // Animation update timer
    Timer animationTimer;
    
    void UpdateTick();
    void UpdateFast();
    void UpdateFastAnimation();
    
private:
    
    // Behavioral
    void UpdateActorState(Actor* actor, EmotionalEmbedding& emotion, float sentientScore);
    void UpdateGazeTarget(Actor* actor);
    void UpdateThoughtMatrix(Actor* actor, EmotionalEmbedding& emotion, float sentientScore);
    void UpdateTargetingMechanics(Actor* actor);
    
    void ApplyEmotionThresholds(const MemoryTrigger& trigger, EmotionalEmbedding& embedding);
    bool EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& embedding);
    void ProjectEmotionalHistory(Actor* actor, EmotionalEmbedding& currentEmotion, float sentientScore);
    int EvaluateThoughtMatrix(Actor* actor, const EmotionalEmbedding& baselineEmotion, float sentientScore, std::vector<EmotionalEmbedding>& outThoughtMatrix);
    void ProcessMemoryTriggers(Actor* actor, EmotionalEmbedding& emotion);
    
    // Count actors currently socializing around a focal target
    int GetSocialGroupSize(Actor* focalActor);
    
    // Mechanical
    void HandleMovementMechanics(Actor* actor);
    float ApplyApproachSlowdown(Actor* actor, const glm::vec3& targetPosition, float currentSpeedScaler, float targetStopDistance);
    
    bool HandleHomeLocation(Actor* actor);
    bool HandleTargetDistance(Actor* actor);
    bool HandleInflictDamage(Actor* actor, Actor* target);
    bool HandleEscapeEvade(Actor* actor, Actor* target);
    bool HandleBreedWith(Actor* actor, Actor* target);
    bool HandleSocializeWith(Actor* actor, Actor* target);
    bool HandleVitality(Actor* actor);
    void HandleCooldownCounters(Actor* actor);
    
    glm::vec3 CalculateForwardVelocity(Actor* actor);
    glm::vec3 CalculateRandomLocalPoint(Actor* actor);
    void CalculateTargetOffsetting(Actor* actor, float distanceScale);
    
    // Animation
    void UpdateAnimationState(Actor* actor);
    
    void UpdateAnimationBody(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a);
    void UpdateAnimationHead(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a);
    void UpdateAnimationLimb(glm::mat4& matrix, Actor* actor, unsigned int a);
    void UpdateAnimationInHand(glm::mat4& matrix, Actor* actor, unsigned int a);
    
    void EnsureNonZeroAnimationState(Actor* actor, unsigned int a);
    void ApplyAnimationRotation(glm::mat4& matrix, Actor* actor, unsigned int a);
    void UpdateHeadRotation(glm::mat4& matrix, Actor* actor, unsigned int a);
    void HandleAnimationSwing(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange, bool animationDirection);
    void UpdateTargetRotation(Actor* actor);
    float CalculateLimbSwingAngle(Actor* actor, unsigned int a);
    
    // Genetics
    void UpdateActorGenetics(Actor* actor);
    void ClearOldGeneticRenderers(Actor* actor);
    MeshRenderer* CreateMeshRendererForGene(Actor* actor, unsigned int geneIndex, Mesh* sourceMesh);
    void ExpressActorGenetics(Actor* actor);
    void CombineParentMemories(Actor* parentA, Actor* parentB, Actor* child);
    
    // Inventory
    bool UpdateActorInventory(Actor* actor);
    
    // Memories
    bool IsMemoryShareable(TriggerType type);
    void ShareMemories(Actor* source, Actor* target);
    
    // Submit a line for debug rendering
    void DebugRenderDrawLine(glm::vec3 from, glm::vec3 to);
    
    // Garbage collection
    bool UpdateGarbageCollection(Actor* actor);
    
    // Current position of the player in the world
    glm::vec3 mPlayerPosition;
    
    // Max update distance
    float mActorUpdateDistance;
    
    // Max render distance
    float mActorRenderDistance;
    
    // Maximum world water level
    float mWorldWaterLevel;
    
    // Time of day
    float mTimeOfDay;
    
    // World query ray cast callback function
    WorldPickupQueryCallback   mWorldPickupQueryCallback;
    WorldPickupPlaceCallback   mWorldPickupPlaceCallback;
    WorldPickupRemoveCallback  mWorldPickupRemoveCallback;
    
    WorldStaticRaycastCallback mWorldStaticQueryCallback;
    WorldStaticPlaceCallback   mWorldStaticPlaceCallback;
    WorldStaticRemoveCallback  mWorldStaticRemoveCallback;
    
    WorldGetNameCallback       mWorldGetNameCallback;
    
    // Debug line renderer
    MeshRenderer* mDebugLineRenderer;
    
    std::vector<MeshRenderer*> mDeadActorRenderers;
    
    std::thread* mActorSystemThread;
    std::thread* mAnimationThread;
    std::mutex mux;
    
    PoolAllocator<Actor> mActors;
    
    std::vector<Actor*> mActiveActors;
    std::vector<Actor*> mFreeActors;
    
    std::vector<Genealogy> mGenealogy;
    
    unsigned int mNumberOfActors;
    
    float mTimeScale;
    double mFrameTimeCurrent;
};


#endif
