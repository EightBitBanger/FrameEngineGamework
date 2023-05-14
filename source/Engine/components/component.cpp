#include "component.h"

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

Component::Component() {
    mType = ComponentType::Undefined;
    mObject = nullptr;
    return;
}

