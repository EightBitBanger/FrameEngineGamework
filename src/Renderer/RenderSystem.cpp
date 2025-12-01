#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>
#include <GameEngineFramework/Engine/types/color.h>

#include <iostream>

CustomAllocator EntityPool   {1024, 1};
CustomAllocator MaterialPool {1024, 1};
CustomAllocator MeshPool     {512, 1};
CustomAllocator LightPool    {512, 1};

extern Logger Log;
extern IntType Int;
extern ColorPreset  Colors;

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
    
    mShadowDistance(300),
    
    mEntity(EntityPool),
    mMesh(MeshPool),
    mMaterial(MaterialPool),
    mLight(LightPool)
{
}

MeshRenderer* RenderSystem::CreateMeshRenderer(void) {
    return mEntity.Create();
}

bool RenderSystem::DestroyMeshRenderer(MeshRenderer* meshRendererPtr) {
    if (meshRendererPtr->mesh != nullptr) 
        if (meshRendererPtr->mesh->isShared == false) 
            mMesh.Destroy(meshRendererPtr->mesh);
    
    for (unsigned int i=0; i < meshRendererPtr->mLods.size(); i++) {
        LevelOfDetail& lod = meshRendererPtr->mLods[i];
        Mesh* meshPtr = lod.mesh;
        if (meshPtr->isShared == false) 
            mMesh.Destroy(meshPtr);
    }
    meshRendererPtr->mLods.clear();
    
    if (meshRendererPtr->material != nullptr) 
        if (meshRendererPtr->material->isShared == false) 
            mMaterial.Destroy(meshRendererPtr->material);
    
    return mEntity.Destroy(meshRendererPtr);
}

unsigned int RenderSystem::GetNumberOfMeshRenderers(void) {
    return mEntity.Size();
}

Mesh* RenderSystem::CreateMesh(void) {
    return mMesh.Create();
}
bool RenderSystem::DestroyMesh(Mesh* meshPtr) {
    return mMesh.Destroy(meshPtr);
}
unsigned int RenderSystem::GetNumberOfMeshes(void) {
    return mMesh.Size();
}

Shader* RenderSystem::CreateShader(void) {
    return mShader.Create();
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
    return mMaterial.Create();
}
bool RenderSystem::DestroyMaterial(Material* materialPtr) {
    return mMaterial.Destroy(materialPtr);
}
unsigned int RenderSystem::GetNumberOfMaterials(void) {
    return mMaterial.Size();
}

Light* RenderSystem::CreateLight(void) {
    return mLight.Create();
}
bool RenderSystem::DestroyLight(Light* lightPtr) {
    return mLight.Destroy(lightPtr);
}
unsigned int RenderSystem::GetNumberOfLights(void) {
    return mLight.Size();
}

Scene* RenderSystem::CreateScene(void) {
    return mScene.Create();
}
bool RenderSystem::DestroyScene(Scene* scenePtr) {
    return mScene.Destroy(scenePtr);
}
unsigned int RenderSystem::GetNumberOfScenes(void) {
    return mScene.Size();
}

Texture* RenderSystem::CreateTexture(void) {
    return mTexture.Create();
}
bool RenderSystem::DestroyTexture(Texture* texturePtr) {
    return mTexture.Destroy(texturePtr);
}
unsigned int RenderSystem::GetNumberOfTextures(void) {
    return mTexture.Size();
}

FrameBuffer* RenderSystem::CreateFrameBuffer(void) {
    return mFrameBuffer.Create();
}
bool RenderSystem::DestroyFrameBuffer(FrameBuffer* frameBufferPtr) {
    return mFrameBuffer.Destroy(frameBufferPtr);
}
unsigned int RenderSystem::GetNumberOfFrameBuffers(void) {
    return mFrameBuffer.Size();
}

Fog* RenderSystem::CreateFog(void) {
    return mFog.Create();
}

bool RenderSystem::DestroyFog(Fog* fogLayer) {
    return mFog.Destroy(fogLayer);
}

unsigned int RenderSystem::GetNumberOfFogLayers(void) {
    return mFog.Size();
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
    std::lock_guard<std::mutex> lock(mux);
    mActiveScenes.push_back(scenePtr);
    return;
}

bool RenderSystem::RemoveSceneFromRenderQueue(Scene* scenePtr) {
    std::lock_guard<std::mutex> lock(mux);
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

// Extract frustum planes from view-projection matrix
Frustum RenderSystem::FrustumExtractPlanes(glm::mat4& viewProjMatrix) {
    
    Frustum frustum;
    
    // Left
    frustum.planes[0] = glm::vec4(
        viewProjMatrix[0][3] + viewProjMatrix[0][0],
        viewProjMatrix[1][3] + viewProjMatrix[1][0],
        viewProjMatrix[2][3] + viewProjMatrix[2][0],
        viewProjMatrix[3][3] + viewProjMatrix[3][0]
    );
    
    // Right
    frustum.planes[1] = glm::vec4(
        viewProjMatrix[0][3] - viewProjMatrix[0][0],
        viewProjMatrix[1][3] - viewProjMatrix[1][0],
        viewProjMatrix[2][3] - viewProjMatrix[2][0],
        viewProjMatrix[3][3] - viewProjMatrix[3][0]
    );
    
    // Bottom
    frustum.planes[2] = glm::vec4(
        viewProjMatrix[0][3] + viewProjMatrix[0][1],
        viewProjMatrix[1][3] + viewProjMatrix[1][1],
        viewProjMatrix[2][3] + viewProjMatrix[2][1],
        viewProjMatrix[3][3] + viewProjMatrix[3][1]
    );
    
    // Top
    frustum.planes[3] = glm::vec4(
        viewProjMatrix[0][3] - viewProjMatrix[0][1],
        viewProjMatrix[1][3] - viewProjMatrix[1][1],
        viewProjMatrix[2][3] - viewProjMatrix[2][1],
        viewProjMatrix[3][3] - viewProjMatrix[3][1]
    );
    
    // Near
    frustum.planes[4] = glm::vec4(
        viewProjMatrix[0][3] + viewProjMatrix[0][2],
        viewProjMatrix[1][3] + viewProjMatrix[1][2],
        viewProjMatrix[2][3] + viewProjMatrix[2][2],
        viewProjMatrix[3][3] + viewProjMatrix[3][2]
    );
    
    // Far
    frustum.planes[5] = glm::vec4(
        viewProjMatrix[0][3] - viewProjMatrix[0][2],
        viewProjMatrix[1][3] - viewProjMatrix[1][2],
        viewProjMatrix[2][3] - viewProjMatrix[2][2],
        viewProjMatrix[3][3] - viewProjMatrix[3][2]
    );
    
    // Normalize the planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(frustum.planes[i]));
        frustum.planes[i] /= length;
    }
    
    return frustum;
}

// Check if a point is inside the frustum
bool RenderSystem::FrustumCheckPoint(Frustum& frustum, glm::vec3& point) {
    
    for (int i = 0; i < 6; i++) {
        
        if (glm::dot(glm::vec3(frustum.planes[i]), point) + frustum.planes[i].w < 0) 
            return false;
        
    }
    
    return true;
}

// Check if a bounding box is inside the frustum
bool RenderSystem::FrustumCheckAABB(Frustum& frustum, glm::vec3& min, glm::vec3& max) {
    for (int i = 0; i < 6; i++) {
        
        glm::vec3 positiveVertex = min;
        
        if (frustum.planes[i].x >= 0) positiveVertex.x = max.x;
        if (frustum.planes[i].y >= 0) positiveVertex.y = max.y;
        if (frustum.planes[i].z >= 0) positiveVertex.z = max.z;
        
        if (glm::dot(glm::vec3(frustum.planes[i]), positiveVertex) + frustum.planes[i].w < 0) 
            return false;
        
    }
    return true;
}

std::vector<std::string> RenderSystem::GetGLErrorCodes(std::string errorLocationString) {
    std::lock_guard<std::mutex> lock(mux);
    
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
extern RenderSystem Renderer;

void RenderThreadMain(void) {
    
    while (isThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(100) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(3) );
    Log.Write( " >> Shutting down on thread renderer" );
    
    return;
}



