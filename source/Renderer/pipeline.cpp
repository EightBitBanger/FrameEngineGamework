#include "rendersystem.h"


void RenderSystem::RenderFrame(float deltaTime) {
    
    if (cameraMain == nullptr) 
        return;
    
    // Clear the screen
    
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    if (skyMain != nullptr) {
        Color& color = skyMain->background;
        glClearColor(color.r, color.g, color.b, 1);
    }
    
    
    // Synchronize attached rigid body transform
    
    if (cameraMain->useMouseLook) 
        cameraMain->MouseLook(deltaTime, displayCenter.x, displayCenter.y);
    
    glm::mat4 projection = glm::perspective( glm::radians( cameraMain->fov ), cameraMain->aspect, cameraMain->clipNear, cameraMain->clipFar);
    
    
    // Calculate viewing angle
    glm::vec3 pos;
    pos.x = cameraMain->transform.position.x;
    pos.y = cameraMain->transform.position.y;
    pos.z = cameraMain->transform.position.z;
    
    // Looking angle
    cameraMain->forward.x = cos( cameraMain->transform.rotation.x * 180 / glm::pi<float>() );
    cameraMain->forward.y = tan( cameraMain->transform.rotation.y * 180 / glm::pi<float>() );
    cameraMain->forward.z = sin( cameraMain->transform.rotation.x * 180 / glm::pi<float>() );
    
    cameraMain->forward = glm::normalize(cameraMain->forward);
    
    // Right angle to the looking angle
    glm::vec3 angle;
    angle.x = cameraMain->transform.position.x + cameraMain->forward.x;
    angle.y = cameraMain->transform.position.y + cameraMain->forward.y;
    angle.z = cameraMain->transform.position.z + cameraMain->forward.z;
    
    cameraMain->right = glm::normalize(glm::cross(cameraMain->up, cameraMain->forward));
    glm::mat4 view = glm::lookAt(pos, angle, cameraMain->up);
    
    
    // Trip the shader to reset the camera projection
    currentShader = nullptr;
    
    
    
    // Draw entity meshes
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        unsigned int entityListSz = scenePtr->GetRenderQueueSize();
        
        for (unsigned int i=0; i < entityListSz; i++) {
            
            Entity* currentEntity = scenePtr->GetEntity(i);
            
            Mesh* mesh = currentEntity->GetAttachedMesh();
            if (mesh == nullptr) continue;
            
            Shader* shader = mesh->GetAttachedShader();
            if (shader == nullptr) continue;
            
            // Mesh binding
            if (currentMesh != mesh) {
                currentMesh = mesh;
                
                currentMesh->Bind();
            }
            
            // Shader binding
            if (currentShader != shader) {
                currentShader = shader;
                
                currentShader->Bind();
                
                glm::mat4 viewProjection = projection * view;
                currentShader->SetProjectionMatrix( viewProjection );
            }
            
            // Material binding
            Material* materialPtr = currentEntity->GetAttachedMaterial();
            if (currentMaterial != materialPtr) {
                currentMaterial = materialPtr;
                
                currentMaterial->Bind();
                currentMaterial->BindTextureSlot(0);
                
                
                if (currentMaterial->doDepthTest) {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(currentMaterial->doDepthTest);
                    glDepthFunc(currentMaterial->depthFunc);
                } else {
                    glDisable(GL_DEPTH_TEST);
                }
                
                
                if (currentMaterial->doFaceCulling) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(currentMaterial->faceCullSide);
                    glFrontFace(currentMaterial->faceWinding);
                } else {
                    glEnable(GL_CULL_FACE);
                }
                
                
                if (currentMaterial->doBlending) {
                    glEnable(GL_BLEND);
                    glBlendFuncSeparate(currentMaterial->blendSource, currentMaterial->blendDestination, currentMaterial->blendAlphaSource, currentMaterial->blendAlphaDestination);
                } else {
                    glDisable(GL_BLEND);
                }
                
                currentShader->Bind();
                currentShader->SetMaterialColor(currentMaterial->color);
                currentShader->SetTextureSampler(0);
            }
            
            currentShader->SetModelMatrix(currentEntity->transform.matrix);
            
            mesh->DrawIndexArray();
            
            continue;
        }
        
    }
    
    SwapBuffers(deviceContext);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::");
#endif
    
    return;
}


