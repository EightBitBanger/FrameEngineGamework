#include "scene.h"


Scene::Scene() : 
    doUpdateLights(true)
{
}

void Scene::AddEntityToSceneRoot(Entity* entity) {
    mEntityList.push_back( entity );
    return;
}

bool Scene::RemoveEntityFromSceneRoot(Entity* entity) {
    for (std::vector<Entity*>::iterator it = mEntityList.begin(); it != mEntityList.end(); ++it) {
        Entity* entityPtr = *it;
        if (entity == entityPtr) {
            mEntityList.erase(it);
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

unsigned int Scene::GetEntityQueueSize(void) {
    return mEntityList.size();
}

unsigned int Scene::GetLightQueueSize(void) {
    return mLightList.size();
}

Entity* Scene::GetEntity(unsigned int index) {
    if (index > mEntityList.size()) 
        return nullptr;
    return mEntityList[index];
}

Light* Scene::GetLight(unsigned int index) {
    if (index > mLightList.size()) 
        return nullptr;
    return mLightList[index];
}

