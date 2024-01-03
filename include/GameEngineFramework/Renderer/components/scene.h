#ifndef __COMPONENT_SCENE
#define __COMPONENT_SCENE

#include <GameEngineFramework/Renderer/components/meshrenderer.h>
#include <GameEngineFramework/Renderer/components/camera.h>
#include <GameEngineFramework/Renderer/components/light.h>


class ENGINE_API Scene {
    
public:
    
    /// Should lights be updated.
    bool doUpdateLights;
    
    /// Is this scene currently active in the renderer.
    bool isActive;
    
    /// The camera associated with this scene.
    Camera* camera;
    
    
    /// Add a mesh renderer to this scene.
    void AddMeshRendererToSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup = RENDER_QUEUE_DEFAULT);
    
    /// Remove a mesh renderer from this scene.
    bool RemoveMeshRendererFromSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup);
    
    /// Add a light to this scene.
    void AddLightToSceneRoot(Light* light);
    
    /// Remove a light from this scene.
    bool RemoveLightFromSceneRoot(Light* light);
    
    
    friend class RenderSystem;
    
    Scene();
    
    
private:
    
    /// List of mesh renderer queue groups in this scene.
    std::vector<MeshRenderer*>  mRenderQueueOverlay;
    std::vector<MeshRenderer*>  mRenderQueueForeground;
    std::vector<MeshRenderer*>  mRenderQueueDefault;
    std::vector<MeshRenderer*>  mRenderQueueBackground;
    std::vector<MeshRenderer*>  mRenderQueueSky;
    
    /// List of lights in this scene.
    std::vector<Light*>  mLightList;
    
};

#endif
