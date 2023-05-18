#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

enum struct ComponentType {
    Undefined=0,
    // Render system
    Renderer, Camera, Light,
    // scripting
    Script,
    // Physics
    RigidBody
};


#include <string>

class Component {
    
public:
    
    /// Component name.
    std::string name;
    
    /// Return the object pointer.
    void* GetComponent(void);
    
    /// Return the component type.
    ComponentType GetType(void);
    
    /// Set the component type and reference object.
    void SetComponent(ComponentType type, void* object);
    
    Component();
    
    
private:
    
    // Type identifier declaring the component pointer type.
    ComponentType mType;
    
    // Object pointer
    void* mObject;
    
};




#endif
