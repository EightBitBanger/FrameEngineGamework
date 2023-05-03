#include "component.h"

void* Component::GetComponent(void) {
    return object;
}

unsigned int Component::GetType(void) {
    return type;
}

void Component::SetComponent(unsigned int component_type, void* component_object) {
    type = component_type;
    object = component_object;
    return;
}

Component::Component() {
    type = COMPONENT_TYPE_UNDEFINED;
    object = nullptr;
    return;
}

