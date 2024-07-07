#include <GameEngineFramework/Engine/components/component.h>

extern EngineComponents Components;


Component::Component() : 
    name(""),
    mType(Components.Undefined),
    mObject(nullptr)
{
}

void* Component::GetComponent(void) {
    return mObject;
}

ComponentType Component::GetType(void) {
    return mType;
}

void Component::SetComponent(ComponentType component_type, void* component_object) {
    mType = component_type;
    mObject = component_object;
    return;
}
