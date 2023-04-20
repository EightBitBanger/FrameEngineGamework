#include "rendersystem.h"

RenderSystem::RenderSystem() {
    
    windowHandle   = NULL;
    deviceContext  = NULL;
    renderContext  = NULL;
    
    renderQueue.clear();
    
    currentMesh      = nullptr;
    currentMaterial  = nullptr;
    currentShader    = nullptr;
    
    cameraMain = nullptr;
    
}


Entity* RenderSystem::CreateEntity(void) {
    Entity* entityPtr = entity.Create();
    entityPtr->material = defaultMaterial;
    return entityPtr;
}
bool RenderSystem::DestroyEntity(Entity* entityPtr) {
    return entity.Destroy(entityPtr);
}

Mesh* RenderSystem::CreateMesh(void) {
    Mesh* meshPtr = mesh.Create();
    return meshPtr;
}
bool RenderSystem::DestroyMesh(Mesh* meshPtr) {
    return mesh.Destroy(meshPtr);
}

Shader* RenderSystem::CreateShader(void) {
    Shader* shaderPtr = shader.Create();
    return shaderPtr;
}
bool RenderSystem::DestroyShader(Shader* shaderPtr) {
    return shader.Destroy(shaderPtr);
}

Camera* RenderSystem::CreateCamera(void) {
    Camera* cameraPtr = camera.Create();
    return cameraPtr;
}
bool RenderSystem::DestroyCamera(Camera* cameraPtr) {
    return camera.Destroy(cameraPtr);
}

Material* RenderSystem::CreateMaterial(void) {
    Material* materialPtr = material.Create();
    return materialPtr;
}
bool RenderSystem::DestroyMaterial(Material* materialPtr) {
    return material.Destroy(materialPtr);
}

Sky* RenderSystem::CreateSky(void) {
    Sky* skyPtr = sky.Create();
    return skyPtr;
}
bool RenderSystem::DestroySky(Sky* skyPtr) {
    return sky.Destroy(skyPtr);
}

Scene* RenderSystem::CreateScene(void) {
    Scene* scenePtr = scene.Create();
    return scenePtr;
}
Scene* RenderSystem::GetScene(unsigned int index) {
    assert(index <= renderQueue.size());
    return renderQueue[index];
}
bool RenderSystem::DestroyScene(Scene* scenePtr) {
    return scene.Destroy(scenePtr);
}

Script* RenderSystem::CreateScript(void) {
    Script* scriptPtr = script.Create();
    return scriptPtr;
}
bool RenderSystem::DestroyScript(Script* scriptPtr) {
    return script.Destroy(scriptPtr);
}


void RenderSystem :: RenderFrame(float deltaTime) {
    
    if (cameraMain == nullptr) 
        return;
    
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    if (skyMain != nullptr) {
        Color& color = skyMain->background;
        glClearColor(color.r, color.g, color.b, 1);
    }
    
    
    if (cameraMain->script != nullptr) 
          cameraMain->script->OnUpdate();
    
    
    if (cameraMain->useMouseLook) 
        cameraMain->MouseLook(deltaTime, displayCenter.x, displayCenter.y);
    
    
    glm::mat4 projection = cameraMain->CalculatePerspectiveMatrix();
    glm::mat4 view = cameraMain->CalculateView();
    
    glm::mat4 viewProj = projection * view;
    currentShader = nullptr;
    
    
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        for (std::vector<Entity*>::iterator it = scenePtr->entities.begin(); it != scenePtr->entities.end(); ++it) {
            
            Entity* currentEntity = *it;
            
            if (currentEntity->script != nullptr) 
                currentEntity->script->OnUpdate();
            
            if (currentEntity->mesh == nullptr) continue;
            Mesh* mesh = currentEntity->mesh;
            
            if (mesh->shader == nullptr) continue;
            Shader* shader = mesh->shader;
            
            
            if (currentMesh != mesh) {
                currentMesh = mesh;
                
                currentMesh->Bind();
            }
            
            
            if (currentShader != shader) {
                currentShader = shader;
                
                currentShader->Bind();
                currentShader->SetProjectionMatrix(viewProj);
            }
            
            
            if (currentMaterial != currentEntity->material) {
                currentMaterial = currentEntity->material;
                
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
            
            
            if (currentEntity->rigidBody != nullptr) {
                rp3d::Transform physicsTransform = currentEntity->rigidBody->getTransform();
                
                rp3d::Vector3    pos  = physicsTransform.getPosition();
                rp3d::Quaternion quat = physicsTransform.getOrientation();
                rp3d::Vector3    rot  = quat.getVectorV();
                
                currentEntity->transform.position.x = pos.x;
                currentEntity->transform.position.y = pos.y;
                currentEntity->transform.position.z = pos.z;
                
                currentEntity->transform.rotation.x = (rot.x * 180) / 3.14159 ;
                currentEntity->transform.rotation.y = (rot.y * 180) / 3.14159 ;
                currentEntity->transform.rotation.z = (rot.z * 180) / 3.14159 ;
            }
            
            glm::mat4 model(1);
            
            Transform parent;
            if (currentEntity->transform.parent != nullptr) 
                parent = *currentEntity->transform.parent;
            
            model = CalculateModelMatrix(parent, currentEntity->transform);
            
            currentShader->SetModelMatrix(model);
            
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


void RenderSystem :: Initiate(void) {
    
    defaultShader = CreateShader();
    defaultShader->BuildDefault();
    
    defaultMaterial = CreateMaterial();
    defaultMaterial->color = Color(1,1,1,1);
    
    
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        for (std::vector<Entity*>::iterator it = scenePtr->entities.begin(); it != scenePtr->entities.end(); ++it) {
            
            Entity* currentEntity = *it;
            
            if (currentEntity->script != nullptr) 
                currentEntity->script->OnCreate();
            
            
        }
        
    }
    
#ifdef _RENDERER_CHECK_OPENGL_ERRORS__
    GetGLErrorCodes("OnInitiate::");
#endif
    
    return;
}




void RenderSystem :: AddToRenderQueue(Scene* scenePtr) {
    renderQueue.push_back( scenePtr );
}

bool RenderSystem :: RemoveFromRenderQueue(Scene* scenePtr) {
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        Scene* thisScenePtr = *it;
        if (scenePtr == thisScenePtr) {
            renderQueue.erase(it);
            return true;
        }
    }
    return false;
}

unsigned int RenderSystem :: GetRenderQueueSize(void) {
    return renderQueue.size();
}

Scene* RenderSystem :: GetRenderQueueScene(unsigned int index) {
    return renderQueue[index];
}




GLenum RenderSystem :: SetRenderTarget(HWND wHndl) {
    
    int iFormat;
    std::string gcVendor, gcRenderer, gcExtensions, gcVersion, Line;
    
    // Set the window handle and get the device context
    windowHandle = wHndl;
    HDC hDC = GetDC(wHndl);
    deviceContext = hDC;
    
    // Get display size
    displaySize.x = GetDeviceCaps(deviceContext, HORZRES);
    displaySize.y = GetDeviceCaps(deviceContext, VERTRES);
    displayCenter.x = displaySize.x / 2;
    displayCenter.y = displaySize.y / 2;
    
    // Get window size
    RECT wRect;
    GetWindowRect(windowHandle, &wRect);
    
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
    
    // Setup pixel format
    iFormat = ChoosePixelFormat(deviceContext, &pfd);
    SetPixelFormat(deviceContext, iFormat, &pfd);
    
    HGLRC hRC = wglCreateContext(hDC);
    renderContext = hRC;
    
    wglMakeCurrent(deviceContext, hRC);
    
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

void RenderSystem :: ReleaseRenderTarget(void) {
    
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext(renderContext);
    
    ReleaseDC(windowHandle, deviceContext);
    
    return;
}

void RenderSystem :: SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    viewport.x = x;
    viewport.y = y;
    viewport.w = w;
    viewport.h = h;
}


glm::mat4 RenderSystem :: CalculateModelMatrix(Transform& parentTransform, Transform& modelTransform) {
    
    glm::mat4 parentTranslation = glm::translate(glm::mat4(1.0f), glm::vec3( parentTransform.position.x, parentTransform.position.y, parentTransform.position.z ));
    
    glm::mat4 
    parentRotation = glm::rotate(glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(0, 1, 0));
    parentRotation = glm::rotate(parentRotation, parentTransform.rotation.x, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    parentRotation = glm::rotate(parentRotation, parentTransform.rotation.y, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    parentRotation = glm::rotate(parentRotation, parentTransform.rotation.z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    
    glm::mat4 parentScale = glm::scale(glm::mat4(1.0f), glm::vec3( parentTransform.scale.x, parentTransform.scale.y, parentTransform.scale.z ));
    
    glm::mat4 modelTranslation = glm::translate(glm::mat4(1.0f), glm::vec3( modelTransform.position.x, modelTransform.position.y, modelTransform.position.z ));
    
    glm::mat4 
    modelRotation = glm::rotate (glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(0, 1, 0));
    modelRotation = glm::rotate(modelRotation, modelTransform.rotation.x, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    modelRotation = glm::rotate(modelRotation, modelTransform.rotation.y, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    modelRotation = glm::rotate(modelRotation, modelTransform.rotation.z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    
    glm::mat4 modelScale = glm::scale(glm::mat4(1.0f), glm::vec3( modelTransform.scale.x, modelTransform.scale.y, modelTransform.scale.z ));
    
    return (parentTranslation * parentRotation * parentScale) * (modelTranslation * modelRotation * modelScale);
}



// Log openGL errors
std::vector<std::string> RenderSystem :: GetGLErrorCodes(std::string errorLocationString) {
    
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


