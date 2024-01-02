#include <GameEngineFramework/Renderer/RenderSystem.h>


//
// Frame rendering pipeline
//

void RenderSystem::RenderFrame(void) {
    
    glm::mat4 viewProjection;
    glm::vec3 eye;
    
    mNumberOfDrawCalls = 0;
    
    mNumberOfLights = 0;
    
    // Clear the view port
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    Shader* currentShader = nullptr;
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::BeginFrame::");
#endif
    
    
    //
    // Run the scene list
    
    for (std::vector<Scene*>::iterator it = mRenderQueue.begin(); it != mRenderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        if (!scenePtr->isActive) 
            continue;
        
        // Set the camera projection angle
        setTargetCamera( scenePtr->camera, eye, viewProjection );
        
        //
        // Gather active lights in this scene
        if (scenePtr->doUpdateLights) 
            accumulateSceneLights( scenePtr, eye );
        
        if (mNumberOfLights > RENDER_NUMBER_OF_LIGHTS) 
            mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
        
        // Check continuous  
        if (!doUpdateLightsEveryFrame) 
            scenePtr->doUpdateLights = false;
        
        
        
        //
        // Draw the mesh renderers
        
        std::vector<MeshRenderer*>& meshRenderers = scenePtr->mMeshRendererList;
        
        unsigned int entityListSz = scenePtr->mMeshRendererList.size();
        
        for (unsigned int i=0; i < entityListSz; i++) {
            
            MeshRenderer* currentEntity = meshRenderers[i];
            
            // Mesh binding
            
            Mesh* mesh = currentEntity->mesh;
            if (mesh == nullptr) 
                continue;
            
            if (mCurrentMesh != mesh) {
                
                mCurrentMesh = mesh;
                mCurrentMesh->Bind();
            }
            
            
            // Material binding
            
            Material* materialPtr = currentEntity->material;
            if (materialPtr == nullptr) 
                continue;
            
            if (mCurrentMaterial != materialPtr) {
                mCurrentMaterial = materialPtr;
                
                mCurrentMaterial->Bind();
                mCurrentMaterial->BindTextureSlot(0);
                
                // Depth testing
                
                if (mCurrentMaterial->doDepthTest) {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(mCurrentMaterial->doDepthTest);
                    glDepthFunc(mCurrentMaterial->depthFunc);
                    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::DepthTest::");
#endif
                    
                } else {
                    glDisable(GL_DEPTH_TEST);
                }
                
                // Face culling and winding
                
                if (mCurrentMaterial->doFaceCulling) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(mCurrentMaterial->faceCullSide);
                    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Culling::");
#endif
                    
                } else {
                    glDisable(GL_CULL_FACE);
                }
                
                
                // Face winding order
                glFrontFace(mCurrentMaterial->faceWinding);
                
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::FaceWinding::");
#endif
                
                // Blending
                
                if (mCurrentMaterial->doBlending) {
                    glEnable(GL_BLEND);
                    
                    //glBlendFunc(mCurrentMaterial->blendSource,
                    //            mCurrentMaterial->blendDestination);
                    
                    
                    glBlendFuncSeparate(mCurrentMaterial->blendSource,
                                        mCurrentMaterial->blendDestination,
                                        mCurrentMaterial->blendAlphaSource,
                                        mCurrentMaterial->blendAlphaDestination);
                    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::Material::Blending::");
#endif
                    
                } else {
                    glDisable(GL_BLEND);
                }
                
            }
            
            
            // Shader binding
            
            Shader* shaderPtr = materialPtr->shader;
            if (shaderPtr != nullptr) {
                
                if (currentShader != shaderPtr) {
                    
                    currentShader = shaderPtr;
                    currentShader->Bind();
                    
                    currentShader->SetTextureSampler(0);
                }
                
                // Send in the light list
                currentShader->SetLightCount(mNumberOfLights);
                currentShader->SetLightPositions(mNumberOfLights, mLightPosition);
                currentShader->SetLightDirections(mNumberOfLights, mLightDirection);
                currentShader->SetLightAttenuation(mNumberOfLights, mLightAttenuation);
                currentShader->SetLightColors(mNumberOfLights, mLightColor);
                
                // Send in the shadow angle
                currentShader->SetShadowMatrix( mShadowTransform.matrix );
                
            } else {
                
                // No shader assigned
                continue;
            }
            
            // Set the projection
            currentShader->SetProjectionMatrix( viewProjection );
            currentShader->SetModelMatrix( currentEntity->transform.matrix );
            
            // Inverse transpose model matrix
            glm::mat3 invTransposeMatrix = glm::transpose( glm::inverse( currentEntity->transform.matrix ) );
            currentShader->SetInverseModelMatrix( invTransposeMatrix );
            
            currentShader->SetCameraPosition(eye);
            
            // Set the material and texture
            currentShader->SetMaterialAmbient(mCurrentMaterial->ambient);
            currentShader->SetMaterialDiffuse(mCurrentMaterial->diffuse);
            currentShader->SetMaterialSpecular(mCurrentMaterial->specular);
            
            mesh->DrawIndexArray();
            
            mNumberOfDrawCalls++;
            
            continue;
        }
        
    }
    
    mNumberOfFrames++;
    
    if (mNumberOfFrames > 7) 
        SwapBuffers(mDeviceContext);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::EndFrame::");
#endif
    
    return;
}

bool RenderSystem::setTargetCamera(Camera* currentCamera, glm::vec3& eye, glm::mat4& viewProjection) {
    
    if (currentCamera == nullptr) 
        return false;
    
    glViewport(currentCamera->viewport.x, 
               currentCamera->viewport.y, 
               currentCamera->viewport.w, 
               currentCamera->viewport.h);
    
    // Point of origin
    eye.x = currentCamera->transform.position.x;
    eye.y = currentCamera->transform.position.y;
    eye.z = currentCamera->transform.position.z;
    
    // Forward looking angle
    currentCamera->forward.x = cos( (currentCamera->transform.localRotation.x * 180) / glm::pi<float>() );
    currentCamera->forward.y = tan( (currentCamera->transform.localRotation.y * 180) / glm::pi<float>() );
    currentCamera->forward.z = sin( (currentCamera->transform.localRotation.x * 180) / glm::pi<float>() );
    currentCamera->forward = glm::normalize(currentCamera->forward);
    
    glm::vec3 lookingAngle;
    lookingAngle.x = currentCamera->transform.position.x + currentCamera->forward.x;
    lookingAngle.y = currentCamera->transform.position.y + currentCamera->forward.y;
    lookingAngle.z = currentCamera->transform.position.z + currentCamera->forward.z;
    
    // View angle
    glm::mat4 view = glm::lookAt(eye, lookingAngle, currentCamera->up);
    
    // Calculate perspective / orthographic angle
    if (!currentCamera->isOrthographic) {
        
        glm::mat4 projection = glm::perspective( glm::radians( currentCamera->fov ), 
                                                 currentCamera->aspect, 
                                                 currentCamera->clipNear, 
                                                 currentCamera->clipFar);
        
        viewProjection = projection * view;
        
    } else {
        
        glm::mat4 projection = glm::ortho(0.0f, 
                                          (float)displaySize.x, 
                                          (float)displaySize.y, 
                                          0.0f, 
                                          currentCamera->clipNear, 
                                          currentCamera->clipFar);
        
        viewProjection = projection * view;
        
    }
    
    // Right angle to the looking angle
    currentCamera->right = glm::normalize(glm::cross(currentCamera->up, currentCamera->forward));
    
    return true;
}

unsigned int RenderSystem::accumulateSceneLights(Scene* currentScene, glm::vec3 eye) {
    
    std::vector<Light*>& lightList = currentScene->mLightList;
    unsigned int totalNumberOfLights = lightList.size();
    
    unsigned int i;
    for (i=0; i < totalNumberOfLights; i++) {
        
        Light* lightPtr = lightList[i];
        
        if (glm::distance(eye, lightPtr->position) > lightPtr->renderDistance) 
            continue;
        
        if (!lightPtr->isActive) 
            continue;
        
        mLightPosition[mNumberOfLights]  = lightPtr->position + lightPtr->offset;
        mLightDirection[mNumberOfLights] = lightPtr->direction;
        
        mLightAttenuation[mNumberOfLights].r = lightPtr->intensity;
        mLightAttenuation[mNumberOfLights].g = lightPtr->range;
        mLightAttenuation[mNumberOfLights].b = lightPtr->attenuation;
        mLightAttenuation[mNumberOfLights].a = lightPtr->type;
        
        mLightColor[mNumberOfLights].r = lightPtr->color.r;
        mLightColor[mNumberOfLights].g = lightPtr->color.g;
        mLightColor[mNumberOfLights].b = lightPtr->color.b;
        
        mNumberOfLights++;
        
        if (mNumberOfLights >= RENDER_NUMBER_OF_LIGHTS) {
            mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
            break;
        }
        
        continue;
    }
    
    return i;
}
