#include "component.h"

void* Component::GetComponent(void) {
    return mObject;
}

componentENUM Component::GetType(void) {
    return mType;
}

void Component::SetComponent(componentENUM component_type, void* component_object) {
    mType = component_type;
    mObject = component_object;
    return;
}

Component::Component() {
    mType = COMPONENT_TYPE_UNDEFINED;
    mObject = nullptr;
    return;
}

