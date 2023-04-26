#include "script.h"

Script::Script() {
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

// Dummy landing function
void DefaultFunctionPtr(void) {
    return;
}

