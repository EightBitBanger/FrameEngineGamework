#include "script.h"

Script::Script() {
    
    isActive = true;
    hasBeenInitiated = false;
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

// Dummy landing function
void DefaultFunctionPtr(void) {
    return;
}

