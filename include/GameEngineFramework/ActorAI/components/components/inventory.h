#ifndef AI_ACTOR_INVENTORY
#define AI_ACTOR_INVENTORY

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API InventoryManager {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Added damage multiplier for the item/weapon in hand.
    float damageMul;
    
    /// Added defense multiplier for the item/weapon in hand.
    float defenseMul;
    
    /// Descriptor string of the currently equipped item.
    std::string inHandItemClass;
    
    /// List of item classes stored in the inventory.
    std::vector<std::string> itemClassList;
    
    /// Transform of the item currently in hand for animation.
    glm::vec3 handPosition;
    glm::vec3 handOffset;
    glm::vec3 handRotation;
    glm::vec3 handScale;
    
    /// Renderer for the item in hand.
    MeshRenderer* holdingRenderer;
    
    /// Adds an item descriptor to the inventory.
    bool AddItem(const std::string& itemClassification);
    
    /// Adds an item descriptor to the inventory and equips the item in hand.
    bool GiveItem(const std::string& itemClassification);
    
    /// Removes an item from the inventory by index.
    bool RemoveItem(unsigned int index);
    
    /// Builds and equips an item from the inventory by its index.
    bool EquipItem(unsigned int index);
    
    /// Builds and equips the first matching item descriptor found in the inventory.
    bool EquipItem(const std::string& itemClassification);
    
    /// Unequips the current item, hides the hand renderer, and resets multipliers.
    void UnequipItem();
    
    /// Equip the item with the highest damage potential.
    bool EquipWeapon();
    
    InventoryManager();
    
    void Reset();
    
private:
    
    bool doEquipWeapon;
    bool doBuildHandMesh;
    
    /// Internal method to construct 3D mesh geometry for an equipped item.
    void BuildHandMesh(const std::string& itemClassification);
};

#endif
