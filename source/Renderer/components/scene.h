

struct Scene {
    
    bool isRenderable;
    
    std::vector<Entity*>  entityQueue;
    
    Scene() {
        
        isRenderable = true;
        
    }
    
    
    void AddToSceneRoot(Entity* entity);
    bool RemoveFromSceneRoot(Entity* entity);
    unsigned int GetRenderQueueSize(void);
    Entity* GetRenderQueueEntity(unsigned int index);
    
};




void Scene::AddToSceneRoot(Entity* entity) {
    entityQueue.push_back( entity );
}


bool Scene::RemoveFromSceneRoot(Entity* entity) {
    for (std::vector<Entity*>::iterator it = entityQueue.begin(); it != entityQueue.end(); ++it) {
        Entity* entityPtr = *it;
        if (entity == entityPtr) {
            entityQueue.erase(it);
            return true;
        }
    }
    return false;    
}


unsigned int Scene::GetRenderQueueSize(void) {
    return entityQueue.size();
}


Entity* Scene::GetRenderQueueEntity(unsigned int index) {
    return entityQueue[index];
}


