#ifndef _PHYSICS_MASKS_
#define _PHYSICS_MASKS_

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"

#include <cstdlib>

// Collision type layer mask
enum class LayerMask {
    Default = 0x0001,
    Ground  = 0x0002,
    Object  = 0x0003,
    Actor   = 0x0004,
    Player  = 0x0005
};


// Solid collision mask
enum class CollisionMask {
    Default = 0x0001,
    Ground  = 0x0002,
    Entity  = 0x0003
};

#endif
