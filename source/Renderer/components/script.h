
void DefaultFunctionPtr(void) {return;}

struct Script {
    
    void(*OnCreate)();
    void(*OnUpdate)();
    
    Script() {
        OnCreate = DefaultFunctionPtr;
        OnUpdate = DefaultFunctionPtr;
    }
    
    
};


