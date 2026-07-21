#ifndef _PHYSICS_MASKS_
#define _PHYSICS_MASKS_

#include <reactphysics3d/reactphysics3d.h>

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
    Entity,
    Static
};

#endif
