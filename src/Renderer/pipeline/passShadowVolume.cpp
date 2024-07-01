#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::ShadowVolumePass(MeshRenderer* currentEntity, glm::vec3& eye, glm::vec3 cameraAngle, glm::mat4& viewProjection) {
    
    if (!currentEntity->isActive) 
        return false;
    
    if (!currentEntity->material->mDoShadowPass)
        return false;
    
    // Calculate shadow distance
    float shadowDistance = glm::distance( eye, currentEntity->transform.position );
    
    if (shadowDistance > mShadowDistance) 
        return false;
    
    // Strip out model rotation to prevent shadow rotation
    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    modelMatrix = glm::translate(modelMatrix, currentEntity->transform.position);
    modelMatrix = glm::scale(modelMatrix, currentEntity->transform.scale);
    
    shaders.shadowCaster->SetProjectionMatrix( viewProjection );
    shaders.shadowCaster->SetModelMatrix( modelMatrix );
    shaders.shadowCaster->SetCameraPosition(eye);
    shaders.shadowCaster->SetCameraAngle(cameraAngle);
    
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable( GL_CULL_FACE );
    
    for (unsigned int s=0; s < mNumberOfShadows; s++) {
        
        float shadowLength = currentEntity->material->mShadowVolumeLength;
        
        mShadowTransform.SetIdentity();
        
        mShadowTransform.RotateWorldAxis( 180, mShadowDirection[s], Vector3(0, 0, 0) );
        
        // Rotate by the inverse light angle
        glm::vec3 angles = currentEntity->transform.EulerAngles();
        
        mShadowTransform.RotateWorldAxis( angles.x, glm::vec3(1, 0, 0), Vector3(0, 0, 0) );
        mShadowTransform.RotateWorldAxis( angles.y, glm::vec3(0, 1, 0), Vector3(0, 0, 0) );
        mShadowTransform.RotateWorldAxis( angles.z, glm::vec3(0, 0, 1), Vector3(0, 0, 0) );
        
        // Offset by half the distance
        glm::vec3 shadowTranslation = (glm::vec3(0, -1, 0) * shadowLength);
        mShadowTransform.Translate( shadowTranslation );
        
        // Scale the length of the shadow
        mShadowTransform.Scale( glm::vec3(1, shadowLength * 2, 1) );
        
        glm::vec3 shadowPosition[1];
        glm::vec3 shadowDirection[1];
        glm::vec4 shadowAttenuation[1];
        glm::vec3 shadowColor[1];
        
        shadowPosition[0]   = mShadowPosition[s];
        shadowDirection[0]  = mShadowDirection[s];
        
        // Shadow color
        shadowColor[0] = glm::vec3(currentEntity->material->mShadowVolumeColor.r, 
                                   currentEntity->material->mShadowVolumeColor.g, 
                                   currentEntity->material->mShadowVolumeColor.b);
        
        // Shadow intensity
        shadowAttenuation[0].r = currentEntity->material->mShadowVolumeAngleOfView;
        shadowAttenuation[0].g = currentEntity->material->mShadowVolumeColorIntensity;
        shadowAttenuation[0].b = currentEntity->material->mShadowVolumeIntensityHigh  * 0.1;
        shadowAttenuation[0].a = currentEntity->material->mShadowVolumeIntensityLow   * 0.1;
        
        // Send in the shadow data through the lighting parameters for this pass
        shaders.shadowCaster->SetLightCount(1);
        shaders.shadowCaster->SetLightPositions(1,   shadowPosition);
        shaders.shadowCaster->SetLightDirections(1,  shadowDirection);
        shaders.shadowCaster->SetLightAttenuation(1, shadowAttenuation);
        shaders.shadowCaster->SetLightColors(1,      shadowColor);
        
        shaders.shadowCaster->SetShadowMatrix( mShadowTransform.matrix );
        
        // Render the shadow pass
        mNumberOfDrawCalls++;
        currentEntity->mesh->DrawIndexArray();
        
        continue;
    }
    
    
    if (mCurrentMaterial->mDoBlending) {
        
        glEnable( GL_BLEND );
        glBlendFunc(mCurrentMaterial->mBlendSource, mCurrentMaterial->mBlendDestination);
        
    } else {
        
        glDisable( GL_BLEND );
        
    }
    
    
    if (mCurrentMaterial->mDoFaceCulling) {
        
        glEnable( GL_CULL_FACE );
        
    } else {
        
        glDisable( GL_CULL_FACE );
        
    }
    
    return true;
}

