#ifndef COMPONENT_SCENE
#define COMPONENT_SCENE

#include <vector>

#include "entity.h"

class Scene {
    
public:
    
    std::vector<Entity*>  entities;
    
    Entity* GetEntity(unsigned int i);
    
    void AddToSceneRoot(Entity* entity);
    bool RemoveFromSceneRoot(Entity* entity);
    
    unsigned int GetRenderQueueSize(void);
    Entity* GetRenderQueueEntity(unsigned int index);
    
};

#endif
