#include <GameEngineFramework/Renderer/RenderSystem.h>

//
// Frame rendering pipeline
//
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
            
            // Sorting
            SortingPass(eye, renderQueueGroup);
            
            // Geometry pass
            for (MeshRenderer* currentEntity : *renderQueueGroup) {
                std::lock_guard<std::mutex> lock(currentEntity->mux);
                
                if (currentEntity->mDoCulling && !CullingPass(currentEntity, scenePtr->camera, viewProjection, frustum))
                    continue;
                
                GeometryPass(currentEntity, eye, scenePtr->camera->forward, viewProjection);
            }
            
            // Shadow pass
            if (mNumberOfShadows > 0) {
                
                shaders.shadowCaster->Bind();
                
                for (MeshRenderer* currentEntity : *renderQueueGroup) {
                    ShadowVolumePass(currentEntity, eye, scenePtr->camera->forward, viewProjection);
                }
                
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
