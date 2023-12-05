#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <iostream>

extern Logger Log;

#include <GameEngineFramework/Types/types.h>
extern IntType Int;


// Render thread
bool isThreadActive = true;
void RenderThreadMain(void);


RenderSystem::RenderSystem() : 
    viewport(Viewport(0, 0, 0, 0)),
    
    displaySize(glm::vec2(0, 0)),
    displayCenter(glm::vec2(0, 0)),
    
    doUpdateLightsEveryFrame(true),
    
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
    
    Log.Write( " >> Starting thread renderer" );
    
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
    Line = DetailStringHead + "Color" + DetailStringEqu + Int.ToString(pfd.cColorBits) + " bit"; Log.Write(Line);
    Line = DetailStringHead + "Depth" + DetailStringEqu + Int.ToString(pfd.cDepthBits) + " bit"; Log.Write(Line);
    Log.WriteLn();
    Log.WriteLn();
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
        
        std::cout << ErrorMsg << "\n";
        
        // Get the next error
        glError = glGetError();
    }
    
    return ErrorList;
}


unsigned int RenderSystem::GetNumberOfDrawCalls(void) {
    return mNumberOfDrawCalls;
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
    Log.Write( " >> Shutting down on thread renderer" );
    
    return;
}



