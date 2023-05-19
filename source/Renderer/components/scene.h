#ifndef __COMPONENT_SCENE
#define __COMPONENT_SCENE

#include <vector>

#include "entity.h"
#include "light.h"


class Scene {
    
public:
    
    /// Add an entity to this scene.
    void AddEntityToSceneRoot(Entity* entity);
    /// Remove an entity from this scene.
    bool RemoveEntityFromSceneRoot(Entity* entity);
    
    /// Add a light to this scene.
    void AddLightToSceneRoot(Light* light);
    /// Remove an entity from this scene.
    bool RemoveLightFromSceneRoot(Light* light);
    
    /// Return the number of entities in this scene.
    unsigned int GetEntityQueueSize(void);
    
    /// Return the number of entities in this scene.
    unsigned int GetLightQueueSize(void);
    
    /// Return an entity from an index location in the entity list.
    Entity* GetEntity(unsigned int index);
    
    /// Return a light from an index location in the light list.
    Light* GetLight(unsigned int index);
    
private:
    
    /// List of entities in this scene.
    std::vector<Entity*>  mEntityList;
    
    /// List of lights in this scene.
    std::vector<Light*>  mLightList;
    
};

#endif
