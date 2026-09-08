#ifndef AI_ACTOR_NAVIGATION
#define AI_ACTOR_NAVIGATION

#include <GameEngineFramework/configuration.h>
class Actor;

class ENGINE_API NavigationSystem {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
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
    void SetBreedWithActor(Actor* actorPtr);
    /// Get the actor with which this actor should breed.
    Actor* GetBreedWithActor(void);
    
    /// Set the target actor with which this actor should interact.
    void SetTargetActor(Actor* actorPtr);
    /// Get the target actor with which this actor should interact.
    Actor* GetTargetActor(void);
    
    /// Set the velocity vector.
    void SetVelocity(const glm::vec3& velocity);
    /// Get the velocity vector.
    glm::vec3 GetVelocity(void);
    
    /// Get the distance to the currently focused actor.
    float GetDistanceToTarget(void);
    
    NavigationSystem();
    
    void Reset();
    
private:
    
    glm::vec3 mVelocity;     // Rate of movement
    glm::vec3 mPosition;     // Position of the actor in the world
    glm::vec3 mRotation;     // Direction the actor is currently facing
    glm::vec3 mFacing;       // Direction the head is currently facing
    glm::vec3 mLookAt;       // Direction the head should be facing
    glm::vec3 mRotateTo;     // Directory the actor should be facing
    
    glm::vec3 mTargetPoint;  // Point to face
    glm::vec3 mTargetLook;   // Point to look at
    
    float mDistanceToTarget; // Distance to the target actor object
    
    Actor* mTargetActor;     // Target actor currently in focus
    
    // List of static objects in the area
    std::vector<std::pair<std::string, glm::vec3>> mQueryStatic;
    
    // List of points to determine height
    // Height is updated on another thread
    std::vector<glm::vec3> mQueryPoints;
};

#endif 
