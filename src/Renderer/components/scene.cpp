#include <GameEngineFramework/Renderer/components/scene.h>


Scene::Scene() : 
    doUpdateLights(true),
    isActive(true),
    camera(nullptr)
{
}

void Scene::AddMeshRendererToSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup) {
    switch (renderQueueGroup) {
        
        case RENDER_QUEUE_OVERLAY:      mRenderQueueOverlay.emplace( mRenderQueueOverlay.begin(), meshRenderer ); break;
        case RENDER_QUEUE_FOREGROUND:   mRenderQueueForeground.emplace( mRenderQueueForeground.begin(), meshRenderer ); break;
        case RENDER_QUEUE_POSTGEOMETRY: mRenderQueuePostGeometry.emplace( mRenderQueuePostGeometry.begin(), meshRenderer ); break;
        default: 
        case RENDER_QUEUE_GEOMETRY:     mRenderQueueGeometry.emplace( mRenderQueueGeometry.begin(), meshRenderer ); break;
        case RENDER_QUEUE_PREGEOMETRY:  mRenderQueuePreGrometry.emplace( mRenderQueuePreGrometry.begin(), meshRenderer ); break;
        case RENDER_QUEUE_BACKGROUND:   mRenderQueueBackground.emplace( mRenderQueueBackground.begin(), meshRenderer ); break;
        case RENDER_QUEUE_SKY:          mRenderQueueSky.emplace( mRenderQueueSky.begin(), meshRenderer ); break;
        
    }
    return;
}

bool Scene::RemoveMeshRendererFromSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup) {
    std::vector<MeshRenderer*>* renderQueue;
    
    switch (renderQueueGroup) {
        case RENDER_QUEUE_OVERLAY:      renderQueue = &mRenderQueueOverlay; break;
        case RENDER_QUEUE_FOREGROUND:   renderQueue = &mRenderQueueForeground; break;
        case RENDER_QUEUE_POSTGEOMETRY: renderQueue = &mRenderQueuePostGeometry; break;
        default: 
        case RENDER_QUEUE_GEOMETRY:     renderQueue = &mRenderQueueGeometry; break;
        case RENDER_QUEUE_PREGEOMETRY:  renderQueue = &mRenderQueuePreGrometry; break;
        case RENDER_QUEUE_BACKGROUND:   renderQueue = &mRenderQueueBackground; break;
        case RENDER_QUEUE_SKY:          renderQueue = &mRenderQueueSky; break;
    }
    
    for (std::vector<MeshRenderer*>::iterator it = renderQueue->begin(); it != renderQueue->end(); ++it) {
        MeshRenderer* entityPtr = *it;
        if (meshRenderer == entityPtr) {
            renderQueue->erase(it);
            return true;
        }
    }
    return false;
}

void Scene::AddLightToSceneRoot(Light* light) {
    mLightList.push_back( light );
    return;
}

bool Scene::RemoveLightFromSceneRoot(Light* light) {
    for (std::vector<Light*>::iterator it = mLightList.begin(); it != mLightList.end(); ++it) {
        Light* lightPtr = *it;
        if (light == lightPtr) {
            mLightList.erase(it);
            return true;
        }
    }
    return false;
}

unsigned int GetNumberOfFogLayers(void);

void Scene::AddFogLayerToScene(Fog* fogLayer) {
    mFogLayers.push_back( fogLayer );
    return;
}

bool Scene::RemoveFogLayer(Fog* fogLayer) {
    for (std::vector<Fog*>::iterator it = mFogLayers.begin(); it != mFogLayers.end(); ++it) {
        Fog* fogLayerCheck = *it;
        if (fogLayer == fogLayerCheck) {
            mFogLayers.erase(it);
            return true;
        }
    }
    return false;
}

