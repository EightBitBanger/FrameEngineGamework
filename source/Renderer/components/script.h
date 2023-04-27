#ifndef COMPONENT_SCRIPT
#define COMPONENT_SCRIPT

void DefaultFunctionPtr(void);
#define  nullfunc  DefaultFunctionPtr

struct Script {
    
    /// State whether OnCreate() has already been called.
    bool hasBeenInitiated;
    
    /// This function will be called on object creation.
    void(*OnCreate)();
    /// This function will be called once per frame.
    void(*OnUpdate)();
    
    Script();
    
};


#endif
