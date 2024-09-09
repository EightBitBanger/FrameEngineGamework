//
// Update actor components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer     Platform;



void EngineSystemManager::UpdateActorPhysics(unsigned int index) {
    
    //
    // Update actor physics
    
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
    
    glm::vec3 actorPosition = mStreamBuffer[index].transform->position;
    glm::vec3 actorRotation = mStreamBuffer[index].actor->mRotation;
    glm::vec3 actorVelocity = mStreamBuffer[index].actor->mVelocity;
    
    // Ray cast here
    glm::vec3 from      = actorPosition;
    glm::vec3 direction = glm::vec3(0, -1, 0);
    from.y = 900;
    
    Hit hit;
    
    float distance = 1000;
    
    // Move the actor out of the way since we cant cast a ray from inside the collider...
    rp3d::Transform transform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    currentPosition.y += 1000;
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Check not on ground
    if (!Physics.Raycast(from, direction, distance, hit)) {
        
        // Apply some falling action
        actorVelocity.y -= 0.01;
        
        // Terminal velocity
        if (actorVelocity.y < -1) 
            actorVelocity.y = -1;
        
    } else {
        
        // Standing on ground
        actorPosition.y   = hit.point.y;
        currentPosition.y = hit.point.y + 1000;
        
        actorVelocity.y = 0;
        
    }
    
    // Move the actor back into position since we are finished casting rays...
    currentPosition.y -= 1000;
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Apply force velocity
    mStreamBuffer[index].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, 
                                                                                 actorVelocity.y, 
                                                                                 actorVelocity.z) );
    
    // Sync actor position
    mStreamBuffer[index].actor->mPosition = actorPosition;
    mStreamBuffer[index].actor->mRotation = actorRotation;
    mStreamBuffer[index].actor->mVelocity = actorVelocity;
    
    return;
}

