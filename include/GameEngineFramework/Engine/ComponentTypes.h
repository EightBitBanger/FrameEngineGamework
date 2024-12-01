#ifndef ENGINE_COMPONENT_TYPES
#define ENGINE_COMPONENT_TYPES

#include <GameEngineFramework/configuration.h>

typedef unsigned short int ComponentType;


struct ENGINE_API EngineComponents {
    /// Undefined component.
    constexpr static short int Undefined = 0;
    
    /// Transform component.
    constexpr static short int Transform = 1;
    
    /// Mesh renderer component.
    constexpr static short int MeshRenderer = 2;
    
    /// Camera component.
    constexpr static short int Camera = 3;
    
    /// Light component.
    constexpr static short int Light = 4;
    
    /// Scripting component.
    constexpr static short int Script = 5;
    
    /// Physics rigid body component.
    constexpr static short int RigidBody = 6;
    
    /// AI actor component.
    constexpr static short int Actor = 7;
    
    /// Text UI component.
    constexpr static short int Text = 8;
    
    /// Panel UI component.
    constexpr static short int Panel = 9;
};



#endif
