#ifndef OPENGL_RENDERER_SYSTEM
#define OPENGL_RENDERER_SYSTEM

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>

#include <GameEngineFramework/Renderer/enumerators.h>

#include <GameEngineFramework/Renderer/components/camera.h>
#include <GameEngineFramework/Renderer/components/light.h>
#include <GameEngineFramework/Renderer/components/material.h>
#include <GameEngineFramework/Renderer/components/mesh.h>
#include <GameEngineFramework/Renderer/components/meshrenderer.h>
#include <GameEngineFramework/Renderer/components/scene.h>
#include <GameEngineFramework/Renderer/components/shader.h>
#include <GameEngineFramework/Renderer/components/framebuffer.h>
#include <GameEngineFramework/Renderer/components/texture.h>

#include <GameEngineFramework/Engine/types/viewport.h>

#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/configuration.h>

#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>

#define GLEW_STATIC
#include "../../../vendor/gl/glew.h"







class ENGINE_API RenderSystem {
    
public:
    
    /// Current rendering view port
    Viewport  viewport;
    
    /// Size of the display.
    glm::vec2 displaySize;
    
    /// Center point of the display.
    glm::vec2 displayCenter;
    
    /// Recalculate lights every frame.
    bool doUpdateLightsEveryFrame;
    
    
    RenderSystem();
    
    Scene* operator[] (unsigned int const i) {return mRenderQueue[i];}
    
    // Components
    
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
    
    /// Create a texture object and return its pointer.
    Texture* CreateTexture(void);
    
    /// Destroy a texture object and return true on success.
    bool DestroyTexture(Texture* texturePtr);
    
    /// Create a frame buffer object and return its pointer.
    FrameBuffer* CreateFrameBuffer(void);
    
    /// Destroy a scene object and return true on success.
    bool DestroyFrameBuffer(FrameBuffer* frameBufferPtr);
    
    // Render queue
    
    /// Add a scene to the render queue for rendering.
    void AddSceneToRenderQueue(Scene* scenePtr);
    
    /// Remove a scene from the render queue.
    bool RemoveSceneFromRenderQueue(Scene* scene);
    
    /// Get the number of scenes in the render queue.
    unsigned int GetRenderQueueSize(void);
    
    /// Return the number of mesh renderers that have been created.
    unsigned int GetMeshRendererCount(void);
    
    
    // Internal
    
    /// Prepare the render system.
    void Initiate(void);
    
    /// Shutdown the render system.
    void Shutdown(void);
    
    /// Set the render area within the display.
    void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    
    /// Draw the current frame as it stands.
    void RenderFrame(void);
    
    /// Return a list of any and all openGL error codes.
    std::vector<std::string> GetGLErrorCodes(std::string errorLocationString);
    
    /// Get number of draw calls made in the last frame.
    unsigned int GetNumberOfDrawCalls(void);
    
    
    friend class EngineSystemManager;
    
    
public:
    
    // Draw call counter
    unsigned int mNumberOfDrawCalls;
    
    // Frame counter
    unsigned long long int mNumberOfFrames;
    
    // Render queue group
    std::vector<Scene*>  mRenderQueue;
    
    // Current asset bindings
    Mesh*      mCurrentMesh;
    Material*  mCurrentMaterial;
    Shader*    mCurrentShader;
    
    // Sorting
    std::vector<std::pair<float, MeshRenderer*>> mRenderQueueSorter[5];
    
    // Light list
    unsigned int mNumberOfLights=0;
    glm::vec3    mLightPosition    [RENDER_NUMBER_OF_LIGHTS];
    glm::vec3    mLightDirection   [RENDER_NUMBER_OF_LIGHTS];
    glm::vec4    mLightAttenuation [RENDER_NUMBER_OF_LIGHTS];
    glm::vec3    mLightColor       [RENDER_NUMBER_OF_LIGHTS];
    
    // Shadow list
    unsigned int mNumberOfShadows=0;
    glm::vec3    mShadowPosition    [RENDER_NUMBER_OF_SHADOWS];
    glm::vec3    mShadowDirection   [RENDER_NUMBER_OF_SHADOWS];
    glm::vec4    mShadowAttenuation [RENDER_NUMBER_OF_SHADOWS];
    glm::vec3    mShadowColor       [RENDER_NUMBER_OF_SHADOWS];
    
    // Shadows parameters
    float        mShadowDistance;
    Transform    mShadowTransform;
    
    // Render component allocators
    PoolAllocator<MeshRenderer>    mEntity;
    PoolAllocator<Mesh>            mMesh;
    PoolAllocator<Material>        mMaterial;
    PoolAllocator<Shader>          mShader;
    PoolAllocator<Camera>          mCamera;
    PoolAllocator<Light>           mLight;
    PoolAllocator<Scene>           mScene;
    PoolAllocator<FrameBuffer>     mFrameBuffer;
    PoolAllocator<Texture>         mTexture;
    
    // TODO: Sorting and other non openGL related render functions could be threaded out here
    
    // Render support thread
    std::thread* renderThreadMain;
    
    
    //
    // Render pipeline
    //
    
    // Set the camera with which the renderer will draw the frame
    bool setTargetCamera(Camera* currentCamera, glm::vec3& eye, glm::mat4& viewProjection);
    
    // Gather a list of active lights for rendering
    unsigned int accumulateSceneLights(Scene* currentScene, glm::vec3 eye);
    
    // Asset binding
    
    bool BindMesh(Mesh* meshPtr);
    
    bool BindMaterial(Material* materialPtr);
    
    bool BindShader(Shader* shaderPtr);
    
    // Passes
    
    bool GeometryPass(MeshRenderer* currentEntity, glm::vec3& eye, glm::vec3 cameraAngle, glm::mat4& viewProjection);
    
    bool ShadowVolumePass(MeshRenderer* currentEntity, glm::vec3& eye, glm::vec3 cameraAngle, glm::mat4& viewProjection);
    
    bool SortingPass(glm::vec3& eye, std::vector<MeshRenderer*>* renderQueueGroup, unsigned int queueGroupIndex);
    
    
    // Default assets
    
    struct DefaultShaders {
        Shader*  texture;
        Shader*  textureUnlit;
        Shader*  color;
        Shader*  colorUnlit;
        Shader*  UI;
        Shader*  shadowCaster;
        
        DefaultShaders() : 
            texture(nullptr),
            textureUnlit(nullptr),
            color(nullptr),
            colorUnlit(nullptr),
            UI(nullptr),
            shadowCaster(nullptr)
        {}
    };
    
    struct DefaultMeshes {
        Mesh* cube;
        Mesh* plain;
        
        DefaultMeshes() : 
            cube(nullptr),
            plain(nullptr)
        {}
    };
    
public:
    
    /// Default shaders provided by the engine.
    DefaultShaders shaders;
    
    /// Default meshes provided by the engine.
    DefaultMeshes  meshes;
    
};



#endif
