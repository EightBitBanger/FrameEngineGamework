#include "component.h"

void* Component::GetComponent(void) {
    return object;
}

componentENUM Component::GetType(void) {
    return type;
}

void Component::SetComponent(componentENUM component_type, void* component_object) {
    type = component_type;
    object = component_object;
    return;
}

Component::Component() {
    type = COMPONENT_TYPE_UNDEFINED;
    object = nullptr;
    return;
}

