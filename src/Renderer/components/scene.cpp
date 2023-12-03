#include <GameEngineFramework/Renderer/components/scene.h>


Scene::Scene() : 
    doUpdateLights(true),
    camera(nullptr)
{
}

void Scene::AddMeshRendererToSceneRoot(MeshRenderer* meshRenderer) {
    mMeshRendererList.push_back( meshRenderer );
    return;
}

bool Scene::RemoveMeshRendererFromSceneRoot(MeshRenderer* meshRenderer) {
    for (std::vector<MeshRenderer*>::iterator it = mMeshRendererList.begin(); it != mMeshRendererList.end(); ++it) {
        MeshRenderer* entityPtr = *it;
        if (meshRenderer == entityPtr) {
            mMeshRendererList.erase(it);
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

unsigned int Scene::GetMeshRendererQueueSize(void) {
    return mMeshRendererList.size();
}

unsigned int Scene::GetLightQueueSize(void) {
    return mLightList.size();
}

MeshRenderer* Scene::GetMeshRenderer(unsigned int index) {
    if (index > mMeshRendererList.size()) 
        return nullptr;
    return mMeshRendererList[index];
}

Light* Scene::GetLight(unsigned int index) {
    if (index > mLightList.size()) 
        return nullptr;
    return mLightList[index];
}

