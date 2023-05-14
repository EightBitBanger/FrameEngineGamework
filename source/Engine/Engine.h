#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "components/gameobject.h"

#include "../application/applicationlayer.h"

#include "../Renderer/components/entity.h"
#include "../Scripting/components/script.h"
#include "../Renderer/components/camera.h"

#include "../my-std-lib/timer.h"
#include "../my-std-lib/logging.h"
#include "../my-std-lib/random.h"

class EngineSystemManager {
    
public:
    
    EngineSystemManager();
    
    // Game objects
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    /// Destroy a game object.
    void DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    // Components
    /// Create a component object with initial type information and return its pointer.
    Component* CreateComponent(unsigned int component_type);
    /// Destroy a component object.
    void DestroyComponent(Component* componentPtr);
    
    // Specially built components and game objects
    /// Create a camera controller game object and return its pointer.
    GameObject* CreateCameraController(float x=0, float y=0, float z=0);
    
    /// Create an entity renderer component and return its pointer.
    Component* CreateComponentEntityRenderer(Mesh* meshPtr, Material* materialPtr);
    
    
    /// Initiate the engine.
    void Initiate(void);
    
    /// Call update on engine components.
    void Update(void);
    
    /// Shutdown the engine.
    void Shutdown(void);
    
private:
    
    // Create an entity renderer and return its pointer.
    Entity* CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr);
    // Destroy an entity renderer.
    void DestroyEntityRenderer(Entity* entityPtr);
    
    
    // Add a game object to the internal list of active objects
    void AddGameObjectToActiveList(GameObject* gameObjectPtr);
    // Remove a game object from the internal list of active objects
    bool RemoveGameObjectFromActiveList(GameObject* gameObjectPtr);
    
    
    Scene* mSceneMain;
    
    // List of active game objects
    std::vector<GameObject*> mGameObjectActive;
    
    // Engine allocators
    PoolAllocator<GameObject> mGameObject;
    PoolAllocator<Component>  mComponents;
    
};



#endif
