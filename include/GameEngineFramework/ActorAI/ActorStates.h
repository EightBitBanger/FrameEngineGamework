#ifndef ACTOR_STATES
#define ACTOR_STATES
#include <type_traits>

template <typename E> constexpr typename std::underlying_type<E>::type to_underlying(E e) {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

namespace ActorState {

enum class Mode {
    Idle,
    Sleeping,
    
    MovePlanting,
    MoveHarvesting,
    
    MoveHunting,
    MoveBreed,
    MoveSocialize,
    MoveRandom,
    
    MoveAttack,
    MoveFlee,
    
    MoveTo,
    WalkTo,
    RunTo,
    
    Frozen    // Hold state for editing / debugging
};

enum class Genetic : unsigned int {
    Base,
    Male,
    Female
};

enum class Animation : unsigned int {
    Body,         // Static, no animation
    Head,         // Animate as part of the head
    Limb,         // Animate as limb
    LimbHolding   // Animate as holding an item
};

}

#endif
