struct Viewport {int x, y, w, h;};

#include "types/transform.h"
#include "types/color.h"

#include "components/shader.h"
#include "components/material.h"
#include "components/mesh.h"

#include "components/script.h"
#include "components/camera.h"
#include "components/entity.h"

#include "components/sky.h"

#include "components/scene.h"



class RenderSystem {
    
    HWND  windowHandle;
    HDC   deviceContext;
    HGLRC renderContext;
    
    std::vector<Scene*>  renderQueue;
    
    Mesh*      currentMesh;
    Material*  currentMaterial;
    Shader*    currentShader;
    
    struct DEPTHTEST {
        void Enable(void) {glEnable(GL_DEPTH_TEST);}
        void Disable(void) {glDisable(GL_DEPTH_TEST);}
        void EnableMask(void) {glDepthMask(GL_TRUE);}
        void DisableMask(void) {glDepthMask(GL_FALSE);}
        void SetFunction(GLenum Func) {glDepthFunc(Func);}
    };
    struct STENCILTEST {
        void Enable(void) {glEnable(GL_STENCIL_TEST);}
        void Disable(void) {glDisable(GL_STENCIL_TEST);}
        void SetMask(GLenum Mask) {glStencilMask(Mask);}
    };
    struct FACECULLING {
        void Enable(void) {glEnable(GL_CULL_FACE);}
        void Disable(void) {glDisable(GL_CULL_FACE);}
        void SetCullSide(bool Side) {if (Side) glCullFace(GL_BACK); else glCullFace(GL_FRONT);}
        void SetFaceWinding(bool Wind) {if (Wind) glFrontFace(GL_CCW); else glFrontFace(GL_CW);}
    };
    struct BLENDING {
        void Enable(void) {glEnable(GL_BLEND);}
        void Disable(void) {glDisable(GL_BLEND);}
        void SetFunction(GLenum source, GLenum destination) {glBlendFunc(source, destination);}
        void SetFunctionSeperate(GLenum alphaSource, GLenum alphaDest, GLenum source, GLenum destination) {glBlendFuncSeparate(alphaSource, alphaDest, source, destination);}
    };
    
    PoolAllocator<Entity>    entity;
    PoolAllocator<Mesh>      mesh;
    PoolAllocator<Shader>    shader;
    PoolAllocator<Camera>    camera;
    PoolAllocator<Material>  material;
    PoolAllocator<Sky>       sky;
    PoolAllocator<Scene>     scene;
    PoolAllocator<Script>    script;
    
public:
    
    DEPTHTEST    DepthTest;
    STENCILTEST  StencilTest;
    FACECULLING  FaceCulling;
    BLENDING     Blending;
    
    Material*  defaultMaterial;
    Shader*    defaultShader;
    
    Sky*    skyMain;
    Camera* cameraMain;
    
    Viewport viewport;
    
    glm::vec2 displaySize;
    glm::vec2 displayCenter;
    
    Scene* operator[] (int const i) {return renderQueue[i];}
    
    RenderSystem() {
        
        windowHandle   = NULL;
        deviceContext  = NULL;
        renderContext  = NULL;
        
        renderQueue.clear();
        
        currentMesh      = nullptr;
        currentMaterial  = nullptr;
        currentShader    = nullptr;
        
        cameraMain = nullptr;
        
    }
    
    
    Entity* CreateEntity(void)             {
        Entity* entityPtr = entity.Create();
        entityPtr->material = defaultMaterial;
        return entityPtr;
    }
    void DestroyEntity(Entity* entityPtr)  {
        if (entityPtr->material != nullptr) material.Destroy(entityPtr->material);
        if (entityPtr->script != nullptr) script.Destroy(entityPtr->script);
        entity.Destroy(entityPtr);
    }
    
    Mesh* CreateMesh(void)           {return mesh.Create();}
    void DestroyMesh(Mesh* meshPtr)  {mesh.Destroy(meshPtr);}
    
    Shader* CreateShader(void)             {return shader.Create();}
    void DestroyShader(Shader* shaderPtr)  {shader.Destroy(shaderPtr);}
    
    Camera* CreateCamera(void)             {return camera.Create();}
    void DestroyCamera(Camera* cameraPtr)  {camera.Destroy(cameraPtr);}
    
    Material* CreateMaterial(void)               {return material.Create();}
    void DestroyMaterial(Material* materialPtr)  {material.Destroy(materialPtr);}
    
    Sky* CreateSky(void)          {return sky.Create();}
    void DestroySky(Sky* skyPtr)  {sky.Destroy(skyPtr);}
    
    Scene* CreateScene(void)            {return scene.Create();}
    void DestroyScene(Scene* scenePtr)  {scene.Destroy(scenePtr);}
    
    Script* CreateScript(void)             {return script.Create();}
    void DestroyScript(Script* scriptPtr)  {script.Destroy(scriptPtr);}
    
    
    
    GLenum  SetRenderTarget(HWND wHndl);
    void    ReleaseRenderTarget(void);
    
    void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    
    void RenderFrame(float deltaTime);
    void Initiate(void);
    
    void AddToRenderQueue(Scene* scene);
    bool RemoveFromRenderQueue(Scene* scene);
    unsigned int GetRenderQueueSize(void);
    Scene* GetRenderQueueScene(unsigned int index);
    
    glm::mat4 CalculateModelMatrix(Transform& parentTransform, Transform& modelTransform);
    
};








void RenderSystem :: RenderFrame(float deltaTime) {
    
    if (cameraMain == nullptr) 
        return;
    
    // Set and clear view port
    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set render sky
    if (skyMain != nullptr) {
        Color color = skyMain->background;
        glClearColor(color.r, color.g, color.b, 1);
    }
    
    
    
    // Update camera
    if (cameraMain->script != nullptr) 
          cameraMain->script->OnUpdate();
    
    // Mouse look
    if (cameraMain->useMouseLook) 
        cameraMain->MouseLook(deltaTime, displayCenter.x, displayCenter.y);
    
    // Calculate projection matrix
    glm::mat4 projection = cameraMain->CalculatePerspectiveMatrix();
    glm::mat4 view = cameraMain->CalculateView();
    
    glm::mat4 viewProj = projection * view;
    currentShader = nullptr;
    
    //
    // Process scene queues
    
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        if (!scenePtr->isRenderable) 
            continue;
        
        if (scenePtr->entityQueue.size() == 0)
            continue;
        
        //
        // Run the entity list
        
        for (std::vector<Entity*>::iterator it = scenePtr->entityQueue.begin(); it != scenePtr->entityQueue.end(); ++it) {
            
            Entity* currentEntity = *it;
            
            // Update native script
            if (currentEntity->script != nullptr) 
                currentEntity->script->OnUpdate();
            
            
            if (currentEntity->mesh == nullptr) continue;
            Mesh* mesh = currentEntity->mesh;
            
            if (mesh->shader == nullptr) continue;
            Shader* shader = mesh->shader;
            
            
            // Update render bindings
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
                
                currentShader->Bind();
                currentShader->SetMaterialColor(currentMaterial->color);
                currentShader->SetTextureSampler(0);
                
            }
            
            // Sync with the rigid body
            if (currentEntity->rigidBody != nullptr) 
                currentEntity->SyncRigidBody();
            
            
            //
            // Calculate model matrix
            glm::mat4 model = glm::mat4(1);
            
            Transform parent;
            if (currentEntity->transform.parent != nullptr) 
                parent = *currentEntity->transform.parent;
            
            model = CalculateModelMatrix(parent, currentEntity->transform);
            
            currentShader->SetModelMatrix(model);
            
            // Draw call
            mesh->DrawIndexArray();
            
            continue;
        }
        
    }
    
    SwapBuffers(deviceContext);
    
    return;
}


void RenderSystem :: Initiate(void) {
    
    Log.Write(" Renderer initiated"); Log.WriteLn();
    
    defaultShader = CreateShader();
    defaultShader->BuildDefault();
    
    defaultMaterial = CreateMaterial();
    defaultMaterial->color = Colors.green;
    
    for (std::vector<Scene*>::iterator it = renderQueue.begin(); it != renderQueue.end(); ++it) {
        
        Scene* scenePtr = *it;
        
        for (std::vector<Entity*>::iterator it = scenePtr->entityQueue.begin(); it != scenePtr->entityQueue.end(); ++it) {
            
            Entity* currentEntity = *it;
            
            if (currentEntity->script != nullptr) 
                currentEntity->script->OnCreate();
            
            
        }
        
    }
    
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
    
    glm::mat4 WorldTranslation = glm::translate(glm::mat4(1.0f), glm::vec3( parentTransform.position.x, parentTransform.position.y, parentTransform.position.z ));
    
    glm::mat4 
    WorldRotation = glm::rotate(glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(0, 1, 0));
    WorldRotation = glm::rotate(WorldRotation, parentTransform.rotation.x, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    WorldRotation = glm::rotate(WorldRotation, parentTransform.rotation.y, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    WorldRotation = glm::rotate(WorldRotation, parentTransform.rotation.z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    
    glm::mat4 WorldScale = glm::scale(glm::mat4(1.0f), glm::vec3( parentTransform.scale.x, parentTransform.scale.y, parentTransform.scale.z ));
    
    glm::mat4 ModelTranslation = glm::translate(glm::mat4(1.0f), glm::vec3( modelTransform.position.x, modelTransform.position.y, modelTransform.position.z ));
    
    glm::mat4 
    ModelRotation = glm::rotate (glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(0, 1, 0));
    ModelRotation = glm::rotate(ModelRotation, modelTransform.rotation.x, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    ModelRotation = glm::rotate(ModelRotation, modelTransform.rotation.y, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    ModelRotation = glm::rotate(ModelRotation, modelTransform.rotation.z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    
    glm::mat4 ModelScale = glm::scale(glm::mat4(1.0f), glm::vec3( modelTransform.scale.x, modelTransform.scale.y, modelTransform.scale.z ));
    
    return (WorldTranslation * WorldRotation * WorldScale) * (ModelTranslation * ModelRotation * ModelScale);
}



