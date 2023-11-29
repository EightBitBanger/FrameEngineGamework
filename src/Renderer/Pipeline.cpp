#include "RenderSystem.h"


//
// Frame rendering pipeline
//

void RenderSystem::RenderFrame(void) {
    
    glm::mat4 viewProjection;
    glm::vec3 eye;
    
    mNumberOfDrawCalls = 0;
    
    mNumberOfLights = 0;
    
    // Clear the view port
    glClear(GL_DEPTH_BUFFER_BIT);// | GL_COLOR_BUFFER_BIT);
    
    Shader* currentShader = nullptr;
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::BeginFrame::");
#endif
    
    //
    // Run the scene list
    
    for (std::vector<Scene*>::iterator it = mRenderQueue.begin(); it != mRenderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        // Set the camera projection angle
        setTargetCamera( scenePtr->camera, eye, viewProjection );
        
        // Gather the list of active lights in this scene
        mNumberOfLights += accumulateSceneLights( scenePtr, eye );
        
        if (mNumberOfLights >= RENDER_NUMBER_OF_LIGHTS) 
            mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
        
        // Draw the mesh renderers
        
        std::vector<MeshRenderer*>& meshRenderers = scenePtr->mMeshRendererList;
        unsigned int entityListSz = meshRenderers.size();
        
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
                currentShader->SetLightAttenuation(mNumberOfLights, mLightAttenuation);
                currentShader->SetLightColors(mNumberOfLights, mLightColor);
                
            } else {
                
                // No shader assigned
                continue;
            }
            
            // Set the projection
            currentShader->SetProjectionMatrix( viewProjection );
            currentShader->SetModelMatrix( currentEntity->transform.matrix );
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
    currentCamera->forward.x = cos( (currentCamera->transform.orientation.x * 180) / glm::pi<float>() );
    currentCamera->forward.y = tan( (currentCamera->transform.orientation.y * 180) / glm::pi<float>() );
    currentCamera->forward.z = sin( (currentCamera->transform.orientation.x * 180) / glm::pi<float>() );
    currentCamera->forward = glm::normalize(currentCamera->forward);
    
    glm::vec3 lookingAngle;
    lookingAngle.x = currentCamera->transform.position.x + currentCamera->forward.x;
    lookingAngle.y = currentCamera->transform.position.y + currentCamera->forward.y;
    lookingAngle.z = currentCamera->transform.position.z + currentCamera->forward.z;
    
    // View angle
    glm::mat4 view = glm::lookAt(eye, lookingAngle, currentCamera->up);
    
    // Calculate projection / orthographic angle
    glm::mat4 projection = glm::mat4(0);
    
    if (!currentCamera->isOrthographic) {
        projection = glm::perspective( glm::radians( currentCamera->fov ), currentCamera->aspect, currentCamera->clipNear, currentCamera->clipFar);
    } else {
        projection = glm::ortho(0.0f, (float)displaySize.x, (float)displaySize.y, 0.0f, currentCamera->clipNear, currentCamera->clipFar);
    }
    
    viewProjection = projection * view;
    
    // Right angle to the looking angle
    currentCamera->right = glm::normalize(glm::cross(currentCamera->up, currentCamera->forward));
    
    return true;
}

unsigned int RenderSystem::accumulateSceneLights(Scene* currentScene, glm::vec3 eye) {
    
    unsigned int numberOfLights=0;
    
    if (!currentScene->doUpdateLights) 
        return 0;
    
    std::vector<Light*>& lightList = currentScene->mLightList;
    unsigned int totalNumberOfLights = lightList.size();
    
    for (unsigned int i=0; i < totalNumberOfLights; i++) {
        
        Light* lightPtr = lightList[i];
        
        if (!lightPtr->isActive) 
            continue;
        
        if (glm::distance(eye, lightPtr->position) > lightPtr->renderDistance) 
            continue;
        
        mLightPosition[i] = lightPtr->position;
        
        mLightAttenuation[i].x = lightPtr->intensity;
        mLightAttenuation[i].y = lightPtr->range;
        mLightAttenuation[i].z = lightPtr->attenuation;
        
        mLightColor[i].r = lightPtr->color.r;
        mLightColor[i].g = lightPtr->color.g;
        mLightColor[i].b = lightPtr->color.b;
        
        numberOfLights++;
        
        continue;
    }
    
    // Check to reset light update
    if (!doUpdateLightsEveryFrame) 
        currentScene->doUpdateLights = false;
    
    return numberOfLights;
}
