#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include <GameEngineFramework/ActorAI/NeuralNetwork.h>

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

enum class NeuralState {
    
    idle,
    move_to,
    
    avoid,
    dodge,
    
    attack,
    follow,
    
    defend,
    flee,
    
    interact,
    
};


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
    
private:
    
    // Master update timer
    Timer mMainTimer;
    // Animation update timer
    Timer mAnimationTimer;
    
    //
    // Behavioral state update
    void UpdateActorState(Actor* actor);
    
    void HandleWalkingChance(Actor* actor, bool isAquatic);
    void HandleFocusOnNearbyActor(Actor* actor);
    bool HandleBreedingState(Actor* actor);
    void HandleNeuralNetwork(Actor* actor);
    
    void HandleMovementHeight(Actor* actor, bool isAquatic);
    
    void UpdateTargetPoint(Actor* actor);
    
    void CheckTargetReached(Actor* actor);
    
    //
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
    
    // Target rotation animation
    void UpdateTargetRotation(Actor* actor);
    
    
    // Neural network encoding
    std::vector<float> Encode(const NeuralState neuralState);
    
    
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
    
};


#endif
