#ifndef COMPONENT_SCRIPT
#define COMPONENT_SCRIPT

void DefaultFunctionPtr(void);
#define  nullfunc  DefaultFunctionPtr

struct Script {
    
    void(*OnCreate)();
    void(*OnUpdate)();
    
    Script();
    
};


#endif
