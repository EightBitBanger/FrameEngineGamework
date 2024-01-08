#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::ShadowPass(MeshRenderer* currentEntity, glm::vec3& eye, glm::mat4& viewProjection) {
    
    // Strip out model rotation to prevent shadow rotation
    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    modelMatrix = glm::translate(modelMatrix, currentEntity->transform.position);
    modelMatrix = glm::scale(modelMatrix, currentEntity->transform.localScale);
    
    shaders.shadowCaster->SetProjectionMatrix( viewProjection );
    shaders.shadowCaster->SetCameraPosition(eye);
    shaders.shadowCaster->SetModelMatrix( modelMatrix );
    
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable( GL_CULL_FACE );
    
    // Calculate shadow angle
    
    for (int s=0; s < mNumberOfShadows; s++) {
        
        float shadowRayScale = 1.3;
        float shadowLength   = currentEntity->material->shadowStencilLength;
        
        mShadowTransform.SetIdentity();
        
        // Initial angle rotation
        mShadowTransform.RotateAxis( 1, glm::vec3(0, -1, 0) );
        
        // Rotate by the inverse light angle
        mShadowTransform.RotateWorldAxis( -180, mShadowDirection[s], Vector3(0, 0, 0) );
        
        // Scale the length of the shadow
        mShadowTransform.Scale( glm::vec3(shadowRayScale, shadowLength * 2, shadowRayScale) );
        
        // Offset by half the distance
        mShadowTransform.Translate( glm::vec3(0, -1, 0) * .5f );
        
        glm::vec3 shadowPosition[1];
        glm::vec3 shadowDirection[1];
        glm::vec4 shadowAttenuation[1];
        glm::vec3 shadowColor[1];
        
        shadowPosition[0]     = mShadowPosition[s];
        shadowDirection[0]    = mShadowDirection[s];
        shadowAttenuation[0]  = mShadowAttenuation[s];
        shadowColor[0]        = mShadowColor[s];
        
        // Send in the shadow data through the lighting parameters
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
    
    
    if (mCurrentMaterial->doBlending) {
        
        glEnable( GL_BLEND );
        glBlendFunc(mCurrentMaterial->blendSource, mCurrentMaterial->blendDestination);
        
    } else {
        
        glDisable( GL_BLEND );
        
    }
    
    
    if (mCurrentMaterial->doFaceCulling) {
        
        glEnable( GL_CULL_FACE );
        
    } else {
        
        glDisable( GL_CULL_FACE );
        
    }
    
    return;
}

