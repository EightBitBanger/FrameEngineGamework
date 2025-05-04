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
    
    
    /// Add an actor to the simulation.
    bool AddActorToSimulation(Actor* actorPtr);
    
    /// Remove an actor from the simulation.
    bool RemoveActorFromSimulation(Actor* actorPtr);
    
    /// Return an actor from the simulation.
    Actor* GetActorFromSimulation(unsigned int index);
    
    
    /// Set the player position in the simulation.
    void SetPlayerWorldPosition(glm::vec3 position);
    
    /// Get the player position in the simulation.
    glm::vec3 GetPlayerWorldPosition(void);
    
    /// Get the number of actors in the AI simulation.
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
    
    // Object generation and deletion callbacks
    
    /// Actor object spawner callback
    Actor* (*SpawnActor)(void);
    
    /// Actor object destroyer callback
    void (*KillActor)(Actor*);
    
private:
    
    // Master update timer
    Timer mMainTimer;
    // Animation update timer
    Timer mAnimationTimer;
    
    // Behavioral
    void UpdateActorState(Actor* actor);
    void UpdateProximityList(Actor* actor);
    
    void HandleIdleState(Actor* actor);
    void HandleWalkState(Actor* actor);
    void HandleAttackState(Actor* actor, Actor* target, float distance);
    void HandleFleeState(Actor* actor, Actor* target, float distance);
    void HandleFocusState(Actor* actor, Actor* target, float distance);
    
    // Mechanical
    void UpdateActorMechanics(Actor* actor);
    
    void HandleWalkingMechanics(Actor* actor);
    void HandleTargettingMechanics(Actor* actor);
    void HandleBreedingMechanics(Actor* actor);
    
    void HandleMovementMechanics(Actor* actor);
    
    
    // Animations
    void UpdateAnimationState(Actor* actor);
    void EnsureNonZeroAnimationState(Actor* actor, unsigned int a);
    void ApplyAnimationRotation(glm::mat4& matrix, Actor* actor, unsigned int a);
    void HandleAnimationSwingBackward(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange);
    void HandleAnimationSwingForward(Actor* actor, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange);
    void UpdateAnimation(glm::mat4& matrix, Actor* actor, unsigned int a);
    void ApplyOffsetFromCenter(glm::mat4& matrix, Actor* actor, unsigned int a);
    void ApplyRotation(glm::mat4& matrix, Actor* actor, unsigned int a);
    void ApplyScaleByAge(glm::mat4& matrix, Actor* actor, unsigned int a);
    void UpdateTargetRotation(Actor* actor);
    void UpdateLookingRotation(Actor* actor);
    
    // Genetics
    void UpdateActorGenetics(Actor* actor);
    void ClearOldGeneticRenderers(Actor* actor);
    MeshRenderer* CreateMeshRendererForGene(Actor* actor, unsigned int geneIndex, Mesh* sourceMesh);
    void ExpressActorGenetics(Actor* actor);
    
    // Current position of the player in the world
    glm::vec3 mPlayerPosition;
    
    // Distance beyond which the actors will no longer update
    float mActorUpdateDistance;
    
    // Maximum world water level
    float mWorldWaterLevel;
    
    
    // Threading
    std::thread* mActorSystemThread;
    std::mutex mux;
    
    // Object pools
    PoolAllocator<Actor> mActors;
    
    // List of active actors in the world
    std::vector<Actor*> mActiveActors;
    
};


#endif
