#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "components/gameobject.h"

#include "../application/applicationlayer.h"


class EngineSystemManager {
    
public:
    
    EngineSystemManager();
    
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    /// Destroy a game object.
    void DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Create an entity renderer component and return its pointer.
    Entity* CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr);
    /// Destroy an entity renderer component.
    void DestroyEntityRenderer(Entity* entityPtr);
    
    /// Create a rigid body physics component and return its pointer.
    rp3d::RigidBody* CreateRigidBody(float x=0, float y=0, float z=0);
    /// Destroy a rigid body physics component.
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    void Initiate();
    
    void Update(void);
    
private:
    
    void AddGameObjectToActiveList(GameObject* gameObjectPtr);
    bool RemoveGameObjectFromActiveList(GameObject* gameObjectPtr);
    
    Scene* sceneMain;
    
    std::vector<GameObject*> gameObjectActive;
    
    PoolAllocator<GameObject> gameObject;
    
};









#endif
