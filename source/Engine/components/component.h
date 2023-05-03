#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#define COMPONENT_TYPE_UNDEFINED      0
#define COMPONENT_TYPE_RENDERER       100
#define COMPONENT_TYPE_RIGIDBODY      101
#define COMPONENT_TYPE_SCRIPT         102
#define COMPONENT_TYPE_CAMERA         103

#include <string>

class Component {
    
public:
    
    /// Component name.
    std::string name;
    
    /// Return the object pointer.
    void* GetComponent(void);
    
    /// Return the component type.
    unsigned int GetType(void);
    
    /// Set the component type and reference object.
    void SetComponent(unsigned int type, void* object);
    
    Component();
    
private:
    
    // Type identifier.
    unsigned int type;
    
    // Object pointer
    void* object;
    
};




#endif
