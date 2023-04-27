#ifndef RESOURCE_COLLIDER_TAG
#define RESOURCE_COLLIDER_TAG

#include "../../Physics/PhysicsSystem.h"

struct ColliderTag {
    
    std::string name;
    
    bool  isStatic;
    
    rp3d::BoxShape* colliderShape;
    
    ColliderTag();
    
};

#endif
