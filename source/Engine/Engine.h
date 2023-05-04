#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "components/gameobject.h"

#include "../application/applicationlayer.h"

#include "../Renderer/components/entity.h"
#include "../Scripting/components/script.h"
#include "../Renderer/components/camera.h"


#include "../std/timer.h"
#include "../std/logging.h"
#include "../std/random.h"

class EngineSystemManager {
    
public:
    
    EngineSystemManager();
    
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    /// Destroy a game object.
    void DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Create a component object with initial type information and return its pointer.
    Component* CreateComponent(unsigned int component_type);
    /// Destroy a component object.
    void DestroyComponent(Component* componentPtr);
    
    
    /// Create a camera controller game object and return its pointer.
    GameObject* CreateCameraController(float x=0, float y=0, float z=0);
    
    
    /// Create an entity renderer component and return its pointer.
    Entity* CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr);
    /// Destroy an entity renderer component.
    void DestroyEntityRenderer(Entity* entityPtr);
    
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    
    void Initiate();
    
    void Update(void);
    
private:
    
    // Add an entity to the internal list of active objects
    void AddGameObjectToActiveList(GameObject* gameObjectPtr);
    // Remove an entity from the internal list of active objects
    bool RemoveGameObjectFromActiveList(GameObject* gameObjectPtr);
    
    // Rigid body free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    
    Scene* sceneMain;
    
    // List of active game objects
    std::vector<GameObject*> gameObjectActive;
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> rigidBodyFreeList;
    
    PoolAllocator<GameObject> gameObject;
    PoolAllocator<Component>  components;
    
};



#endif
