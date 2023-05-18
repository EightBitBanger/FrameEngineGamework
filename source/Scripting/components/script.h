#ifndef __COMPONENT_SCRIPT
#define __COMPONENT_SCRIPT

#include "../../Engine/components/gameobject.h"

void DefaultFunctionPtr(GameObject* gameObject);

#define  nullfunc  DefaultFunctionPtr

struct Script {
    
    /// State whether the script is active and should be updated.
    bool isActive;
    
    /// State whether OnCreate() has already been called.
    bool hasBeenInitiated;
    
    /// Pointer to the game object who owns this script.
    GameObject* gameObject;
    
    /// This function will be called on object creation.
    void(*OnCreate)(GameObject* gameObjectPtr);
    
    /// This function will be called once per frame.
    void(*OnUpdate)(GameObject* gameObjectPtr);
    
    
    Script();
    
};


#endif
