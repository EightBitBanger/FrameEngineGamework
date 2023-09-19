#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#include <string>

#define  COMPONENT_TYPE_UNDEFINED       0
#define  COMPONENT_TYPE_MESH_RENDERER   1
#define  COMPONENT_TYPE_CAMERA          2
#define  COMPONENT_TYPE_LIGHT           3
#define  COMPONENT_TYPE_SCRIPT          4
#define  COMPONENT_TYPE_RIGID_BODY      5


typedef unsigned short int ComponentType;


struct EngineComponents {
    /// Undefined component type.
    short int Undefined = COMPONENT_TYPE_UNDEFINED;
    /// Mesh renderer component type.
    short int MeshRenderer = COMPONENT_TYPE_MESH_RENDERER;
    /// Camera component type.
    short int Camera = COMPONENT_TYPE_CAMERA;
    /// Light component type.
    short int Light = COMPONENT_TYPE_LIGHT;
    /// Script component type.
    short int Script = COMPONENT_TYPE_SCRIPT;
    /// RigidBody component type.
    short int RigidBody = COMPONENT_TYPE_RIGID_BODY;
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
