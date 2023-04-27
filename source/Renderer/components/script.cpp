#include "script.h"

Script::Script() {
    
    hasBeenInitiated = false;
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

// Dummy landing function
void DefaultFunctionPtr(void) {
    return;
}

