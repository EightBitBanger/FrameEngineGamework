#ifndef AI_ACTOR_INVENTORY
#define AI_ACTOR_INVENTORY

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API InventoryManager {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Name of the in hand item.
    std::string inHandItemName;
    
    /// Added damage multiplier for the item/weapon in hand.
    float damageMul;
    
    /// Added defense multiplier for the item/weapon in hand.
    float defenseMul;
    
    /// List of item classes in the inventory.
    std::vector<std::string> itemClassList;
    
    /// Transform of the item currently in hand for animation.
    glm::vec3 handPosition;
    glm::vec3 handOffset;
    glm::vec3 handRotation;
    glm::vec3 handScale;
    
    /// Renderer for the item in hand.
    MeshRenderer* holdingRenderer;
    
    /// Give the actor an item to hold in hand.
    void GiveItem(const std::string& itemClassification);
    
    InventoryManager();
};

#endif
