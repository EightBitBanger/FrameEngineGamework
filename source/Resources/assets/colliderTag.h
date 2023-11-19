#ifndef RESOURCE_COLLIDER_TAG
#define RESOURCE_COLLIDER_TAG

#include "../../Physics/PhysicsSystem.h"

struct __declspec(dllexport) ColliderTag {
    
    /// Collider resource name.
    std::string name;
    
    /// Collider state is static or reacts dynamically.
    bool  isStatic;
    
    /// Physical box collider shape.
    rp3d::BoxShape* colliderShape;
    
    ColliderTag();
    
};

#endif
