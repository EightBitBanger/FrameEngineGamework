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
    
    glm::mat4 projection = cameraMain->CalculatePerspectiveMatrix();
    glm::mat4 view = cameraMain->CalculateView();
    
    glm::mat4 viewProj = projection * view;
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
                currentShader->SetProjectionMatrix(viewProj);
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
            
            currentShader->SetModelMatrix(currentEntity->transform.modelMatrix);
            
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


