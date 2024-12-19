#ifndef AI_ACTOR
#define AI_ACTOR

#include <GameEngineFramework/ActorAI/ActorStates.h>

#include <GameEngineFramework/ActorAI/Genetics/Gene.h>
#include <GameEngineFramework/ActorAI/neurons/NeuralLayer.h>
#include <GameEngineFramework/ActorAI/neurons/WeightedLayer.h>

#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Renderer/RenderSystem.h>

#include <glm/glm.hpp>

#include <vector>
#include <mutex>

#include <string>


class ENGINE_API Actor {
    
public:
    
    friend class ActorSystem;
    friend class EngineSystemManager;
    friend class GeneticPresets;
    
    /// Set the name of the actor.
    void SetName(std::string newName);
    
    /// Get the name of the actor.
    std::string GetName(void);
    
    /// Set the active state of the actor.
    void SetActive(bool state);
    
    /// Get the active state of the actor.
    bool GetActive(void);
    
    /// Set the age of the actor.
    void SetAge(unsigned long long int newAge);
    
    /// Get the age of the actor.
    unsigned long long int GetAge(void);
    
    /// Set the generation of the actor.
    void SetGeneration(unsigned int newAge);
    
    /// Set the generation of the actor.
    unsigned int GetGeneration(void);
    
    /// Set the walking speed of the actor.
    void SetSpeed(float newSpeed);
    
    /// Get the walking speed of the actor.
    float GetSpeed(void);
    
    /// Set the speed the actor can run at as a youth.
    void SetSpeedYouth(float speed);
    
    /// Get the speed the actor can run at as a youth.
    float GetSpeedYouth(void);
    
    /// Set the running speed multiplier of the actor.
    void SetSpeedMultiplier(float newSpeedMul);
    
    /// Get the running speed multiplier of the actor.
    float GetSpeedMultiplier(void);
    
    /// Set the position of the actor.
    void SetPosition(glm::vec3 position);
    
    /// Get the position of the actor.
    glm::vec3 GetPosition(void);
    
    /// Set the position toward which the actor should point.
    void SetTargetPoint(glm::vec3 position);
    
    /// Set the flag to update the actors genetics.
    void SetGeneticUpdateFlag(void);
    
    /// Set the flag to re-express the genome.
    void SetGeneticExpressionFlag(void);
    
    /// Get the number of mesh renderers associated with this actor.
    unsigned int GetNumberOfMeshRenderers(void);
    
    /// Return a mesh renderer at the given index.
    MeshRenderer* GetMeshRendererAtIndex(unsigned int index);
    
    /// Set the user bit mask byte.
    void SetUserBitmask(uint8_t bitmask);
    
    /// Get the user bit mask byte.
    uint8_t GetUserBitmask(void);
    
    /// Set the user data pointer.
    void SetUserDataA(void* ptr);
    
    /// Get the user data pointer.
    void* GetUserDataA(void);
    
    /// Set the user data pointer.
    void SetUserDataB(void* ptr);
    
    /// Get the user data pointer.
    void* GetUserDataB(void);
    
    // Genetics
    
    /// Add a new gene to the actors genome. The index location will be returned.
    unsigned int AddGene(Gene& newGene);
    
    /// Remove a gene from the actors genome.
    void RemoveGene(unsigned int index);
    
    /// Clear all the genes in this actors genome.
    void ClearGenome(void);
    
    /// Get the number of genes in the genome.
    unsigned int GetNumberOfGenes(void);
    
    /// Get a gene from the genome.
    Gene GetGeneFromGenome(unsigned int index);
    
    // Neural networking
    
    /// Add a weighted layer to the neural network.
    void AddWeightedLayer(WeightedLayer& newNeuralLayer);
    
    /// Remove a weighted layer from the neural network by the given index location.
    void RemoveWeightedLayer(unsigned int index);
    
    /// Clear neural layers.
    void ClearWeightedLayers(void);
    
    /// Get the number of weighted layers in the neural network.
    unsigned int GetNumberOfWeightedLayers(void);
    
    /// Get a weighted layer from the neural network.
    WeightedLayer GetWeightedLayerFromNetwork(unsigned int index);
    
    /// Set the input data to be fed through the neural network.
    void SetNeuralInputLayer(NeuralLayer inputLayer);
    
    // AI state behavioral hardwiring
    
    /// Set the chance for the actor to change direction.
    void SetChanceToChangeDirection(float chance);
    
    /// Get the chance for the actor to change direction.
    float GetChanceToChangeDirection(void);
    
    /// Set the chance to focus on a nearby actor.
    void SetChanceToFocusOnActor(float chance);
    
    /// Get the chance to focus on a nearby actor.
    float GetChanceToFocusOnActor(void);
    
    /// Set the chance to begin walking.
    void SetChanceToWalk(float chance);
    
    /// Get the chance to begin walking.
    float GetChanceToWalk(void);
    
    /// Set the chance to stop walking.
    void SetChanceToStopWalking(float chance);
    
    /// Get the chance to stop walking.
    float GetChanceToStopWalking(void);
    
    /// Set the distance to walk when selecting new random positions.
    void SetDistanceToWalk(float distance);
    
    /// Get the distance to walk when selecting new random positions.
    float GetDistanceToWalk(void);
    
    /// Set the distance to begin attacking a pray actor if any are nearby.
    void SetDistanceToAttack(float distance);
    
    /// Get the distance to begin attacking a pray actor if any are nearby.
    float GetDistanceToAttack(void);
    
    /// Set the distance to begin running from a predator actor if any are nearby.
    void SetDistanceToFlee(float distance);
    
    /// Get the distance to begin running from a predator actor if any are nearby.
    float GetDistanceToFlee(void);
    
    /// Set the minimum height preference when traveling.
    void SetHeightPreferenceMin(float height);
    
    /// Get the minimum height preference when traveling.
    float GetHeightPreferenceMin(void);
    
    /// Set the maximum height preference when traveling.
    void SetHeightPreferenceMax(float height);
    
    /// Get the maximum height preference when traveling.
    float GetHeightPreferenceMax(void);
    
    // Memories
    
    /// Add a memory to this actor.
    void AddMemory(std::string memory);
    
    /// Remove a memory from this actor.
    bool RemoveMemory(std::string memory);
    
    /// Clear all the actor memories.
    void ClearMemories(void);
    
    /// Check if a memory exists in this actor.
    bool CheckMemoryExists(std::string memory);
    
    // Age scaling
    
    /// Set the initial scale for the actor.
    void SetYouthScale(float scale);
    /// Get the initial scale from the actor.
    float GetYouthScale(void);
    
    /// Set the max adult scale for the actor.
    void SetAdultScale(float scale);
    /// Get the max adult scale from the actor.
    float GetAdultScale(void);
    
    Actor();
    
private:
    
    // Actor name string
    std::string mName;
    
    // Number of ticks this actor has accumulated in its lifetime
    unsigned long long int mAge;
    
    // Current position in the generational sequence over time
    unsigned int mGeneration;
    
    // Initial scale for this actor
    float mYouthScale;
    
    // Max scale for adult actor
    float mAdultScale;
    
    // Collider position offset
    glm::vec3 mColliderOffset;
    
    // Collider scale
    glm::vec3 mColliderScale;
    
    // Maximum speed this actor can travel
    float mSpeed;
    
    // Maximum speed this actor can travel as a youth
    float mSpeedYouth;
    
    // Running speed multiplier
    float mSpeedMul;
    
    // Rotation speed when changing directions
    float mSnapSpeed;
    
    
    // Movement vector
    glm::vec3 mVelocity;
    
    // Position of the actor in the world
    glm::vec3 mPosition;
    
    // Forward facing direction of the actor in the world
    glm::vec3 mRotation;
    
    // Direction toward which the actor should face
    glm::vec3 mRotateTo;
    
    // World target point toward which the actor should face
    glm::vec3 mTargetPoint;
    
    // Distance to the player
    float mDistance;
    
    //
    // States
    
    // Is this actor active in the simulation
    bool mIsActive;
    
    // Should the genetics be re constructed
    bool mDoUpdateGenetics;
    
    // Should the genetics be re-expressed
    bool mDoReexpressGenetics;
    
    
    // Is the actor walking
    bool mIsWalking;
    
    // Is the actor running
    bool mIsRunning;
    
    // Is the actor attacking another actor
    bool mIsAttacking;
	
	// Is the actor fleeing from a predator
	bool mIsFleeing;
	
	// Is the actor consuming a food source
	bool mIsConsuming;
	
	// Should the actor face toward or away from the target point
	bool mIsFacing;
	
	
    // Chance to suddenly change direction
    float mChanceToChangeDirection;
	
	// Chance to focus on a near by actor
	float mChanceToFocusOnActor;
	
	// Chance to begin walking
	float mChanceToWalk;
    
    // Chance to suddenly stop moving
    float mChanceToStopWalking;
	
	
    // Distance to focus on a near by actor
	float mDistanceToFocusOnActor;
	
	// Distance to travel when moving to a random target position
	float mDistanceToWalk;
    
    // Distance to begin attacking a pray actor
	float mDistanceToAttack;
	
    // Distance to begin fleeing from a predator actor
	float mDistanceToFlee;
    
    
    // Minimum world height this actor prefers to inhabit
	float mHeightPreferenceMin;
	
	// Maximum world height this actor prefers to inhabit
	float mHeightPreferenceMax;
    
    
    // Cool down period before focusing on another actor
    unsigned int mObservationCoolDownCounter;
    
    
    // Is this actor loaded into the scene
    bool mIsActorActiveInScene;
    
    // User bit mask byte
    uint8_t mBitmask;
    
    // User data pointers
    void* mUserDataA;
    void* mUserDataB;
    
    // List of actor names considered to be pray actors
    std::vector<std::string> mAttackActors;
    
    // List of actor names considered to be predator actors
    std::vector<std::string> mFleeFromActors;
    
    // List of memories collected by this entity
    std::vector<std::string> mMemories;
    
    // Genetic expression
    std::vector<Gene> mGenes;
    
    // List of render components representing genetic expression
    std::vector<MeshRenderer*> mGeneticRenderers;
    
    // List of animation states for each genetic component
    std::vector<glm::vec4> mAnimationStates;
    
    // Layers of weighted neurological expression
    std::vector<WeightedLayer> mWeightedLayers;
    
    std::mutex mux;
    
};





#endif
