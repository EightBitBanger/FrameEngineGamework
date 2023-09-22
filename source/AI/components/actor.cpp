#include "actor.h"


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
    
    std::cout << "age " << age << std::endl;
    
}
