#ifndef _PHYSICS_MASKS_
#define _PHYSICS_MASKS_

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"

#include <cstdlib>

// Collision type layer mask
enum class LayerMask {
    Default,
    Static,
    Ground,
    Object,
    Actor,
    Player
};


// Solid collision mask
enum class CollisionMask {
    Default,
    Ground,
    Entity
};

#endif
