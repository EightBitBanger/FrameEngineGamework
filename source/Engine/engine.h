#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "../application/applicationlayer.h"


class EngineSystemManager {
    
public:
    
    EngineSystemManager();
    
    /// Create a game object and return its pointer.
    Entity* CreateGameObject(void);
    
    /// Destroy a game object.
    void DestroyGameObject(Entity* entityPtr);
    
    
private:
    
    
    
};









#endif
