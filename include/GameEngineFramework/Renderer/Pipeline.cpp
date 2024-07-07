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
    
    
    //
    // Run the scene list
    
    for (std::vector<Scene*>::iterator it = mActiveScenes.begin(); it != mActiveScenes.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        if (!scenePtr->isActive) 
            continue;
        
        // Set the camera projection angle
        setTargetCamera( scenePtr->camera, eye, viewProjection );
        
        // Gather active lights in this scene
        if (scenePtr->doUpdateLights) {
            
            // Update the lights list 
            accumulateSceneLights( scenePtr, eye );
            
            if (mNumberOfLights > RENDER_NUMBER_OF_LIGHTS) 
                mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
            
            // Check continuous light update
            if (!doUpdateLightsEveryFrame) 
                scenePtr->doUpdateLights = false;
            
        }
        
        
        //
        // Draw the render queues
        
        for (unsigned int group=0; group < 5; group++) {
            
            std::vector<MeshRenderer*>* renderQueueGroup;
            
            switch (group) {
                case 0: renderQueueGroup = &scenePtr->mRenderQueueSky; break;
                case 1: renderQueueGroup = &scenePtr->mRenderQueueBackground; break;
                case 2: renderQueueGroup = &scenePtr->mRenderQueueDefault; break;
                case 3: renderQueueGroup = &scenePtr->mRenderQueueForeground; break;
                case 4: renderQueueGroup = &scenePtr->mRenderQueueOverlay; break;
            }
            
            
            if (renderQueueGroup->size() == 0) 
                continue;
            
            //
            // Sorting
            
            SortingPass( eye, renderQueueGroup, group );
            
            
            //
            // Geometry pass
            
            for (unsigned int i=0; i < renderQueueGroup->size(); i++) {
                
                MeshRenderer* currentEntity = *(renderQueueGroup->data() + i);
                
                bool isCulled = false;
                
                if (currentEntity->mDoCulling) 
                    isCulled = CullingPass(currentEntity, scenePtr->camera);
                
                // Render geometry if not culled
                if (!isCulled) 
                    GeometryPass( currentEntity, eye, scenePtr->camera->forward, viewProjection );
                
                continue;
            }
            
            
            //
            // Shadow pass
            
            if (mNumberOfShadows > 0) {
                
                shaders.shadowCaster->Bind();
                
                for (unsigned int i=0; i < renderQueueGroup->size(); i++) {
                    
                    MeshRenderer* currentEntity = *(renderQueueGroup->data() + i);
                    
                    ShadowVolumePass( currentEntity, eye, scenePtr->camera->forward, viewProjection );
                    
                    continue;
                }
                
                mCurrentShader->Bind();
                
            }
            
            continue;
        }
        
        continue;
    }
    
    
    //
    // Light halo effect ???
    /*
    if (mNumberOfLights > 0) {
        
        shaders.color->Bind();
        meshes.cube->Bind();
        
        for (int i=0; i < mNumberOfLights; i++) {
            
            // Point lights
            //if (mLightAttenuation[i].a > 0) 
            //    continue;
            
            glm::mat4 modelMatrix = glm::identity<glm::mat4>();
            modelMatrix = glm::translate(modelMatrix, mLightPosition[i]);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 10, 10));
            
            shaders.color->SetProjectionMatrix( viewProjection );
            shaders.color->SetCameraPosition(eye);
            shaders.color->SetModelMatrix( modelMatrix );
            
            meshes.cube->DrawIndexArray();
            
            continue;
        }
        
        mCurrentShader->Bind();
        mCurrentMesh->Bind();
        
    }
    */
    
    mNumberOfFrames++;
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::EndFrame::");
#endif
    
    return;
}

