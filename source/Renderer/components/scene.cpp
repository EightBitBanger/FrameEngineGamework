#include "scene.h"

Entity* Scene::GetEntity(unsigned int index) {
    if (index > entities.size()) 
        return nullptr;
    return entities[index];
}

void Scene::AddToSceneRoot(Entity* entity) {
    entities.push_back( entity );
    return;
}

bool Scene::RemoveFromSceneRoot(Entity* entity) {
    for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        Entity* entityPtr = *it;
        if (entity == entityPtr) {
            entities.erase(it);
            return true;
        }
    }
    return false;    
}


unsigned int Scene::GetRenderQueueSize(void) {
    return entities.size();
}


Entity* Scene::GetRenderQueueEntity(unsigned int index) {
    return entities[index];
}


