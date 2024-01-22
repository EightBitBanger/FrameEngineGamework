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

Texture* RenderSystem::CreateTexture(void) {
    Texture* texturePtr = mTexture.Create();
    return texturePtr;
}
bool RenderSystem::DestroyTexture(Texture* texturePtr) {
    return mTexture.Destroy(texturePtr);
}

FrameBuffer* RenderSystem::CreateFrameBuffer(void) {
    FrameBuffer* frameBufferPtr = mFrameBuffer.Create();
    return frameBufferPtr;
}
bool RenderSystem::DestroyFrameBuffer(FrameBuffer* frameBufferPtr) {
    return mFrameBuffer.Destroy(frameBufferPtr);
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



