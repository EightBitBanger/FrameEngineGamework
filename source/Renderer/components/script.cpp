#include "script.h"

Script::Script() {
    
    OnCreate = nullfunc;
    OnUpdate = nullfunc;
}

void DefaultFunctionPtr(void) {return;}

