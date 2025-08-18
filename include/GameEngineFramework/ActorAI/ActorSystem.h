#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include <GameEngineFramework/ActorAI/ActorStates.h>
#include <GameEngineFramework/ActorAI/GeneticPresets.h>

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>

#include <GameEngineFramework/ActorAI/Genetics/Gene.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>
#include <GameEngineFramework/ActorAI/Genetics/Phen.h>
#include <GameEngineFramework/ActorAI/Genetics/Bio.h>

#include <GameEngineFramework/ActorAI/components/actor.h>

#include <GameEngineFramework/Timer/Timer.h>

#include <thread>
#include <mutex>
#include <chrono>


class ENGINE_API ActorSystem {
    
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
    
    
    /// Set the distance where actor detail will not be drawn
    void SetWaterLevel(float waterLevel);
    
    /// Get the distance where actor detail will not be drawn
    float GetWaterLevel(void);
    
    
    // Called internally
    
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
    void UpdateProximityList(Actor* actor);
    
    bool HandleWalkState(Actor* actor);
    bool HandleAttackState(Actor* actor, Actor* target, float distance);
    bool HandleFleeState(Actor* actor, Actor* target, float distance);
    bool HandleFocusState(Actor* actor, Actor* target, float distance);
    bool HandleBreedingState(Actor* actor, Actor* target, float distance);
    void HandleNeuralNetwork(Actor* actor);
    
    // Mechanical
    void HandleMovementMechanics(Actor* actor);
    void HandleTargettingMechanics(Actor* actor);
    void HandleTargetDistance(Actor* actor);
    
    void HandleInflictDamage(Actor* actor, Actor* target);
    void HandleEscapeEvade(Actor* actor, Actor* target);
    void HandleBreedWith(Actor* actor, Actor* target);
    void HandleVitality(Actor* actor);
    void HandleCooldownCounters(Actor* actor);
    
    glm::vec3 CalculateForwardVelocity(Actor* actor);
    glm::vec3 CalculateRandomLocalPoint(Actor* actor);
    
    // Animation
    void UpdateAnimationState(Actor* actor);
    
    void UpdateAnimationBody(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a);
    void UpdateAnimationHead(glm::mat4& matrix, Actor* actor, MeshRenderer* geneRenderer, unsigned int a);
    void UpdateAnimationLimb(glm::mat4& matrix, Actor* actor, unsigned int a);
    
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
    bool UpdateGarbageCollection(Actor* actor);
    
    // Memories
    void UpdateActorMemories(Actor* actor);
    
    // Current position of the player in the world
    glm::vec3 mPlayerPosition;
    
    // Distance beyond which the actors will no longer update
    float mActorUpdateDistance;
    
    // Maximum world water level
    float mWorldWaterLevel;
    
    std::thread* mActorSystemThread;
    std::mutex mux;
    
    PoolAllocator<Actor> mActors;
    
    std::vector<Actor*> mActiveActors;
    std::vector<Actor*> mFreeActors;
    
    unsigned int mNumberOfActors;
    
};


#endif
