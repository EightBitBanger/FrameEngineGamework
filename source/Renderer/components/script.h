#ifndef COMPONENT_SCRIPT
#define COMPONENT_SCRIPT

void DefaultFunctionPtr(void);
#define  nullfunc  DefaultFunctionPtr

struct Script {
    
    bool hasBeenInitiated;
    
    void(*OnCreate)();
    void(*OnUpdate)();
    
    Script();
    
};


#endif
