#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "../application/applicationlayer.h"

extern Timer        PhysicsTime;
extern Timer        Time;
extern Logger       Log;
extern RandomGen    Random;
extern ColorPreset  Colors;

extern InputSystem       Input;
extern PhysicsSystem     Physics;
extern RenderSystem      Renderer;
extern ResourceManager   Resources;
extern ApplicationLayer  Application;



class EngineSystemManager {
    
public:
    
    EngineSystemManager();
    
    /// Create and return an assembled entity object.
    Entity* CreateGameObject(void);
    
    
    
private:
    
    
    
};









#endif
