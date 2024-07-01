#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>

#include <iostream>

extern Logger Log;

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
    mNumberOfFrames(0),
    
    mCurrentMesh(nullptr),
    mCurrentMaterial(nullptr),
    mCurrentShader(nullptr),
    
    mNumberOfLights(0),
    mNumberOfShadows(0),
    
    mShadowDistance(300)
{
}

MeshRenderer* RenderSystem::CreateMeshRenderer(void) {
    MeshRenderer* meshRendererPtr = mEntity.Create();
    return meshRendererPtr;
}
bool RenderSystem::DestroyMeshRenderer(MeshRenderer* meshRendererPtr) {
    if (meshRendererPtr->mesh != nullptr) 
        if (meshRendererPtr->mesh->isShared == false) 
            mMesh.Destroy(meshRendererPtr->mesh);
    if (meshRendererPtr->material != nullptr) 
        if (meshRendererPtr->material->isShared == false) 
            mMaterial.Destroy(meshRendererPtr->material);
    return mEntity.Destroy(meshRendererPtr);
}

unsigned int RenderSystem::GetNumberOfMeshRenderers(void) {
    return mEntity.Size();
}

Mesh* RenderSystem::CreateMesh(void) {
    Mesh* meshPtr = mMesh.Create();
    return meshPtr;
}
bool RenderSystem::DestroyMesh(Mesh* meshPtr) {
    return mMesh.Destroy(meshPtr);
}
unsigned int RenderSystem::GetNumberOfMeshes(void) {
    return mMesh.Size();
}

Shader* RenderSystem::CreateShader(void) {
    Shader* shaderPtr = mShader.Create();
    return shaderPtr;
}
bool RenderSystem::DestroyShader(Shader* shaderPtr) {
    return mShader.Destroy(shaderPtr);
}
unsigned int RenderSystem::GetNumberOfShaders(void) {
    return mShader.Size();
}

Camera* RenderSystem::CreateCamera(void) {
    Camera* cameraPtr = mCamera.Create();
    cameraPtr->viewport = viewport;
    return cameraPtr;
}
bool RenderSystem::DestroyCamera(Camera* cameraPtr) {
    return mCamera.Destroy(cameraPtr);
}
unsigned int RenderSystem::GetNumberOfCameras(void) {
    return mCamera.Size();
}

Material* RenderSystem::CreateMaterial(void) {
    Material* materialPtr = mMaterial.Create();
    return materialPtr;
}
bool RenderSystem::DestroyMaterial(Material* materialPtr) {
    return mMaterial.Destroy(materialPtr);
}
unsigned int RenderSystem::GetNumberOfMaterials(void) {
    return mMaterial.Size();
}

Light* RenderSystem::CreateLight(void) {
    Light* lightPtr = mLight.Create();
    return lightPtr;
}
bool RenderSystem::DestroyLight(Light* lightPtr) {
    return mLight.Destroy(lightPtr);
}
unsigned int RenderSystem::GetNumberOfLights(void) {
    return mLight.Size();
}

Scene* RenderSystem::CreateScene(void) {
    Scene* scenePtr = mScene.Create();
    return scenePtr;
}
bool RenderSystem::DestroyScene(Scene* scenePtr) {
    return mScene.Destroy(scenePtr);
}
unsigned int RenderSystem::GetNumberOfScenes(void) {
    return mScene.Size();
}

Texture* RenderSystem::CreateTexture(void) {
    Texture* texturePtr = mTexture.Create();
    return texturePtr;
}
bool RenderSystem::DestroyTexture(Texture* texturePtr) {
    return mTexture.Destroy(texturePtr);
}
unsigned int RenderSystem::GetNumberOfTextures(void) {
    return mTexture.Size();
}

FrameBuffer* RenderSystem::CreateFrameBuffer(void) {
    FrameBuffer* frameBufferPtr = mFrameBuffer.Create();
    return frameBufferPtr;
}
bool RenderSystem::DestroyFrameBuffer(FrameBuffer* frameBufferPtr) {
    return mFrameBuffer.Destroy(frameBufferPtr);
}
unsigned int RenderSystem::GetNumberOfFrameBuffers(void) {
    return mFrameBuffer.Size();
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
    mActiveScenes.push_back( scenePtr );
    return;
}

bool RenderSystem::RemoveSceneFromRenderQueue(Scene* scenePtr) {
    for (std::vector<Scene*>::iterator it = mActiveScenes.begin(); it != mActiveScenes.end(); ++it) {
        Scene* thisScenePtr = *it;
        if (scenePtr == thisScenePtr) {
            mActiveScenes.erase(it);
            return true;
        }
    }
    return false;
}

unsigned int RenderSystem::GetRenderQueueSize(void) {
    return mActiveScenes.size();
}

void RenderSystem::SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    viewport.x = x;
    viewport.y = y;
    viewport.w = w;
    viewport.h = h;
    return;
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



