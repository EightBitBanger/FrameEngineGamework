#ifndef COMPONENT_SCENE
#define COMPONENT_SCENE

#include <vector>

#include "entity.h"

class Scene {
    
public:
    
    /// Return an entity from an index location.
    Entity* GetEntity(unsigned int index);
    
    /// Add an entity to this scene.
    void AddToSceneRoot(Entity* entity);
    /// Remove an entity from this scene.
    bool RemoveFromSceneRoot(Entity* entity);
    
    /// Return the number of entities in this scene.
    unsigned int GetRenderQueueSize(void);
    
    /// List of entities in this scene.
    std::vector<Entity*>  entities;
    
};

#endif
