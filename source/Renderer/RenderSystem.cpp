#include "rendersystem.h"


RenderSystem::RenderSystem() : 
    
    cameraMain(nullptr),
    
    mWindowHandle(NULL),
    mDeviceContext(NULL),
    mRenderContext(NULL),
    
    mCurrentMesh(nullptr),
    mCurrentMaterial(nullptr)
{
}

MeshRenderer* RenderSystem::CreateMeshRenderer(void) {
    MeshRenderer* meshRendererPtr = mEntity.Create();
    meshRendererPtr->AttachMaterial(defaultMaterial);
    return meshRendererPtr;
}
bool RenderSystem::DestroyMeshRenderer(MeshRenderer* meshRendererPtr) {
    return mEntity.Destroy(meshRendererPtr);
}

Mesh* RenderSystem::CreateMesh(void) {
    Mesh* meshPtr = mMesh.Create();
    return meshPtr;
}
bool RenderSystem::DestroyMesh(Mesh* meshPtr) {
    return mMesh.Destroy(meshPtr);
}

Shader* RenderSystem::CreateShader(void) {
    Shader* shaderPtr = mShader.Create();
    return shaderPtr;
}
bool RenderSystem::DestroyShader(Shader* shaderPtr) {
    return mShader.Destroy(shaderPtr);
}

Camera* RenderSystem::CreateCamera(void) {
    Camera* cameraPtr = mCamera.Create();
    return cameraPtr;
}
bool RenderSystem::DestroyCamera(Camera* cameraPtr) {
    return mCamera.Destroy(cameraPtr);
}

Material* RenderSystem::CreateMaterial(void) {
    Material* materialPtr = mMaterial.Create();
    materialPtr->SetShader(defaultShader);
    return materialPtr;
}
bool RenderSystem::DestroyMaterial(Material* materialPtr) {
    return mMaterial.Destroy(materialPtr);
}

Light* RenderSystem::CreateLight(void) {
    Light* lightPtr = mLight.Create();
    return lightPtr;
}
bool RenderSystem::DestroyLight(Light* lightPtr) {
    return mLight.Destroy(lightPtr);
}

Scene* RenderSystem::CreateScene(void) {
    Scene* scenePtr = mScene.Create();
    return scenePtr;
}
bool RenderSystem::DestroyScene(Scene* scenePtr) {
    return mScene.Destroy(scenePtr);
}

RenderPipeline* RenderSystem::CreateRenderPipeline(void) {
    return mPipeline.Create();
}

bool RenderSystem::DestroyRenderPipeline(RenderPipeline* renderPipelinePtr) {
    return mPipeline.Destroy(renderPipelinePtr);
}

void RenderSystem::Initiate(void) {
    
    defaultShader = CreateShader();
    defaultShader->BuildDefault();
    
    defaultMaterial = CreateMaterial();
    
    currentPipeline = CreateRenderPipeline();
    currentPipeline->currentShader = defaultShader;
    
#ifdef _RENDERER_CHECK_OPENGL_ERRORS__
    GetGLErrorCodes("OnInitiate::");
#endif
    
    return;
}


void RenderSystem::AddSceneToRenderQueue(Scene* scenePtr) {
    assert(scenePtr != nullptr);
    mRenderQueue.push_back( scenePtr );
    return;
}

bool RenderSystem::RemoveSceneFromRenderQueue(Scene* scenePtr) {
    for (std::vector<Scene*>::iterator it = mRenderQueue.begin(); it != mRenderQueue.end(); ++it) {
        Scene* thisScenePtr = *it;
        if (scenePtr == thisScenePtr) {
            mRenderQueue.erase(it);
            return true;
        }
    }
    return false;
}

unsigned int RenderSystem::GetRenderQueueSize(void) {
    return mRenderQueue.size();
}

Scene* RenderSystem::GetRenderQueueScene(unsigned int index) {
    assert(index < mRenderQueue.size());
    return mRenderQueue[index];
}

unsigned int RenderSystem::GetMeshRendererCount(void) {
    return mEntity.Size();
}

GLenum RenderSystem::SetRenderTarget(HWND wHndl) {
    
    int iFormat;
    std::string gcVendor, gcRenderer, gcExtensions, gcVersion, Line;
    
    // Set the window handle and get the device context
    mWindowHandle = wHndl;
    HDC hDC = GetDC(wHndl);
    mDeviceContext = hDC;
    
    // Get display size
    displaySize.x = GetDeviceCaps(mDeviceContext, HORZRES);
    displaySize.y = GetDeviceCaps(mDeviceContext, VERTRES);
    displayCenter.x = displaySize.x / 2;
    displayCenter.y = displaySize.y / 2;
    
    // Get window size
    RECT wRect;
    GetWindowRect(mWindowHandle, &wRect);
    
    SetViewport(0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top);
    
    // Pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize       = sizeof (pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cDepthBits  = 16;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    int numberOfObjectsToThrow = 3;
        
    // Setup pixel format
    iFormat = ChoosePixelFormat(mDeviceContext, &pfd);
    SetPixelFormat(mDeviceContext, iFormat, &pfd);
    
    HGLRC hRC = wglCreateContext(hDC);
    mRenderContext = hRC;
    
    wglMakeCurrent(mDeviceContext, hRC);
    
    // Initiate glew after setting the render target
    GLenum glpassed = glewInit();
    
    //
    // Log hardware details
    
    const char* gcVendorConst     = (const char*)glGetString(GL_VENDOR);
    const char* gcRendererConst   = (const char*)glGetString(GL_RENDERER);
    const char* gcExtensionsConst = (const char*)glGetString(GL_EXTENSIONS);
    const char* gcVersionConst    = (const char*)glGetString(GL_VERSION);
    
    gcVendor     = std::string(gcVendorConst);
    gcRenderer   = std::string(gcRendererConst);
    gcExtensions = std::string(gcExtensionsConst);
    gcVersion    = std::string(gcVersionConst);
    
    // Log details
    Log.Write("== Hardware details =="); Line = "" + gcRenderer;
    Log.Write(Line);
    Log.WriteLn();
    
    std::string DetailStringHead = "  - ";
    std::string DetailStringEqu  = " = ";
    
    Line = " Device"; Log.Write(Line);
    Line = DetailStringHead + "Name   " + DetailStringEqu + gcVendor;  Log.Write(Line);
    Line = DetailStringHead + "Version" + DetailStringEqu + gcVersion; Log.Write(Line);
    Log.WriteLn();
    
    Line = " Colors"; Log.Write(Line);
    Line = DetailStringHead + "Color" + DetailStringEqu + IntToString(pfd.cColorBits) + " bit"; Log.Write(Line);
    Line = DetailStringHead + "Depth" + DetailStringEqu + IntToString(pfd.cDepthBits) + " bit"; Log.Write(Line);
    Log.WriteLn();
    Log.WriteLn();
    
    return glpassed;
}

void RenderSystem::ReleaseRenderTarget(void) {
    
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext(mRenderContext);
    
    ReleaseDC(mWindowHandle, mDeviceContext);
    
    return;
}

void RenderSystem::SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    viewport.x = x;
    viewport.y = y;
    viewport.w = w;
    viewport.h = h;
    return;
}

glm::mat4 RenderSystem::CalculateModelMatrix(Transform& model) {
    
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3( model.position.x, model.position.y, model.position.z ));
    glm::mat4 rotation = glm::toMat4(model.orientation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3( model.scale.x, model.scale.y, model.scale.z ));
    
    return translation * rotation * scale;
}

std::vector<std::string> RenderSystem::GetGLErrorCodes(std::string errorLocationString) {
    
    GLenum glError;
    std::string ErrorMsg = errorLocationString;
    std::vector<std::string> ErrorList;
    
    // Get any GL error
    glError = glGetError();
    
    // While there are errors
    while (glError != GL_NO_ERROR) {
        
        switch(glError) {
            
            case GL_INVALID_OPERATION:             ErrorMsg+=" INVALID_OPERATION"; break;
            case GL_INVALID_ENUM:                  ErrorMsg+=" INVALID_ENUM";  break;
            case GL_INVALID_VALUE:                 ErrorMsg+=" INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY:                 ErrorMsg+=" OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: ErrorMsg+=" INVALID_FRAMEBUFFER_OPERATION"; break;
            default :                              ErrorMsg+=" UNKNOWN_ERROR"; break;
            
        }
        
        //Log.Write(ErrorMsg);
        //Log.WriteLn();
        
        ErrorList.push_back( ErrorMsg );
        
        std::cout << ErrorMsg << std::endl;
        
        
        // Get the next error
        glError = glGetError();
    }
    
    return ErrorList;
}


//
// Render processing
//

void RenderSystem::RenderFrame(float deltaTime) {
    
    if (cameraMain == nullptr) 
        return;
    
    // Clear the view port
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
    
    // Camera mouse looking
    if (cameraMain->useMouseLook) {
        cameraMain->MouseLook(deltaTime, displayCenter.x, displayCenter.y);
    } else {
        // Restore camera looking angle
        cameraMain->transform.orientation.x = cameraMain->lookAngle.x;
        cameraMain->transform.orientation.y = cameraMain->lookAngle.y;
    }
    
    
    glm::mat4 projection = glm::perspective( glm::radians( cameraMain->fov ), cameraMain->aspect, cameraMain->clipNear, cameraMain->clipFar);
    
    // Calculate viewing angle
    glm::vec3 eye;
    eye.x = cameraMain->transform.position.x;
    eye.y = cameraMain->transform.position.y;
    eye.z = cameraMain->transform.position.z;
    
    // Forward looking angle
    cameraMain->forward.x = cos( cameraMain->transform.orientation.x * 180 / glm::pi<float>() );
    cameraMain->forward.y = tan( cameraMain->transform.orientation.y * 180 / glm::pi<float>() );
    cameraMain->forward.z = sin( cameraMain->transform.orientation.x * 180 / glm::pi<float>() );
    cameraMain->forward = glm::normalize(cameraMain->forward);
    
    // Calculate view point
    glm::vec3 angle;
    angle.x = cameraMain->transform.position.x + cameraMain->forward.x;
    angle.y = cameraMain->transform.position.y + cameraMain->forward.y;
    angle.z = cameraMain->transform.position.z + cameraMain->forward.z;
    
    glm::mat4 view = glm::lookAt(eye, angle, cameraMain->up);
    glm::mat4 viewProjection = projection * view;
    
    // Right angle to the looking angle
    cameraMain->right = glm::normalize(glm::cross(cameraMain->up, cameraMain->forward));
    
    
    // Bind the render pipeline
    if (currentPipeline == nullptr) 
        return;
    
    // Set the default shader
    Shader* currentShader = currentPipeline->currentShader;
    currentShader->Bind();
    currentShader->SetProjectionMatrix( viewProjection );
    
    currentShader->SetCameraPosition(eye);
    
    //
    // Run through the scenes
    //
    
    for (std::vector<Scene*>::iterator it = mRenderQueue.begin(); it != mRenderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        //
        // Check to update the scene light list
        //
        
        if (scenePtr->doUpdateLights) {
            numberOfLights = scenePtr->GetLightQueueSize();
            if (numberOfLights > RENDER_NUMBER_OF_LIGHTS) numberOfLights = RENDER_NUMBER_OF_LIGHTS;
            
            // Accumulate a list of lights
            for (unsigned int i=0; i < numberOfLights; i++) {
                Light* lightPtr = scenePtr->GetLight(i);
                
                // Get the light position, attenuation and color
                lightPosition[i] = lightPtr->transform.position;
                
                lightAttenuation[i].x = lightPtr->intensity;
                lightAttenuation[i].y = lightPtr->range;
                lightAttenuation[i].z = lightPtr->attenuation;
                
                lightColor[i].r = lightPtr->color.r;
                lightColor[i].g = lightPtr->color.g;
                lightColor[i].b = lightPtr->color.b;
                
                continue;
            }
            
            // Send the lights into the shader
            currentShader->SetLightCount(numberOfLights);
            currentShader->SetLightPositions(numberOfLights, lightPosition);
            currentShader->SetLightAttenuation(numberOfLights, lightAttenuation);
            currentShader->SetLightColors(numberOfLights, lightColor);
            
        }
        
        //
        // Render entities
        //
        
        unsigned int entityListSz = scenePtr->GetMeshRendererQueueSize();
        
        for (unsigned int i=0; i < entityListSz; i++) {
            
            MeshRenderer* currentEntity = scenePtr->GetMeshRenderer(i);
            
            // Mesh binding
            Mesh* mesh = currentEntity->GetAttachedMesh();
            if (mesh == nullptr) 
                continue;
            
            if (mCurrentMesh != mesh) {
                mCurrentMesh = mesh;
                
                mCurrentMesh->Bind();
            }
            
            //
            // Material binding
            //
            Material* materialPtr = currentEntity->GetAttachedMaterial();
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
                } else {
                    glDisable(GL_DEPTH_TEST);
                }
                
                // Face culling and winding
                if (mCurrentMaterial->doFaceCulling) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(mCurrentMaterial->faceCullSide);
                    glFrontFace(mCurrentMaterial->faceWinding);
                } else {
                    glDisable(GL_CULL_FACE);
                }
                
                // Blending
                if (mCurrentMaterial->doBlending) {
                    glEnable(GL_BLEND);
                    glBlendFuncSeparate(mCurrentMaterial->blendSource, mCurrentMaterial->blendDestination, mCurrentMaterial->blendAlphaSource, mCurrentMaterial->blendAlphaDestination);
                } else {
                    glDisable(GL_BLEND);
                }
                
                
                //
                // Shader binding
                //
                Shader* shaderPtr = materialPtr->GetShader();
                if (shaderPtr != nullptr) {
                    if (currentShader != shaderPtr) {
                        currentShader = shaderPtr;
                        currentShader->Bind();
                        
                        currentShader->SetProjectionMatrix( viewProjection );
                        currentShader->SetCameraPosition(eye);
                    }
                }
                
                // Apply material to shader
                currentShader->SetMaterialAmbient(mCurrentMaterial->ambient);
                currentShader->SetMaterialDiffuse(mCurrentMaterial->diffuse);
                currentShader->SetTextureSampler(0);
            }
            
            
            currentShader->SetModelMatrix(currentEntity->transform.matrix);
            
            mesh->DrawIndexArray();
            
            continue;
        }
        
    }
    
    SwapBuffers(mDeviceContext);
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnRender::");
#endif
    
    return;
}
