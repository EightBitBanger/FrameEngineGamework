#ifndef COMPONENT_SCRIPT
#define COMPONENT_SCRIPT

void DefaultFunctionPtr(void);
#define  nullfunc  DefaultFunctionPtr

class Script {
    
public:
    
    void(*OnCreate)();
    void(*OnUpdate)();
    
    Script();
    
};


#endif
