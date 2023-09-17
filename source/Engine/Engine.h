#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "components/gameobject.h"

#include "../application/applicationlayer.h"
#include "../../tests/framework.h"

#include "../Math/Math.h"
#include "../Renderer/components/entity.h"
#include "../Scripting/components/script.h"
#include "../Renderer/components/camera.h"

#include "../../vendor/CodeBaseLibrary/timer.h"
#include "../../vendor/CodeBaseLibrary/logging.h"
#include "../../vendor/CodeBaseLibrary/random.h"



class EngineSystemManager {
    
public:
    
    // Game objects
    
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    /// Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    
    // Components
    
    /// Create a component object with initial type information and return its pointer.
    Component* CreateComponent(ComponentType type);
    /// Destroy a component object.
    bool DestroyComponent(Component* componentPtr);
    
    /// Get the number of component objects.
    unsigned int GetComponentCount(void);
    
    
    // Specially built components and game objects
    
    /// Create a camera controller game object and return its pointer.
    GameObject* CreateCameraController(glm::vec3 position=glm::vec3(0));
    
    /// Create a mesh renderer component and return its pointer.
    Component* CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr);
    
    /// Create a point light component and return its pointer.
    Component* CreateComponentLight(glm::vec3 position=glm::vec3(0));
    
    
    EngineSystemManager();
    
    /// Initiate the engine.
    void Initiate(void);
    
    /// Call update on engine components.
    void Update(void);
    
    /// Shutdown the engine.
    void Shutdown(void);
    
private:
    
    Scene* mSceneMain;
    
    // List of active game objects
    std::vector<GameObject*> mGameObjectActive;
    
    // Engine allocators
    PoolAllocator<GameObject> mGameObjects;
    PoolAllocator<Component>  mComponents;
    
};



#endif
