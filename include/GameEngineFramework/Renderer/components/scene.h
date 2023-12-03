#ifndef __COMPONENT_SCENE
#define __COMPONENT_SCENE

#include "meshrenderer.h"
#include "camera.h"
#include "light.h"


class __declspec(dllexport) Scene {
    
public:
    
    friend class RenderSystem;
    
    /// Should lights be updated.
    bool doUpdateLights;
    
    /// The camera associated with this scene.
    Camera* camera;
    
    Scene();
    
    /// Add a mesh renderer to this scene.
    void AddMeshRendererToSceneRoot(MeshRenderer* MeshRenderer);
    /// Remove a mesh renderer from this scene.
    bool RemoveMeshRendererFromSceneRoot(MeshRenderer* MeshRenderer);
    
    /// Add a light to this scene.
    void AddLightToSceneRoot(Light* light);
    /// Remove a light from this scene.
    bool RemoveLightFromSceneRoot(Light* light);
    
    /// Return the number of mesh renderers in this scene.
    unsigned int GetMeshRendererQueueSize(void);
    
    /// Return the number of lights in this scene.
    unsigned int GetLightQueueSize(void);
    
    /// Return a mesh renderer from an index location in the entity list.
    MeshRenderer* GetMeshRenderer(unsigned int index);
    
    /// Return a light from an index location in the light list.
    Light* GetLight(unsigned int index);
    
    
private:
    
    /// List of mesh renderers in this scene.
    std::vector<MeshRenderer*>  mMeshRendererList;
    
    /// List of lights in this scene.
    std::vector<Light*>  mLightList;
    
};

#endif
