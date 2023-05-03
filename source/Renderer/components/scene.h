#ifndef __COMPONENT_SCENE
#define __COMPONENT_SCENE

#include <vector>

#include "entity.h"


class Scene {
    
public:
    
    /// Add an entity to this scene.
    void AddToSceneRoot(Entity* entity);
    /// Remove an entity from this scene.
    bool RemoveFromSceneRoot(Entity* entity);
    
    /// Return the number of entities in this scene.
    unsigned int GetRenderQueueSize(void);
    
    /// Return an entity from an index location.
    Entity* GetEntity(unsigned int index);
    
    
private:
    
    /// List of entities in this scene.
    std::vector<Entity*>  entities;
    
};

#endif
