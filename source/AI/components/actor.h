#ifndef _AI_ACTOR__
#define _AI_ACTOR__

#include <vector>
#include <string>
#include <iostream>


class Actor {
    
public:
    
    std::string name;
    
    bool isActive;
    
    unsigned long int age;
    
    // AI personality parameters
    
    int chanceToMove;
    int chanceToLookAtPlayer;
    int chanceToLookAtEntity;
    int chanceToStopMoving;
    
    float distanceToMove;
    float distanceToLookAtPlayer;
    float distanceToLookAtEntity;
    float distanceToStopMoving;
    
    // Actor genetics
    
    
    
    
    
    
    
    Actor();
    
    void Update(void);
    
    
    
    
private:
    
    
};





#endif // _AI_ACTOR__
