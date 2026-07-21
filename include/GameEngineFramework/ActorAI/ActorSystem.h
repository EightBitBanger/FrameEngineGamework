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

class UniversalConstants {
    friend class ActorSystem;
    
    const float sentienceThreshold   = 0.8f;
    const float emotionalThreshold   = 0.5f;
};

class ENGINE_API ActorSystem {
    friend class InventoryManager;
public:
    
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
    
    /// Signal to the AI thread to update the simulation.
    void UpdateSendSignal(void);
    
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
    
    
    // Internal
    
    /// Initiate the actor AI system.
    void Initiate(void);
    
    /// Shutdown the actor AI system.
    void Shutdown(void);
    
    /// Update the actors in the simulation.
    void Update();
    
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
    
    /// Get a renderer from a dead actor.
    MeshRenderer* GetDeadRenderer(unsigned int index);
    
    /// Remove a renderer from the dead actor renderer list.
    MeshRenderer* RemoveDeadRenderer(unsigned int index);
    
    /// Enable the debug line renderer.
    bool DebugRendererEnable(void);
    
    /// Disable the debug line renderer.
    void DebugRendererDisable(void);
    
    /// Draw a debug line in the debug renderer.
    void DebugRenderDrawLine(glm::vec3 from, glm::vec3 to, Color color);
    
private:
    
    // Update at the rate of tick
    void UpdateTick();
    // Update at the frame rate
    void UpdateFast();
    
    // Master update timer
    Timer mMainTimer;
    // Animation update timer
    Timer mAnimationTimer;
    
    // Behavioral
    void UpdateActorState(Actor* actor);
    void ApplyEmotionThresholds(const std::string& emotionType, float value, EmotionalEmbedding& embedding);
    bool EvaluateEmotionalBehavior(Actor* actor, Actor* targetActor, float threshold, EmotionalEmbedding& embedding);
    void ProjectEmotionalHistory(Actor* actor, EmotionalEmbedding& currentEmotion, float sentientScore);
    int EvaluateThoughtMatrix(Actor* actor, const EmotionalEmbedding& baselineEmotion, float sentientScore, std::vector<EmotionalEmbedding>& outThoughtMatrix);
    void ProcessMemoryTriggers(Actor* actor, EmotionalEmbedding& emotion);
    
    // Mechanical
    void HandleMovementMechanics(Actor* actor);
    void HandleTargettingMechanics(Actor* actor);
    
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
    
    // Genetics
    void UpdateActorGenetics(Actor* actor);
    void ClearOldGeneticRenderers(Actor* actor);
    MeshRenderer* CreateMeshRendererForGene(Actor* actor, unsigned int geneIndex, Mesh* sourceMesh);
    void ExpressActorGenetics(Actor* actor);
    void CombineParentMemories(const Actor* parentA, const Actor* parentB, Actor* child);
    
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
    
    // Debug line renderer
    MeshRenderer* mDebugLineRenderer;
    
    std::vector<MeshRenderer*> mDeadActorRenderers;
    
    std::thread* mActorSystemThread;
    std::mutex mux;
    
    PoolAllocator<Actor> mActors;
    
    std::vector<Actor*> mActiveActors;
    std::vector<Actor*> mFreeActors;
    
    unsigned int mNumberOfActors;
};


#endif
