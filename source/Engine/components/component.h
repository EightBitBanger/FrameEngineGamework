#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#include <string>


enum struct ComponentType {
    Undefined=0,
    // Render system
    MeshRenderer, Camera, Light,
    // scripting
    Script,
    // Physics
    RigidBody
};


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
