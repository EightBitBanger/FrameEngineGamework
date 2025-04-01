#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorTargetRotation(unsigned int index) {
    
    // Face toward target point
    glm::vec3 position = mStreamBuffer[index].actor->navigation.mPosition;
    
    float xx = position.x - mStreamBuffer[index].actor->navigation.mTargetPoint.x;
    float zz = position.z - mStreamBuffer[index].actor->navigation.mTargetPoint.z;
    
    mStreamBuffer[index].actor->navigation.mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check to invert facing direction
    if (!mStreamBuffer[index].actor->state.mIsFacing) {
        
        mStreamBuffer[index].actor->navigation.mRotateTo.y += 180;
        
        if (mStreamBuffer[index].actor->navigation.mRotateTo.y > 360) 
            mStreamBuffer[index].actor->navigation.mRotateTo.y -= 360;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    if (mStreamBuffer[index].actor->navigation.mRotation.y < 90) 
        if (mStreamBuffer[index].actor->navigation.mRotateTo.y > 270) 
            mStreamBuffer[index].actor->navigation.mRotation.y += 360;
    
    if (mStreamBuffer[index].actor->navigation.mRotation.y > 270) 
        if (mStreamBuffer[index].actor->navigation.mRotateTo.y < 90) 
            mStreamBuffer[index].actor->navigation.mRotation.y -= 360;
    
    // Rotate actor toward the focal point
    if (mStreamBuffer[index].actor->navigation.mRotation != mStreamBuffer[index].actor->navigation.mRotateTo) {
        
        glm::vec3 fadeValue( mStreamBuffer[index].actor->navigation.mRotation );
        glm::vec3 fadeTo   ( mStreamBuffer[index].actor->navigation.mRotateTo );
        
        fadeValue.x = Math.Lerp(fadeValue.x, fadeTo.x, mStreamBuffer[index].actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeValue.y, fadeTo.y, mStreamBuffer[index].actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeValue.z, fadeTo.z, mStreamBuffer[index].actor->physical.mSnapSpeed);
        
        mStreamBuffer[index].actor->navigation.mRotation = fadeValue;
    }
    
    return;
}

