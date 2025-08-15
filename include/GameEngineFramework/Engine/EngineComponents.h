#ifndef ENGINE_COMPONENT_TYPES
#define ENGINE_COMPONENT_TYPES

#include <GameEngineFramework/configuration.h>

#include <type_traits>
#include <cstddef>

typedef unsigned short int ComponentType;

struct ENGINE_API EngineComponents {
    constexpr static short Undefined     = -1;
    constexpr static short Transform     = 0;
    constexpr static short MeshRenderer  = 1;
    constexpr static short Camera        = 2;
    constexpr static short Light         = 3;
    constexpr static short Script        = 4;
    constexpr static short RigidBody     = 5;
    constexpr static short Actor         = 6;
    constexpr static short Sound         = 7;
    
    // Total number of components tracker
    constexpr static short NumberOfComponents = Sound + 1;
};

template<class T, class = void>
struct component_index {
    static_assert(!std::is_same<T, T>::value,
                  "GetComponent<T>: T is not mapped to an EngineComponents ID");
    static constexpr short value = EngineComponents::Undefined;
};

#define MAP_COMPONENT(TYPE, ENUM_NAME) template<> struct component_index<TYPE, void> {static constexpr short value = EngineComponents::ENUM_NAME;}

#endif
