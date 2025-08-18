#ifndef __COMPONENT_SCENE
#define __COMPONENT_SCENE

#include <GameEngineFramework/Renderer/components/meshrenderer.h>
#include <GameEngineFramework/Renderer/components/camera.h>
#include <GameEngineFramework/Renderer/components/light.h>
#include <GameEngineFramework/Renderer/components/fog.h>


class ENGINE_API Scene {
    
public:
    
    /// Should lights be updated for this scene.
    bool doUpdateLights;
    
    /// Is this scene currently active in the renderer.
    bool isActive;
    
    /// The camera associated with this scene.
    Camera* camera;
    
    /// Add a mesh renderer to this scene.
    void AddMeshRendererToSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup = RENDER_QUEUE_GEOMETRY);
    
    /// Remove a mesh renderer from this scene.
    bool RemoveMeshRendererFromSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup);
    
    /// Add a light to this scene.
    void AddLightToSceneRoot(Light* light);
    
    /// Remove a light from this scene.
    bool RemoveLightFromSceneRoot(Light* light);
    
    /// Adds a layer of fog to the scene. The index of the
    /// fog layer will be returned.
    void AddFogLayerToScene(Fog* fogLayer);
    
    /// Remove a fog layer from the scene.
    bool RemoveFogLayer(Fog* fogLayer);
    
    friend class RenderSystem;
    
    Scene();
    
private:
    
    // List of mesh renderer queue groups in this scene
    std::vector<MeshRenderer*>  mRenderQueueOverlay;
    std::vector<MeshRenderer*>  mRenderQueueForeground;
    std::vector<MeshRenderer*>  mRenderQueuePostGeometry;
    std::vector<MeshRenderer*>  mRenderQueueGeometry;
    std::vector<MeshRenderer*>  mRenderQueuePreGrometry;
    std::vector<MeshRenderer*>  mRenderQueueBackground;
    std::vector<MeshRenderer*>  mRenderQueueSky;
    
    // List of lights in this scene
    std::vector<Light*>  mLightList;
    
    // Fog layers to blend into this scene
    std::vector<Fog*> mFogLayers;
};

#endif
