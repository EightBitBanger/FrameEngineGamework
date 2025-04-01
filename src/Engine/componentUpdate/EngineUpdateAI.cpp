#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActor(unsigned int index) {
    
    UpdateActorGenetics(index);
    ExpressActorGenetics(index);
    UpdateActorTargetRotation(index);
    UpdateActorAnimation(index);
    UpdateActorPhysics(index);
    
    if (mStreamBuffer[index].actor->physical.mAge % 10 == 1) 
        mStreamBuffer[index].actor->ReexpressPhenotype();
    
    // Check walking state
    if (mStreamBuffer[index].actor->state.mIsWalking) {
        
        // Apply forward velocity
        glm::vec3 forward;
        
        forward.x = cos( glm::radians( -(mStreamBuffer[index].actor->navigation.mRotation.y - 90.0f) ) );
        // TODO: Actors should fly???
        //forward.y = tan( glm::radians( -(mStreamBuffer[index].actor->mRotation.x - 90) ) );
        forward.z = sin( glm::radians( -(mStreamBuffer[index].actor->navigation.mRotation.y - 90) ) );
        
        float actorSpeed = mStreamBuffer[index].actor->physical.mSpeed;
        
        glm::vec3 actorVelocity = forward * (actorSpeed * 0.1f) * 0.1f;
        
        mStreamBuffer[index].actor->navigation.mVelocity.x = actorVelocity.x;
        mStreamBuffer[index].actor->navigation.mVelocity.z = actorVelocity.z;
        
        return;
    }
    
    // Stop moving but keep falling
    mStreamBuffer[index].actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
    
    return;
}

