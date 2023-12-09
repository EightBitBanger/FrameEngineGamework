#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#include <GameEngineFramework/configuration.h>

#include <string>


#define  COMPONENT_TYPE_UNDEFINED       0
// Engine
#define  COMPONENT_TYPE_TRANSFORM       1
//Rendering
#define  COMPONENT_TYPE_MESH_RENDERER   2
#define  COMPONENT_TYPE_CAMERA          3
#define  COMPONENT_TYPE_LIGHT           4
#define  COMPONENT_TYPE_SCRIPT          5
// Physics
#define  COMPONENT_TYPE_RIGID_BODY      6
// AI
#define  COMPONENT_TYPE_ACTOR           7
// UI
#define  COMPONENT_TYPE_TEXT            8
#define  COMPONENT_TYPE_PANEL           9



typedef unsigned short int ComponentType;


struct ENGINE_API EngineComponents {
    /// Undefined component.
    constexpr static short int Undefined = COMPONENT_TYPE_UNDEFINED;
    
    /// Transform component.
    constexpr static short int Transform = COMPONENT_TYPE_TRANSFORM;
    
    /// Mesh renderer component.
    constexpr static short int MeshRenderer = COMPONENT_TYPE_MESH_RENDERER;
    
    /// Camera component.
    constexpr static short int Camera = COMPONENT_TYPE_CAMERA;
    
    /// Light component.
    constexpr static short int Light = COMPONENT_TYPE_LIGHT;
    
    /// Scripting component.
    constexpr static short int Script = COMPONENT_TYPE_SCRIPT;
    
    /// Physics rigid body component.
    constexpr static short int RigidBody = COMPONENT_TYPE_RIGID_BODY;
    
    /// AI actor component.
    constexpr static short int Actor = COMPONENT_TYPE_ACTOR;
    
    /// Text UI component.
    constexpr static short int Text = COMPONENT_TYPE_TEXT;
    
    /// Panel UI component.
    constexpr static short int Panel = COMPONENT_TYPE_PANEL;
};




class ENGINE_API Component {
    
public:
    
    friend class EngineSystemManager;
    
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
