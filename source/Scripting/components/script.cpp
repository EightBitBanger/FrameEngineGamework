#include "script.h"

Script::Script() {
    
    isActive         = false;
    hasBeenInitiated = false;
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

// Dummy landing function
void DefaultFunctionPtr(void) {
    return;
}

