#ifndef RESOURCE_COLLIDER_TAG
#define RESOURCE_COLLIDER_TAG

#include "../../Physics/PhysicsSystem.h"

struct __declspec(dllexport) ColliderTag {
    
    /// Collider resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Collider state is static or reacts as a dynamic object.
    bool isStatic;
    
    /// Physical box collider shape.
    rp3d::BoxShape* colliderShape;
    
    ColliderTag();
    
};

#endif
