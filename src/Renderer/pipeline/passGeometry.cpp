#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::GeometryPass(MeshRenderer* currentEntity, glm::vec3& eye, glm::vec3 cameraAngle, glm::mat4& viewProjection) {
    
    if (!currentEntity->isActive) 
        return false;
    
    // Mesh binding
    
    Mesh* meshPtr = currentEntity->mesh;
    
    if (meshPtr == nullptr) 
        return false;
    
    BindMesh( meshPtr );
    
    // Material binding
    
    Material* materialPtr = currentEntity->material;
    
    if (materialPtr == nullptr) 
        return false;
    
    BindMaterial( materialPtr );
    
    // Shader binding
    
    Shader* shaderPtr = materialPtr->shader;
    
    if (shaderPtr == nullptr) 
        return false;
    
    BindShader( shaderPtr );
    
    // Set the projection
    
    mCurrentShader->SetProjectionMatrix( viewProjection );
    mCurrentShader->SetModelMatrix( currentEntity->transform.matrix );
    
    // Inverse transpose model matrix for lighting with non linear scaling
    glm::mat3 invTransposeMatrix = glm::transpose( glm::inverse( currentEntity->transform.matrix ) );
    
    mCurrentShader->SetInverseModelMatrix( invTransposeMatrix );
    
    mCurrentShader->SetCameraPosition(eye);
    mCurrentShader->SetCameraAngle(cameraAngle);
    
    // Set the material
    mCurrentShader->SetMaterialAmbient(mCurrentMaterial->ambient);
    mCurrentShader->SetMaterialDiffuse(mCurrentMaterial->diffuse);
    mCurrentShader->SetMaterialSpecular(mCurrentMaterial->specular);
    
    // Fog layers
    for (unsigned int i=0; i < 4; i++) {
        
        if (!fogActive[i]) {
            
            // Blank the fog values
            
            mCurrentShader->SetFogDensity(i, 0.0f);
            
            mCurrentShader->SetFogHeightCutoff(i, 1000000.0f);
            
            Color blank(0, 0, 0);
            mCurrentShader->SetFogColorBegin(i, blank);
            mCurrentShader->SetFogColorEnd(i, blank);
            
            mCurrentShader->SetFogBegin(i, 0.0f);
            mCurrentShader->SetFogEnd(i, 0.0f);
            
            continue;
        }
        
        mCurrentShader->SetFogDensity(i, fogDensity[i]);
        
        mCurrentShader->SetFogHeightCutoff(i, fogHeightCutoff[i]);
        
        mCurrentShader->SetFogColorBegin(i, fogColorBegin[i]);
        mCurrentShader->SetFogColorEnd(i, fogColorEnd[i]);
        
        mCurrentShader->SetFogBegin(i, fogBegin[i]);
        mCurrentShader->SetFogEnd(i, fogEnd[i]);
        
    }
    
    
    // Render the geometry
    currentEntity->mesh->DrawIndexArray();
    mNumberOfDrawCalls++;
    
    return true;
}

