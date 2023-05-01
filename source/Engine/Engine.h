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
    
    
    void Update(void);
    
private:
    
    PoolAllocator<GameObject> gameObject;
    
};









#endif
