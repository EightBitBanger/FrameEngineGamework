#ifndef AI_ACTOR
#define AI_ACTOR

#include <GameEngineFramework/ActorAI/ActorStates.h>

#include <GameEngineFramework/ActorAI/Genetics/Gene.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>
#include <GameEngineFramework/ActorAI/Genetics/Phen.h>
#include <GameEngineFramework/ActorAI/Genetics/Bio.h>

#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Renderer/RenderSystem.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>
#include <mutex>

#include <string>


class ENGINE_API Actor {
    
public:
    
    friend class ActorSystem;
    friend class EngineSystemManager;
    friend class GeneticPresets;
    
    // Flags
    bool isGarbage;     // Mark for internal destruction and garbage collection
    
    // Name
    
    /// Set the name of the actor.
    void SetName(std::string newName);
    /// Get the name of the actor.
    std::string GetName(void);
    
    // State
    
    /// Set the active state of the actor.
    void SetActive(bool active);
    /// Get the active state of the actor.
    bool GetActive(void);
    
    // Flags
    
    /// Set the flag to update the actors genetics.
    void RebuildPhenotype(void);
    
    /// Set the flag to re-express the genome.
    void ReexpressPhenotype(void);
    
    /// Re-initiate the state of the actor. The mesh renderers will not be
    /// reset as they are owned and must be reset by the engine system.
    void Reset(void);
    
    
    class ENGINE_API NavigationSystem {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        /// Set the position of the actor.
        void SetPosition(glm::vec3 position);
        /// Get the position of the actor.
        glm::vec3 GetPosition(void);
        
        /// Set the position toward which the actor should point.
        void SetTargetPoint(glm::vec3 position);
        /// Get the position toward which the actor is pointing.
        glm::vec3 GetTargetPoint(void);
        
        /// Set the actor with which this actor should breed.
        Actor* GetBreedWithActor(void);
        /// Get the actor with which this actor should breed.
        void SetBreedWithActor(Actor* actorPtr);
        
        /// Set the target actor with which this actor should interact.
        Actor* GetTargetActor(void);
        /// Get the target actor with which this actor should interact.
        void SetTargetActor(Actor* actorPtr);
        
        NavigationSystem();
        
    private:
        
        glm::vec3 mVelocity;     // Movement vector
        glm::vec3 mPosition;     // Position of the actor in the world
        glm::vec3 mRotation;     // Direction the actor is facing
        glm::vec3 mLookAt;       // Direction the head is facing
        
        glm::vec3 mRotateTo;     // Where to rotate to
        glm::vec3 mTargetPoint;  // Point to face
        glm::vec3 mTargetLook;   // Point to look at
        
        Actor* mTargetActor;     // Target actor currently in focus
        
        std::mutex mux;
        
    } navigation;
    
    
    
    
    class ENGINE_API Behavior {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        /// Set the distance to which the actor can focus on another actor or player.
        void SetDistanceToFocus(float distance);
        /// Get the distance to which the actor can focus on another actor or player.
        float GetDistanceToFocus(void);
        
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
        
        /// Set the attack state.
        void SetPredatorState(bool state);
        /// Get the attack state.
        bool GetPredatorState(void);
        
        /// Set the prey state.
        void SetPreyState(bool state);
        /// Get the prey state.
        bool GetPreyState(void);
        
        Behavior();
        
    private:
        
        bool mIsPredator;           // True = can attack prey   False = herbivore
        bool mIsPrey;               // True = can be attacked   False = docile
        
        float mDistanceToFocus;     // Distance to focus on a near by actor
        float mDistanceToWalk;      // Distance to travel when moving to a random position
        float mDistanceToAttack;    // Distance to begin attacking a pray actor
        float mDistanceToFlee;      // Distance to begin fleeing from a predator actor
        
        float mHeightPreferenceMin; // Minimum world height this actor prefers to inhabit
        float mHeightPreferenceMax; // Maximum world height this actor prefers to inhabit
        
        // List of near by actors that are within focal range
        std::vector<Actor*> mProximityList;
        
        std::mutex mux;
        
    } behavior;
    
    
    
    
    class ENGINE_API State {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        // The current state the actor is in
        ActorState::Mode    mode;
        ActorState::State   current;
        
    private:
        
        bool mIsActive;
        bool mIsWalking;
        bool mIsRunning;
        bool mIsFacing;
        
        // List of animation states for each genetic component
        std::vector<glm::vec4> mAnimation;
        
        std::mutex mux;
        
        State();
        
    } state;
    
    
    
    
    class ENGINE_API IdiosyncraticCharacteristics {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        /// Add a memory to this actor.
        void Add(std::string name, std::string memory);
        /// Remove a memory from this actor.
        bool Remove(std::string name);
        
        /// Clear all the actor memories.
        void Clear(void);
        
        /// Check if a memory exists in this actor.
        bool CheckExists(std::string memory);
        
        IdiosyncraticCharacteristics();
        
    private:
        
        // List of memories collected by this entity
        std::unordered_map<std::string, std::string> mMemories;
        
        std::mutex mux;
        
    } memories;
    
    
    
    
    class ENGINE_API GeneticsSystem {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
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
        
        // Should the genetics be re constructed
        bool mDoUpdateGenetics;
        
        // Should the genetics be re-expressed
        bool mDoReexpressGenetics;
        
        // Current position in the generational sequence over time
        unsigned int mGeneration;
        
        // Genetic blueprints
        std::vector<Gene> mGenes;
        
        // Phonetic expression
        std::vector<Phen> mPhen;
        
        // List of render components representing genetic expression
        std::vector<MeshRenderer*> mGeneticRenderers;
        
        std::mutex mux;
        
    } genetics;
    
    
    
    
    class ENGINE_API BiologicalSystem {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        float health;
        float hunger;
        float immunity;
        float energy;
        float stress;
        
        BiologicalSystem();
        
    private:
        
        // Biological elements
        std::vector<Bio> mBiologics;
        
        std::mutex mux;
        
    } biological;
    
    
    
    
    class ENGINE_API PhysicalAttributes {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        /// Set the age of the actor.
        void SetAge(unsigned long int newAge);
        /// Get the age of the actor.
        unsigned long int GetAge(void);
        
        /// Set the senior age of the actor.
        void SetSeniorAge(float oldAge);
        /// Get the senior age of the actor.
        float GetSeniorAge(void);
        
        /// Set the age at which the actor will begin adulthood.
        void SetAdultAge(float age);
        /// Get the age at which the actor will begin adulthood.
        float GetAdultAge(void);
        
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
        
        /// Set the initial scale for the actor.
        void SetYouthScale(float scale);
        /// Get the initial scale from the actor.
        float GetYouthScale(void);
        
        /// Set the max adult scale for the actor.
        void SetAdultScale(float scale);
        /// Get the max adult scale from the actor.
        float GetAdultScale(void);
        
        // Sexual orientation
        
        /// Set the sexual orientation for reproduction.
        void SetSexualOrientation(bool orientation);
        
        /// Get the sexual orientation for reproduction.
        bool GetSexualOrientation(void);
        
        PhysicalAttributes();
        
    private:
        
        // Number of ticks this actor has accumulated in its lifetime
        unsigned long int mAge;
        
        // Age at which the actor is able to reproduce.
        float mAgeAdult;
        
        // Maximum Age at which the actor is considered a senior.
        float mAgeSenior;
        
        // Maximum speed this actor can travel
        float mSpeed;
        
        // Maximum speed this actor can travel as a youth
        float mSpeedYouth;
        
        // Running speed multiplier
        float mSpeedMul;
        
        // Rotation speed when changing directions
        float mSnapSpeed;
        
        // Initial scale for this actor
        float mYouthScale;
        
        // Max scale for adult actor
        float mAdultScale;
        
        // Reproductive orientation  1=Male 0=Female
        bool mSexualOrientation;
        
        // Collider position offset
        glm::vec3 mColliderOffset;
        
        // Collider scale
        glm::vec3 mColliderScale;
        
        std::mutex mux;
        
    } physical;
    
    
    
    
    class ENGINE_API CooldownCounters {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
        // Cool down counters
        
        /// Set the current cool down counter for actor breeding.
        void SetCoolDownBreeding(unsigned int counter);
        
        /// Get the current cool down counter for actor breeding.
        unsigned int GetCoolDownBreeding(void);
        
        CooldownCounters();
        
    private:
        
        // Prevents actor from hyper focusing on its surroundings
        // This will stop the actor from acting spastic
        unsigned int mObservationCoolDownCounter;
        
        // Prevents actor movement lock when outside the prefered height range
        unsigned int mMovementCoolDownCounter;
        
        // Prevents over breeding
        unsigned int mBreedingCoolDownCounter;
        
        std::mutex mux;
        
    } counters;
    
    
    
    
    class ENGINE_API UserVariables {
        
        friend class Actor;
        friend class ActorSystem;
        friend class EngineSystemManager;
        friend class GeneticPresets;
        
    public:
        
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
        
        UserVariables();
        
    private:
        
        // User bit mask byte
        uint8_t mBitmask;
        
        // User data pointers
        void* mUserDataA;
        void* mUserDataB;
        
        std::mutex mux;
        
    } user;
    
    
    Actor();
    
private:
    
    // Actor name string
    std::string mName;
    
};


#endif
