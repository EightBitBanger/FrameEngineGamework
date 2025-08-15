#ifndef ENGINE_COMPONENT
#define ENGINE_COMPONENT

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/Engine/EngineComponents.h>

#include <string>



class ENGINE_API Component {
    
public:
    
    friend class EngineSystemManager;
    
    /// Component name.
    std::string name;
    
    /// Return the object pointer.
    void* GetComponent(void);
    
    /// Return the component type.
    ComponentType GetType(void);
    
    Component();
    
    
private:
    
    // Set the component type and reference object
    void SetComponent(ComponentType type, void* object);
    
    // Type identifier declaring the component pointer type
    ComponentType mType;
    
    // Object pointer
    void* mObject;
    
};


#endif
