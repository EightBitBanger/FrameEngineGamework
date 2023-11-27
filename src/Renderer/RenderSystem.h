#ifndef OPENGL_RENDERER_SYSTEM
#define OPENGL_RENDERER_SYSTEM

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>

#include <thread>
#include <mutex>
#include <chrono>

#define GLEW_STATIC
#include <gl/glew.h>

#include "../../vendor/CodeBaseLibrary/poolallocator.h"
#include "../../vendor/CodeBaseLibrary/types.h"
#include "../../vendor/CodeBaseLibrary/logging.h"

#include "enumerators.h"

#include "../engine/types/viewport.h"

#include "components/shader.h"
#include "components/material.h"
#include "components/mesh.h"
#include "components/camera.h"
#include "components/light.h"
#include "components/meshrenderer.h"
#include "components/scene.h"

#include "../configuration.h"


#define RENDER_COMMAND_STREAM_BUFFER_SIZE   1024 * 16

struct RenderCommandStreamBuffer {
    
    MeshRenderer* meshRenderer;
    
    // Vertex GPU buffer
    unsigned int mVertexArray;
    unsigned int mBufferVertex;
    unsigned int mBufferIndex;
    
    // Texture GPU buffer
    unsigned int mTextureBuffer;
    
};



class __declspec(dllexport) RenderSystem {
    
public:
    
    /// Current rendering view port
    Viewport  viewport;
    
    /// Current size of the display.
    glm::vec2 displaySize;
    
    /// Current center of the display.
    glm::vec2 displayCenter;
    
    /// Recalculate lights every frame.
    bool doUpdateLightsEveryFrame;
    
    
    RenderSystem();
    
    Scene* operator[] (unsigned int const i) {return mRenderQueue[i];}
    
    /// Create a mesh renderer object and return its pointer.
    MeshRenderer* CreateMeshRenderer(void);
    
    /// Destroy a mesh renderer object and return true on success.
    bool DestroyMeshRenderer(MeshRenderer* meshRendererPtr);
    
    /// Create a mesh object and return its pointer.
    Mesh* CreateMesh(void);
    
    /// Destroy a mesh object and return true on success.
    bool DestroyMesh(Mesh* meshPtr);
    
    /// Create a shader object and return its pointer.
    Shader* CreateShader(void);
    
    /// Destroy a shader object and return true on success.
    bool DestroyShader(Shader* shaderPtr);
    
    /// Create a camera object and return its pointer.
    Camera* CreateCamera(void);
    
    /// Destroy a camera object and return true on success.
    bool DestroyCamera(Camera* cameraPtr);
    
    /// Create a material object and return its pointer.
    Material* CreateMaterial(void);
    
    /// Destroy a material object and return true on success.
    bool DestroyMaterial(Material* materialPtr);
    
    /// Create a light object and return its pointer.
    Light* CreateLight(void);
    
    /// Destroy a light object and return true on success.
    bool DestroyLight(Light* lightPtr);
    
    /// Create a scene object and return its pointer.
    Scene* CreateScene(void);
    
    /// Destroy a scene object and return true on success.
    bool DestroyScene(Scene* scenePtr);
    
    /// Prepare the render system.
    void Initiate(void);
    
    /// Shutdown the render system.
    void Shutdown(void);
    
    /// Set the target render context.
    GLenum  SetRenderTarget(HWND wHndl);
    
    /// Free the target render context.
    void ReleaseRenderTarget(void);
    
    /// Set the render area within the display.
    void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    
    /// Draw the current frame as it stands.
    void RenderFrame(void);
    
    /// Add a scene to the render queue for drawing.
    void AddSceneToRenderQueue(Scene* scene);
    
    /// Remove a scene from the render queue.
    bool RemoveSceneFromRenderQueue(Scene* scene);
    
    /// Get the number of scenes in the render queue.
    unsigned int GetRenderQueueSize(void);
    
    /// Return the number of entities that have been created.
    unsigned int GetMeshRendererCount(void);
    
    /// Calculate the model matrix from a given transform.
    glm::mat4 CalculateModelMatrix(Transform& modelTransform);
    
    /// Return a list of any and all openGL error codes.
    std::vector<std::string> GetGLErrorCodes(std::string errorLocationString);
    
    /// Get number of draw calls made in the last frame.
    unsigned int GetNumberOfDrawCalls(void);
    
private:
    
    // Draw call counter
    unsigned int mNumberOfDrawCalls;
    
    // Device render context
    HWND  mWindowHandle;
    HDC   mDeviceContext;
    HGLRC mRenderContext;
    
    // Render queue group
    std::vector<Scene*>  mRenderQueue;
    
    // Current asset bindings
    Mesh*      mCurrentMesh;
    Material*  mCurrentMaterial;
    
    // Light list
    unsigned int mNumberOfLights=0;
    glm::vec3    mLightPosition    [RENDER_NUMBER_OF_LIGHTS];
    glm::vec3    mLightAttenuation [RENDER_NUMBER_OF_LIGHTS];
    glm::vec3    mLightColor       [RENDER_NUMBER_OF_LIGHTS];
    
    // Render component allocators
    PoolAllocator<MeshRenderer>    mEntity;
    PoolAllocator<Mesh>            mMesh;
    PoolAllocator<Material>        mMaterial;
    PoolAllocator<Shader>          mShader;
    PoolAllocator<Camera>          mCamera;
    PoolAllocator<Light>           mLight;
    PoolAllocator<Scene>           mScene;
    
    // Render support thread
    std::thread* renderThreadMain;
    
    
    // Render pipeline
    
    bool setTargetCamera(Camera* currentCamera, glm::vec3& eye, glm::mat4& viewProjection);
    
    void accumulateSceneLights(Scene* currentScene, glm::vec3 eye, unsigned int& numberOfActiveLights);
    
};

#endif
