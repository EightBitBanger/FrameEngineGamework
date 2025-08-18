#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::GeometryPass(MeshRenderer* currentEntity, glm::vec3& eye, glm::vec3& cameraAngle, glm::mat4& viewProjection) {
    // Mesh binding
    
    // Level of detail shift selection
    Mesh* mesh = LevelOfDetailPass(currentEntity, eye);
    if (mesh == nullptr) 
        return false;
    
    BindMesh( mesh );
    
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
    
    // Render the geometry
    mesh->DrawIndexArray();
    mNumberOfDrawCalls++;
    
    return true;
}

