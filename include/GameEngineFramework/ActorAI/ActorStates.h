#ifndef ACTOR_STATES
#define ACTOR_STATES

namespace ActorState {

enum class Mode {
    Idle,         // Check for an available task
    Sleeping,     // Do nothing
    MoveRandom,   // Move randomly
    MoveTo,       // Move to a point, not effecting walk|run state
    WalkTo,       // Walk to a point
    RunTo         // Run to a point
};

enum class State {
    None,         // No state
    Look,         // Look at an actor or point
    Focus,        // Point towards an actor or point
    Attack,       // Attack the target actor
    Flee,         // Flee from the target actor
    Defend        // Defend from the target actor
};

enum class Action {
    Purge         // Remove the actor ref from all lists
};

}

#endif
