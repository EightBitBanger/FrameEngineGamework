#include <GameEngineFramework/Engine/components/component.h>

Component::Component() : 
    name(""),
    mType(0),
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
}
