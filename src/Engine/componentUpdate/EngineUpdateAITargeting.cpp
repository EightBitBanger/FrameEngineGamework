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


void EngineSystemManager::UpdateActorTargetRotation(unsigned int index) {
    
    // Face toward target point
    glm::vec3 position = mStreamBuffer[index].actor->mPosition;
    
    float xx = position.x - mStreamBuffer[index].actor->mTargetPoint.x;
    float zz = position.z - mStreamBuffer[index].actor->mTargetPoint.z;
    
    mStreamBuffer[index].actor->mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check to invert facing direction
    if (!mStreamBuffer[index].actor->mIsFacing) {
        
        mStreamBuffer[index].actor->mRotateTo.y += 180;
        
        if (mStreamBuffer[index].actor->mRotateTo.y > 360) 
            mStreamBuffer[index].actor->mRotateTo.y -= 360;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    if (mStreamBuffer[index].actor->mRotation.y < 90) 
        if (mStreamBuffer[index].actor->mRotateTo.y > 270) 
            mStreamBuffer[index].actor->mRotation.y += 360;
    
    if (mStreamBuffer[index].actor->mRotation.y > 270) 
        if (mStreamBuffer[index].actor->mRotateTo.y < 90) 
            mStreamBuffer[index].actor->mRotation.y -= 360;
    
    // Rotate actor toward the focal point
    if (mStreamBuffer[index].actor->mRotation != mStreamBuffer[index].actor->mRotateTo) {
        
        glm::vec3 fadeValue( mStreamBuffer[index].actor->mRotation );
        glm::vec3 fadeTo   ( mStreamBuffer[index].actor->mRotateTo );
        
        fadeValue.x = Math.Lerp(fadeValue.x, fadeTo.x, mStreamBuffer[index].actor->mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeValue.y, fadeTo.y, mStreamBuffer[index].actor->mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeValue.z, fadeTo.z, mStreamBuffer[index].actor->mSnapSpeed);
        
        mStreamBuffer[index].actor->mRotation = fadeValue;
    }
    
    return;
}

