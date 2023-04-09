#ifndef COMPONENT_SCENE
#define COMPONENT_SCENE

#include <vector>

#include "entity.h"

struct Scene {
    
    std::vector<Entity*>  entityQueue;
    
    void AddToSceneRoot(Entity* entity);
    bool RemoveFromSceneRoot(Entity* entity);
    
    unsigned int GetRenderQueueSize(void);
    Entity* GetRenderQueueEntity(unsigned int index);
    
};

#endif
