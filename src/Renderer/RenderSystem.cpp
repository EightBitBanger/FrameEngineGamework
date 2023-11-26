#include "rendersystem.h"

#include <iostream>

#include "../../vendor/codebaselibrary/logging.h"

extern Logger Log;

Logger RenderLog;

// Render thread
bool isThreadActive = true;
void RenderThreadMain(void);


RenderSystem::RenderSystem() : 
    viewport(Viewport(0, 0, 0, 0)),
    
    displaySize(glm::vec2(0, 0)),
    displayCenter(glm::vec2(0, 0)),
    
    mNumberOfDrawCalls(0),
    
    mWindowHandle(NULL),
    mDeviceContext(NULL),
    mRenderContext(NULL),
    
    mCurrentMesh(nullptr),
    mCurrentMaterial(nullptr),
    
    mNumberOfLights(0)
{
}

MeshRenderer* RenderSystem::CreateMeshRenderer(void) {
    MeshRenderer* meshRendererPtr = mEntity.Create();
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
    cameraPtr->viewport = viewport;
    return cameraPtr;
}
bool RenderSystem::DestroyCamera(Camera* cameraPtr) {
    return mCamera.Destroy(cameraPtr);
}

Material* RenderSystem::CreateMaterial(void) {
    Material* materialPtr = mMaterial.Create();
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

void RenderSystem::Initiate(void) {
    
#ifdef RENDERER_CHECK_OPENGL_ERRORS
    GetGLErrorCodes("OnInitiate::");
#endif
    
    renderThreadMain = new std::thread( RenderThreadMain );
    
    Log.Write( " >> Render thread started" );
    
    return;
}

void RenderSystem::Shutdown(void) {
    
    isThreadActive = false;
    
    renderThreadMain->join();
    
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
    pfd.cDepthBits  = 32;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    
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
#ifdef  LOG_RENDER_DETAILS
    const char* gcVendorConst     = (const char*)glGetString(GL_VENDOR);
    const char* gcRendererConst   = (const char*)glGetString(GL_RENDERER);
    const char* gcExtensionsConst = (const char*)glGetString(GL_EXTENSIONS);
    const char* gcVersionConst    = (const char*)glGetString(GL_VERSION);
    
    gcVendor     = std::string(gcVendorConst);
    gcRenderer   = std::string(gcRendererConst);
    gcExtensions = std::string(gcExtensionsConst);
    gcVersion    = std::string(gcVersionConst);
    
    // Log details
    RenderLog.Write("== Hardware details =="); Line = "" + gcRenderer;
    RenderLog.Write(Line);
    RenderLog.WriteLn();
    
    std::string DetailStringHead = "  - ";
    std::string DetailStringEqu  = " = ";
    
    Line = " Device"; RenderLog.Write(Line);
    Line = DetailStringHead + "Name   " + DetailStringEqu + gcVendor;  RenderLog.Write(Line);
    Line = DetailStringHead + "Version" + DetailStringEqu + gcVersion; RenderLog.Write(Line);
    RenderLog.WriteLn();
    
    Line = " Colors"; RenderLog.Write(Line);
    Line = DetailStringHead + "Color" + DetailStringEqu + IntToString(pfd.cColorBits) + " bit"; RenderLog.Write(Line);
    Line = DetailStringHead + "Depth" + DetailStringEqu + IntToString(pfd.cDepthBits) + " bit"; RenderLog.Write(Line);
    RenderLog.WriteLn();
    RenderLog.WriteLn();
#endif
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
        
        ErrorList.push_back( ErrorMsg );
        
        std::cout << ErrorMsg << std::endl;
        
        // Get the next error
        glError = glGetError();
    }
    
    return ErrorList;
}


unsigned int RenderSystem::GetNumberOfDrawCalls(void) {
    return mNumberOfDrawCalls;
}


//
// Frame rendering pipeline
//

void RenderSystem::RenderFrame(void) {
    
    glm::mat4 viewProjection;
    glm::vec3 eye;
    
    mNumberOfDrawCalls = 0;
    
    // Clear the view port
    glClear(GL_DEPTH_BUFFER_BIT);
    
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
        unsigned int numberOfActiveLights=0;
        accumulateSceneLights( scenePtr, eye, numberOfActiveLights );
        
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
                currentShader->SetLightCount(numberOfActiveLights);
                currentShader->SetLightPositions(numberOfActiveLights, mLightPosition);
                currentShader->SetLightAttenuation(numberOfActiveLights, mLightAttenuation);
                currentShader->SetLightColors(numberOfActiveLights, mLightColor);
                
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

void RenderSystem::accumulateSceneLights(Scene* currentScene, glm::vec3 eye, unsigned int& numberOfActiveLights) {
    
    numberOfActiveLights=0;
    
    if (mNumberOfLights > RENDER_NUMBER_OF_LIGHTS) 
        mNumberOfLights = RENDER_NUMBER_OF_LIGHTS;
    
    if (!currentScene->doUpdateLights) 
        return;
    
    std::vector<Light*> lightList = currentScene->mLightList;
    mNumberOfLights = currentScene->mLightList.size();
    
    for (unsigned int i=0; i < mNumberOfLights; i++) {
        
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
        
        numberOfActiveLights++;
        continue;
    }
    
    return;
}



//
// Render thread
//

void RenderThreadMain(void) {
    
    while (isThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(3) );
    Log.Write( " >> Render thread stopped" );
    
    return;
}



