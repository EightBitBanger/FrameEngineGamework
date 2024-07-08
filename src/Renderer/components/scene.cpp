#include <GameEngineFramework/Renderer/components/scene.h>


Scene::Scene() : 
    doUpdateLights(true),
    isActive(true),
    camera(nullptr)
{
}

void Scene::AddMeshRendererToSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup) {
    switch (renderQueueGroup) {
        case RENDER_QUEUE_OVERLAY:
            mRenderQueueOverlay.emplace( mRenderQueueOverlay.begin(), meshRenderer );
            break;
        case RENDER_QUEUE_FOREGROUND:
            mRenderQueueForeground.emplace( mRenderQueueForeground.begin(), meshRenderer );
            break;
        default:
        case RENDER_QUEUE_DEFAULT:
            mRenderQueueDefault.emplace( mRenderQueueDefault.begin(), meshRenderer );
            break;
        case RENDER_QUEUE_BACKGROUND:
            mRenderQueueBackground.emplace( mRenderQueueBackground.begin(), meshRenderer );
            break;
        case RENDER_QUEUE_SKY:
            mRenderQueueSky.emplace( mRenderQueueSky.begin(), meshRenderer );
            break;
    }
    return;
}

bool Scene::RemoveMeshRendererFromSceneRoot(MeshRenderer* meshRenderer, int renderQueueGroup) {
    
    std::vector<MeshRenderer*>* renderQueue = nullptr;
    
    switch (renderQueueGroup) {
        case RENDER_QUEUE_OVERLAY:    renderQueue = &mRenderQueueOverlay; break;
        case RENDER_QUEUE_FOREGROUND: renderQueue = &mRenderQueueForeground; break;
        default: 
        case RENDER_QUEUE_DEFAULT:    renderQueue = &mRenderQueueDefault; break;
        case RENDER_QUEUE_BACKGROUND: renderQueue = &mRenderQueueBackground; break;
        case RENDER_QUEUE_SKY:        renderQueue = &mRenderQueueSky; break;
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

