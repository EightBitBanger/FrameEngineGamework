#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

#include <array>
#include <string>
#include <cstdint>

#include <GameEngineFramework/UI/UserInterfaceSystem.h>

class ENGINE_API ItemClass {
public:
    unsigned int stackMax;   // Maximum number of items per stack
};


class ENGINE_API InventorySystem {
public:
    InventorySystem();
    ~InventorySystem();
    
    void Initiate(unsigned int numberOfSlots, int windowW, int windowH);
    void Update();
    
    // Items
    bool AddItem(std::string name);
    bool RemoveItem(unsigned int slot);
    std::string QueryItem(unsigned int slot);
    
    // Selector
    void SetSelectorIndex(unsigned int index);
    unsigned int GetSelectorIndex();
    bool CheckSlotEmpty(unsigned int slot);
    
    void NextSlot();
    void PrevSlot();
    
    // Item class
    bool AddItemClass(std::string name, ItemClass itemClass);
    bool CheckItemClassExists(std::string name);
    
    // Callback
    void OnWindowResize(int windowW, int windowH);
    
    // Save inventory state the world directory
    bool SaveToFile(const std::string& worldDirectory) const;
    // Load inventory state from the world directory
    bool LoadFromFile(const std::string& worldDirectory);
    
    std::string version;
    
private:
    Panel* mBar;
    Panel* mSelectorPanel;
    
    // Items in the inventory
    std::vector<unsigned int> mCount;
    std::vector<std::string> mItems;
    
    // UI
    std::vector<Panel*> mSlotPanels;
    std::vector<Text*> mSlotCounters;
    std::vector<Sprite*> mSlotSprites;
    std::vector<Sprite*> mHealthSprites;
    std::vector<Sprite*> mHungerSprites;
    
    // Item classes
    std::unordered_map<std::string, ItemClass> mItemClass;
    
    // Selector
    unsigned int mSelectorIndex;
    
    int mWindowW;
    int mWindowH;
    
public:
    
    int mHotbarX;
    int mHotbarY;
    
    int mSpacingGapPx;
    int mSlotSizePx;
    int mSpriteSizePx;
    
    int mTextX;
    int mTextY;
};

#endif
