#ifndef COMPONENT_SCENE
#define COMPONENT_SCENE

#include <vector>

#include "entity.h"

class Scene {
    
public:
    
    Entity* GetEntity(unsigned int index);
    
    void AddToSceneRoot(Entity* entity);
    bool RemoveFromSceneRoot(Entity* entity);
    
    unsigned int GetRenderQueueSize(void);
    Entity* GetRenderQueueEntity(unsigned int index);
    
    std::vector<Entity*>  entities;
    
};

#endif
