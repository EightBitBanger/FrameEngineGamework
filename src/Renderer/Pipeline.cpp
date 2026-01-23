#include <GameEngineFramework/Renderer/RenderSystem.h>

int dbgCounter = 0;

void RenderSystem::RenderFrame(void) {
    
    glm::mat4 viewProjection;
    glm::vec3 eye;
    
    mNumberOfDrawCalls = 0;
    
    if (doUpdateLightsEveryFrame) {
        mNumberOfLights = 0;
        mNumberOfShadows = 0;
    }
    
    // Clear the view port
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::BeginFrame::");
#endif
    
    std::lock_guard<std::mutex> lock(mux);
    
    // Run the scene list
    for (Scene* scenePtr : mActiveScenes) {
        if (!scenePtr->isActive) 
            continue;
        
        // Set the camera projection angle
        setTargetCamera(scenePtr->camera, eye, viewProjection);
        
        // Extract camera projection edges for clipping
        Frustum frustum = FrustumExtractPlanes(viewProjection);
        
        // Gather fog layers
        accumulateSceneFogLayers(scenePtr);
        
        // Gather active lights in this scene
        if (scenePtr->doUpdateLights) {
            accumulateSceneLights(scenePtr, eye);
            
            if (mNumberOfLights > RENDER_NUMBER_OF_LIGHTS) 
                mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
            
            if (!doUpdateLightsEveryFrame) 
                scenePtr->doUpdateLights = false;
        }
        
        // Draw the render queues
        for (unsigned int group = 0; group < RENDER_NUMBER_OF_QUEUE_GROUPS; group++) {
            
            std::vector<MeshRenderer*>* renderQueueGroup = nullptr;
            switch (group) {
                case 0: renderQueueGroup = &scenePtr->mRenderQueueSky; break;
                case 1: renderQueueGroup = &scenePtr->mRenderQueueBackground; break;
                case 2: renderQueueGroup = &scenePtr->mRenderQueuePreGrometry; break;
                case 3: renderQueueGroup = &scenePtr->mRenderQueueGeometry; break;
                case 4: renderQueueGroup = &scenePtr->mRenderQueuePostGeometry; break;
                case 5: renderQueueGroup = &scenePtr->mRenderQueueForeground; break;
                case 6: renderQueueGroup = &scenePtr->mRenderQueueOverlay; break;
            }
            
            if (!renderQueueGroup || renderQueueGroup->empty()) 
                continue;
            
            std::vector<MeshRenderer*> shadowList;
            shadowList.reserve(100);
            
            SortingPass(eye, renderQueueGroup);
            
            // Geometry pass
            for (MeshRenderer* currentEntity : *renderQueueGroup) {
                if (!currentEntity->isActive) 
                    continue;
                
                if (currentEntity->material == nullptr || 
                    currentEntity->mesh == nullptr) 
                    continue;
                
                if (currentEntity->material->shader == nullptr) 
                    continue;
                
                if (currentEntity->mDoCulling) 
                    if (!CullingPass(currentEntity, scenePtr->camera, viewProjection, frustum)) 
                        continue;
                
                // Level of detail shift selection
                Mesh* mesh = LevelOfDetailPass(currentEntity, eye);
                if (mesh == nullptr) 
                    continue;
                
                BindMesh( mesh );
                
                Material* materialPtr = currentEntity->material;
                if (materialPtr == nullptr) 
                    continue;
                BindMaterial( materialPtr );
                
                Shader* shaderPtr = materialPtr->shader;
                if (shaderPtr == nullptr) 
                    continue;
                BindShader( shaderPtr );
                
                // Set texture count
                unsigned int numberOfTextures = materialPtr->mTextures.size();
                shaderPtr->SetTextureSamplerCount(numberOfTextures);
                
                GeometryPass(currentEntity, eye, scenePtr->camera->forward, viewProjection);
                
                if (currentEntity->material->mDoShadowPass) 
                    shadowList.push_back(currentEntity);
            }
            
            // Shadow volume pass
            for (unsigned int i=0; i < shadowList.size(); i++) {
                MeshRenderer* currentEntity = shadowList[i];
                
                BindMesh( currentEntity->mesh );
                
                // Material binding
                Material* materialPtr = currentEntity->material;
                if (materialPtr == nullptr) 
                    continue;
                BindMaterial( materialPtr );
                
                shaders.shadowCaster->Bind();
                
                if (currentEntity->material->mDoShadowPass) 
                    ShadowVolumePass(currentEntity, eye, scenePtr->camera->forward, viewProjection);
                
                if (mCurrentShader) 
                    mCurrentShader->Bind();
            }
        }
    }
    
    mNumberOfFrames++;
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::EndFrame::");
#endif
}
