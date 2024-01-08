#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::GeometryPass(MeshRenderer* currentEntity, glm::vec3& eye, glm::mat4& viewProjection) {
    
    // Mesh binding
    
    Mesh* meshPtr = currentEntity->mesh;
    
    if (meshPtr == nullptr) 
        return;
    
    BindMesh( meshPtr );
    
    // Material binding
    
    Material* materialPtr = currentEntity->material;
    
    if (materialPtr == nullptr) 
        return;
    
    BindMaterial( materialPtr );
    
    // Shader binding
    
    Shader* shaderPtr = materialPtr->shader;
    
    if (shaderPtr == nullptr) 
        return;
    
    BindShader( shaderPtr );
    
    // Set the projection
    
    mCurrentShader->SetProjectionMatrix( viewProjection );
    mCurrentShader->SetModelMatrix( currentEntity->transform.matrix );
    
    // Inverse transpose model matrix for lighting with non linear scaling
    glm::mat3 invTransposeMatrix = glm::transpose( glm::inverse( currentEntity->transform.matrix ) );
    
    mCurrentShader->SetInverseModelMatrix( invTransposeMatrix );
    
    mCurrentShader->SetCameraPosition(eye);
    
    // Set the material and texture
    mCurrentShader->SetMaterialAmbient(mCurrentMaterial->ambient);
    mCurrentShader->SetMaterialDiffuse(mCurrentMaterial->diffuse);
    mCurrentShader->SetMaterialSpecular(mCurrentMaterial->specular);
    
    return;
}

