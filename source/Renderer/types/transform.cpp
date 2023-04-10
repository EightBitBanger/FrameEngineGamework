#include "transform.h"

Transform::Transform() {
    
    position = glm::vec3(0, 0, 0);
    rotation = glm::vec3(0, 0, 0);
    scale    = glm::vec3(1, 1, 1);
    
    parent = nullptr;
    
}


void Transform::ChildAdd(Transform* transform) {
    child.push_back(transform);
}

bool Transform::ChildRemove(Transform* transform) {
    for (std::vector<Transform*>::iterator it = child.begin(); it != child.end(); ++it) {
        Transform* transformPtr = *it;
        if (transform == transformPtr) {
            child.erase(it);
            return true;
        }
    }
    return false;    
}


Transform Transform::Identity(void) {
    Transform identity;
    return identity;
}

