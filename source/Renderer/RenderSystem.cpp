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
    return;
}





Entity* RenderSystem::CreateEntity(void) {
    Entity* entityPtr = entity.Create();
    entityPtr->AttachMaterial(defaultMaterial);
    return entityPtr;
}
bool RenderSystem::DestroyEntity(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    return entity.Destroy(entityPtr);
}


Mesh* RenderSystem::CreateMesh(void) {
    Mesh* meshPtr = mesh.Create();
    meshPtr->AttachShader(defaultShader);
    return meshPtr;
}
bool RenderSystem::DestroyMesh(Mesh* meshPtr) {
    assert(meshPtr != nullptr);
    return mesh.Destroy(meshPtr);
}


Shader* RenderSystem::CreateShader(void) {
    Shader* shaderPtr = shader.Create();
    return shaderPtr;
}
bool RenderSystem::DestroyShader(Shader* shaderPtr) {
    assert(shaderPtr != nullptr);
    return shader.Destroy(shaderPtr);
}


Camera* RenderSystem::CreateCamera(void) {
    Camera* cameraPtr = camera.Create();
    return cameraPtr;
}
bool RenderSystem::DestroyCamera(Camera* cameraPtr) {
    assert(cameraPtr != nullptr);
    return camera.Destroy(cameraPtr);
}


Material* RenderSystem::CreateMaterial(void) {
    Material* materialPtr = material.Create();
    return materialPtr;
}
bool RenderSystem::DestroyMaterial(Material* materialPtr) {
    assert(materialPtr != nullptr);
    return material.Destroy(materialPtr);
}


Sky* RenderSystem::CreateSky(void) {
    Sky* skyPtr = sky.Create();
    return skyPtr;
}
bool RenderSystem::DestroySky(Sky* skyPtr) {
    assert(skyPtr != nullptr);
    return sky.Destroy(skyPtr);
}


Scene* RenderSystem::CreateScene(void) {
    Scene* scenePtr = scene.Create();
    return scenePtr;
}
bool RenderSystem::DestroyScene(Scene* scenePtr) {
    assert(scenePtr != nullptr);
    return scene.Destroy(scenePtr);
}






void RenderSystem :: Initiate(void) {
    
    defaultShader = CreateShader();
    defaultShader->BuildDefault();
    
    defaultMaterial = CreateMaterial();
    defaultMaterial->color = Color(0, 0, 0, 1);
    
#ifdef _RENDERER_CHECK_OPENGL_ERRORS__
    GetGLErrorCodes("OnInitiate::");
#endif
    
    return;
}



void RenderSystem :: AddToRenderQueue(Scene* scenePtr) {
    assert(scenePtr != nullptr);
    renderQueue.push_back( scenePtr );
    return;
}

bool RenderSystem :: RemoveFromRenderQueue(Scene* scenePtr) {
    assert(scenePtr != nullptr);
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
    assert(index <= renderQueue.size());
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
    return;
}


glm::mat4 RenderSystem :: CalculateModelMatrix(Transform& model) {
    
    glm::mat4 modelTranslation = glm::translate(glm::mat4(1.0f), glm::vec3( model.position.x, model.position.y, model.position.z ));
    
    glm::mat4 
    modelRotation = glm::rotate (glm::mat4(1.0f), glm::radians( 0.0f ), glm::vec3(0, 1, 0));
    modelRotation = glm::rotate(modelRotation, model.rotation.x, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    modelRotation = glm::rotate(modelRotation, model.rotation.y, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    modelRotation = glm::rotate(modelRotation, model.rotation.z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    
    glm::mat4 modelScale = glm::scale(glm::mat4(1.0f), glm::vec3( model.scale.x, model.scale.y, model.scale.z ));
    
    return modelTranslation * modelRotation * modelScale;
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


