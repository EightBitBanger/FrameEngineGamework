#ifndef COMPONENT_SCRIPT
#define COMPONENT_SCRIPT

void DefaultFunctionPtr(void);

struct Script {
    
    void(*OnCreate)();
    void(*OnUpdate)();
    
    Script();
    
};


#endif
