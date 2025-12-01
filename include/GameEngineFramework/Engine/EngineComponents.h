#ifndef ENGINE_COMPONENT_TYPES
#define ENGINE_COMPONENT_TYPES

#include <GameEngineFramework/configuration.h>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <vector>
#include <mutex>

#include <type_traits>
#include <cstddef>

typedef unsigned short int ComponentType;

enum class ComponentUpdateType {
    NoUpdate,
    PerFrame,
    PerObject
};

struct ENGINE_API EngineComponent {
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


class ENGINE_API ComponentTypeRegistry {
public:
    
    /// Get the ID of a given type.
    template<typename T> int GetID() const {
        const std::type_index key(typeid(T));
        std::lock_guard<std::mutex> lock(mMux);
        std::unordered_map<std::type_index, int>::const_iterator it = mTypeRegistry.find(key);
        if (it == mTypeRegistry.end()) return -1;
        return it->second;
    }
    
    /// Registry a new type by its associated ID.
    template<typename T> bool RegisterComponentType(int fixed_id) {
        if (fixed_id < 0) return false;
        const std::type_index key(typeid(T));
        std::lock_guard<std::mutex> lock(mMux);
        
        std::unordered_map<std::type_index, int>::const_iterator it = mTypeRegistry.find(key);
        if (it != mTypeRegistry.end()) return it->second == fixed_id;
        
        // Check collision
        if (fixed_id < (int)mIDRegistry.size() && mIDRegistry[fixed_id] != std::type_index(typeid(void))) 
            return false;
        
        // Grow reverse map if needed
        if (fixed_id >= (int)mIDRegistry.size()) 
            mIDRegistry.resize(fixed_id + 1, std::type_index(typeid(void)));
        
        mIDRegistry[fixed_id] = key;
        mTypeRegistry.emplace(key, fixed_id);
        
        mNumberOfComponents++;
        return true;
    }
    
    unsigned int GetNumberOfComponents();
    
private:
    
    unsigned int mNumberOfComponents;
    
    mutable std::mutex mMux;
    
    std::unordered_map<std::type_index, int> mTypeRegistry;
    std::vector<std::type_index> mIDRegistry{};
};

#endif
