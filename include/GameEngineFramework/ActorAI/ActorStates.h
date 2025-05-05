#ifndef ACTOR_STATES
#define ACTOR_STATES

namespace ActorState {

enum class Mode {
    Idle,
    MoveTo,
    WalkTo,
    RunTo
};

enum class State {
    None,
    Look,
    Focus,
    Attack,
    Flee,
    Defend
};

}

#endif
