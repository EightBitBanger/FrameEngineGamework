#ifndef ACTOR_STATES
#define ACTOR_STATES

namespace ActorState {

enum class Mode {
    Idle,           // Do nothing but conscious
    Sleeping,       // Do nothing but effectively unconscious
    
    MoveAttack,     // Move to attack the target
    MoveFlee,       // Move to flee from the target
    
    MoveRandom,     // Initiate a move to a random point
    MoveToRandom,   // Move toward a random point
    
    MoveTo,         // Move to a point
    WalkTo,         // Walk to a point
    RunTo           // Run to a point
};

/*
enum class State {
    None,         // No state
    Look,         // Look at an actor or point
    Observe,      // Focus on an actor or point
    Attack,       // Attack the target actor
    Flee,         // Flee from the target actor
    Defend,       // Defend from the target actor
    Breed         // Reproduce with an actor of the opposite sex
};
*/

enum class Animation {
    Body,         // Static, no animation
    Head,         // Animate as part of the head
    Limb          // Animate as a limb
};

enum class Action {
    Purge         // Remove the actor ref from all lists
};

}

#endif
