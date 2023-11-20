#include "actor.h"

extern PhysicsSystem        Physics;


Actor::Actor() : 
    name(""),
    isActive(true),
    age(0),
    
    chanceToMove(0),
    chanceToLookAtPlayer(0),
    chanceToLookAtEntity(0),
    chanceToStopMoving(0),
    
    distanceToMove(0),
    distanceToLookAtPlayer(0),
    distanceToLookAtEntity(0),
    distanceToStopMoving(0)
    
    
{
}

void Actor::Update(void) {
    
    
    
    
}
