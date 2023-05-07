#include "scene.h"

Entity* Scene::GetEntity(unsigned int index) {
    if (index > mEntityList.size()) 
        return nullptr;
    return mEntityList[index];
}

void Scene::AddToSceneRoot(Entity* entity) {
    mEntityList.push_back( entity );
    return;
}

bool Scene::RemoveFromSceneRoot(Entity* entity) {
    for (std::vector<Entity*>::iterator it = mEntityList.begin(); it != mEntityList.end(); ++it) {
        Entity* entityPtr = *it;
        if (entity == entityPtr) {
            mEntityList.erase(it);
            return true;
        }
    }
    return false;    
}


unsigned int Scene::GetRenderQueueSize(void) {
    return mEntityList.size();
}

