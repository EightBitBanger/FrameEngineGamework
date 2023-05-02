#include "script.h"

Script::Script() {
    
    isActive         = false;
    hasBeenInitiated = false;
    
    gameObject = nullptr;
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

// Dummy landing function
void DefaultFunctionPtr(void* gameObjectPtr) {
    return;
}

