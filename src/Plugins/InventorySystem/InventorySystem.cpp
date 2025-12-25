#include <GameEngineFramework/Engine/EngineSystems.h>

#include "GameEngineFramework/Plugins/InventorySystem/InventorySystem.h"
#include <algorithm>
#include <fstream>

InventorySystem::InventorySystem() : 
    version("0.0.0"),
    mBar(nullptr),
    mSelectorIndex(0),
    mWindowW(0),
    mWindowH(0),
    mHotbarX(0),
    mHotbarY(120),
    mSpacingGapPx(3),
    mSlotSizePx(40),
    mSpriteSizePx(38),
    mTextX(10),
    mTextY(15)
{}

InventorySystem::~InventorySystem() {}

void InventorySystem::Initiate(unsigned int numberOfSlots, int windowW, int windowH) {
    std::string versionsDirectory = "versions/";
    std::string classesDirectory = "/classes/";
    std::string bitmapDirectory = "/bitmap/";
    std::string classesPath = versionsDirectory + version + classesDirectory;
    std::string bitmapPath = versionsDirectory + version + bitmapDirectory;
    
    // Load the items
    std::vector<std::string> listOfItems = fs.DirectoryGetList(classesPath);
    for (unsigned int i=0; i < listOfItems.size(); i++) {
        
        // Load inventory details
        if (listOfItems[i].find(".class") != std::string::npos) {
            // Add new item class
            FileLoader loader(classesPath + listOfItems[i]);
            if (!loader.CheckIsFileLoaded()) 
                Engine.console.Print("Failed to load " + listOfItems[i]);
            
            int stackMax = 1;
            if (loader.assetData.find("stackMax") != loader.assetData.end()) 
                stackMax = String.ToInt(loader.assetData["stackMax"][0]);
            
            if (loader.assetData.find("bitmap") != loader.assetData.end()) {
                std::string resourceName = loader.assetData["bitmap"][0];
                if (!Resources.LoadTexture(bitmapPath + resourceName + ".png", resourceName, true)) {
                    Engine.console.Print("Failed to load " + resourceName);
                    continue;
                }
                
                TextureTag* matTag = Resources.FindTextureTag(resourceName);
                if (matTag == nullptr) {
                    Engine.console.Print("Failed to load " + resourceName);
                    continue;
                }
                matTag->Load();
                
                ItemClass item;
                item.stackMax = stackMax;
                
                AddItemClass(resourceName, item);
            }
            
        }
        
    }
    
    // Initiate the inventory layout
    mWindowW = windowW;
    mWindowH = windowH;
    
    const int totalWidth = numberOfSlots * mSlotSizePx + (numberOfSlots - 1) * mSpacingGapPx;
    const int xx = mHotbarX + (mWindowW - totalWidth) / 2;
    const int yy = mWindowH - mHotbarY;
    
    mSelectorPanel = UI.CreatePanel();
    mSelectorPanel->name = "selector";
    mSelectorPanel->alpha = 0.5f;
    mSelectorPanel->isActive = true;
    
    mSelectorPanel->x = xx + (mSelectorIndex * (mSlotSizePx + mSpacingGapPx)) - 2;
    mSelectorPanel->y = yy - 2;
    
    mSelectorPanel->w = mSlotSizePx + 4;
    mSelectorPanel->h = mSlotSizePx + 4;
    
    for (unsigned int i = 0; i < numberOfSlots; i++) {
        // Slot background panels
        Panel* slot = UI.CreatePanel();
        slot->name = Int.ToString(i);
        slot->alpha = 0.1f;
        slot->isActive = true;
        
        int slotX = xx + (i * (mSlotSizePx + mSpacingGapPx));
        int slotY = yy;
        
        slot->x = slotX;
        slot->y = slotY;
        slot->w = mSlotSizePx;
        slot->h = mSlotSizePx;
        
        mSlotPanels.push_back(slot);
        
        // Item counter
        Text* counter = UI.CreateText();
        counter->isActive = false;
        counter->color = Colors.white;
        counter->text = "0";
        counter->x = slotX + mSlotSizePx - mTextX;
        counter->y = slotY + mSlotSizePx - mTextY;
        
        mSlotCounters.push_back(counter);
        
        // Item sprites
        Sprite* spr = UI.CreateSprite();
        spr->isActive = false;
        spr->tint = Colors.white;
        spr->tint.a = 1.0f;
        spr->x = slotX + mSpacingGapPx - 2;
        spr->y = slotY + mSpacingGapPx - 2;
        spr->w = mSpriteSizePx;
        spr->h = mSpriteSizePx;
        
        spr->texture = Renderer.CreateTexture();
        
        mSlotSprites.push_back(spr);
        mCount.push_back(0);
        mItems.push_back("");
    }
    
    // Bar back panel
    mBar = UI.CreatePanel();
    mBar->alpha = 0.1f;
    mBar->x = xx - mSpacingGapPx;
    mBar->y = yy - mSpacingGapPx;
    mBar->w = numberOfSlots * (mSlotSizePx + mSpacingGapPx) + mSpacingGapPx;
    mBar->h = mSlotSizePx + (mSpacingGapPx * 2);
    
    TextureTag* matHealth = Resources.FindTextureTag("health_ind");
    TextureTag* matHunger = Resources.FindTextureTag("hunger_ind");
    matHealth->Load();
    matHunger->Load();
    
    /*
    const int numberOfIndicators = 10;
    for (unsigned int i=0; i < numberOfIndicators; i++) {
        // Health sprites
        Sprite* mSpriteHealth = UI.CreateSprite();
        mSpriteHealth->isActive = true;
        mSpriteHealth->tint = Colors.ltgray;
        mSpriteHealth->tint.a = 1.0f;
        mSpriteHealth->x = xx - numberOfIndicators + (i * 16) + 8;
        mSpriteHealth->y = yy - 24;
        mSpriteHealth->w = 16;
        mSpriteHealth->h = 16;
        
        mSpriteHealth->texture = Renderer.CreateTexture();
        mSpriteHealth->texture->UploadTextureToGPU(matHealth->buffer, matHealth->width, matHealth->height, GL_NEAREST, GL_NEAREST);
        mSpriteHealth->isActive = true;
        mHealthSprites.push_back(mSpriteHealth);
        
        // Hunger sprites
        Sprite* mSpriteHunger = UI.CreateSprite();
        mSpriteHunger->isActive = true;
        mSpriteHunger->tint = Colors.ltgray;
        mSpriteHunger->tint.a = 1.0f;
        mSpriteHunger->x = xx + numberOfIndicators + (16 * numberOfIndicators) + (i * 16) + 13;
        mSpriteHunger->y = yy - 24;
        mSpriteHunger->w = 16;
        mSpriteHunger->h = 16;
        
        mSpriteHunger->texture = Renderer.CreateTexture();
        mSpriteHunger->texture->UploadTextureToGPU(matHunger->buffer, matHunger->width, matHunger->height, GL_NEAREST, GL_NEAREST);
        mSpriteHunger->isActive = true;
        
        mHungerSprites.push_back(mSpriteHunger);
    }
    */
}

void InventorySystem::Update() {
    const int placeWidth = 7;
    const int numberOfSlots = mCount.size();
    const int totalWidth = numberOfSlots * mSlotSizePx + (numberOfSlots - 1) * mSpacingGapPx;
    const int xx = mHotbarX + (mWindowW - totalWidth) / 2;
    const int yy = mWindowH - mHotbarY;
    
    mSelectorPanel->x = xx + (mSelectorIndex * (mSlotSizePx + mSpacingGapPx)) - 2;
    mSelectorPanel->y = yy - 2;
    mSelectorPanel->alpha = 0.3f;
    
    for (unsigned int i = 0; i < mSlotPanels.size(); i++) {
        if (i == mSelectorIndex) {
            mSlotPanels[i]->isActive = false;
        } else {
            mSlotPanels[i]->isActive = true;
        }
    }
    
    for (unsigned int i = 0; i < mSlotSprites.size(); i++) {
        int slotX = xx + (i * (mSlotSizePx + mSpacingGapPx));
        int slotY = yy;
        
        Sprite* sprite = mSlotSprites[i];
        sprite->x = slotX + mSpacingGapPx - 2;
        sprite->y = slotY + mSpacingGapPx - 2;
    }
    
    for (unsigned int i = 0; i < mCount.size(); i++) {
        unsigned amount = mCount[i];
        if (amount > 1) {
            Text* counter = mSlotCounters[i];
            counter->isActive = true;
            counter->text = Int.ToString(amount);
            int slotX = xx + (i * (mSlotSizePx + mSpacingGapPx));
            int slotY = yy;
            
            counter->x = slotX + mSlotSizePx - 10;
            counter->y = slotY + mSlotSizePx - 15;
            
            if (mCount[i] > 9)   counter->x -= placeWidth;
            if (mCount[i] > 99)  counter->x -= placeWidth;
            if (mCount[i] > 999) counter->x -= placeWidth;
            
        } else {
            // Hide text on 1 or less items
            Text* counter = mSlotCounters[i];
            counter->isActive = false;
        }
    }
    
    // Health and hunger sprites
    const int numberOfIndicators = 10;
    for (unsigned int i=0; i < mHealthSprites.size(); i++) {
        Sprite* mSpriteHealth = mHealthSprites[i];
        mSpriteHealth->x = xx - numberOfIndicators + (i * 16) + 8;
        mSpriteHealth->y = yy - 24;
    }
    for (unsigned int i=0; i < mHungerSprites.size(); i++) {
        Sprite* mSpriteHunger = mHungerSprites[i];
        mSpriteHunger->x = xx + numberOfIndicators + (16 * numberOfIndicators) + (i * 16) + 13;
        mSpriteHunger->y = yy - 24;
    }
}

bool InventorySystem::AddItem(std::string name) {
    // Identify the item class
    if (mItemClass.find(name) == mItemClass.end()) 
        return false;
    ItemClass itemClass = mItemClass[name];
    
    // Check if the item currently exists in any slot
    for (unsigned int i=0; i < mItems.size(); i++) {
        if (mItems[i] != name || mCount[i] >= itemClass.stackMax) 
            continue;
        mCount[i]++;
        return true;
    }
    // Check selected item slot
    if (mItems[mSelectorIndex] == name || mItems[mSelectorIndex] == "") {
        if (mCount[mSelectorIndex] < itemClass.stackMax) {
            // Load item image
            TextureTag* matTag = Resources.FindTextureTag(name);
            mSlotSprites[mSelectorIndex]->texture->UploadTextureToGPU(matTag->buffer, matTag->width, matTag->height, GL_NEAREST, GL_NEAREST);
            mSlotSprites[mSelectorIndex]->isActive = true;
            
            mItems[mSelectorIndex] = name;
            mCount[mSelectorIndex]++;
            return true;
        }
    }
    
    // Check for an empty slot
    for (unsigned int i=0; i < mItems.size(); i++) {
        if (mItems[i] != "") 
            continue;
        
        // Load item image
        TextureTag* matTag = Resources.FindTextureTag(name);
        mSlotSprites[i]->texture->UploadTextureToGPU(matTag->buffer, matTag->width, matTag->height, GL_NEAREST, GL_NEAREST);
        mSlotSprites[i]->isActive = true;
        
        mItems[i] = name;
        mCount[i] = 1;
        return true;
    }
    
    return false;
}

bool InventorySystem::RemoveItem(unsigned int slot) {
    if (slot >= mCount.size()) 
        return false;
    std::string name = mItems[slot];
    if (name == "") 
        return false;
    // Identify the item class
    if (mItemClass.find(name) == mItemClass.end()) 
        return false;
    ItemClass itemClass = mItemClass[name];
    
    if (mItems[slot] != name) 
        return false;
    if (mCount[slot] == 0) 
        return false;
    
    mCount[slot]--;
    // Check no items left
    if (mCount[slot] == 0) {
        mItems[slot] = "";
        mSlotSprites[slot]->isActive = false;
    }
    return true;
}

std::string InventorySystem::QueryItem(unsigned int slot) {
    if (slot >= mCount.size()) 
        return "";
    return mItems[slot];
}

void InventorySystem::SetSelectorIndex(unsigned int index) {
    if (index < mCount.size()) 
        mSelectorIndex = index;
}

unsigned int InventorySystem::GetSelectorIndex() {
    return mSelectorIndex;
}

bool InventorySystem::CheckSlotEmpty(unsigned int slot) {
    if (mCount[slot] == 0) 
        return true;
    return false;
}

void InventorySystem::NextSlot() {
    mSelectorIndex++;
    if (mSelectorIndex >= mCount.size()) 
        mSelectorIndex = 0;
}

void InventorySystem::PrevSlot() {
    if (mSelectorIndex == 0) 
        mSelectorIndex = mCount.size();
    mSelectorIndex--;
}

bool InventorySystem::AddItemClass(std::string name, ItemClass itemClass) {
    if (CheckItemClassExists(name)) 
        return false;
    mItemClass[name] = itemClass;
    return true;
}

bool InventorySystem::CheckItemClassExists(std::string name) {
    if (mItemClass.find(name) == mItemClass.end()) 
        return false;
    return true;
}

void InventorySystem::OnWindowResize(int windowW, int windowH) {
    mWindowW = windowW;
    mWindowH = windowH;
    const int placeWidth = 7;
    const int numberOfSlots = mCount.size();
    const int totalWidth = numberOfSlots * mSlotSizePx + (numberOfSlots - 1) * mSpacingGapPx;
    const int xx = mHotbarX + (mWindowW - totalWidth) / 2;
    const int yy = mWindowH - mHotbarY;
    
    mSelectorPanel->x = xx + (mSelectorIndex * (mSlotSizePx + mSpacingGapPx));
    mSelectorPanel->y = yy;
    
    for (unsigned int i = 0; i < numberOfSlots; i++) {
        int slotX = xx + (i * (mSlotSizePx + mSpacingGapPx));
        int slotY = yy;
        
        Panel* slot = mSlotPanels[i];
        slot->x = slotX;
        slot->y = slotY;
        slot->w = mSlotSizePx;
        slot->h = mSlotSizePx;
        
        // Item counter
        Text* counter = mSlotCounters[i];
        counter->text = Int.ToString(mCount[i]);
        
        counter->x = slotX + mSlotSizePx - 10;
        counter->y = slotY + mSlotSizePx - 15;
        
        if (mCount[i] > 9)   counter->x -= placeWidth;
        if (mCount[i] > 99)  counter->x -= placeWidth;
        if (mCount[i] > 999) counter->x -= placeWidth;
    }
    
    mBar->x = xx - mSpacingGapPx;
    mBar->y = yy - mSpacingGapPx;
    mBar->w = numberOfSlots * (mSlotSizePx + mSpacingGapPx) + mSpacingGapPx;
    mBar->h = mSlotSizePx + (mSpacingGapPx * 2);
}

// Save inventory to <worldDirectory>/player.dat as text
bool InventorySystem::SaveToFile(const std::string& worldDirectory) const {
    // Build path: "worlds/MyWorld/player.dat" style
    std::string fileName = worldDirectory + "/player.dat";
    
    std::string buffer;
    
    // Version tag (like world.dat)
    buffer += "inventory_version=" + version + "\n";
    
    // Number of slots
    buffer += "slots=" + Int.ToString(static_cast<int>(mCount.size())) + "\n";
    
    // Selector index
    buffer += "selector_index=" + Int.ToString(static_cast<int>(mSelectorIndex)) + "\n";
    
    // One line per slot: slotN=name,count
    const unsigned int slotCount = static_cast<unsigned int>(mCount.size());
    for (unsigned int i = 0; i < slotCount; ++i) {
        buffer += "slot" + Int.ToString(static_cast<int>(i)) + "=";
        
        if (i < mItems.size() && mCount[i] > 0 && !mItems[i].empty()) {
            buffer += mItems[i] + "," + Int.ToString(static_cast<int>(mCount[i]));
        } else {
            // Empty slot
            buffer += ",0";
        }
        
        buffer += "\n";
    }
    
    // Write file using same serializer as world.dat / rules.dat
    Serializer.Serialize(fileName, (void*)buffer.data(), buffer.size());
    return true;
}

// Load inventory from <worldDirectory>/player.dat
bool InventorySystem::LoadFromFile(const std::string& worldDirectory) {
    std::string fileName = worldDirectory + "/player.dat";
    
    // Check size first
    unsigned int fileSize = Serializer.GetFileSize(fileName);
    if (fileSize == 0)
        return false;   // No player.dat yet
    
    // Read text buffer
    std::string buffer;
    buffer.resize(fileSize + 1);
    
    Serializer.Deserialize(fileName, (void*)buffer.data(), fileSize);
    
    // Split into lines
    std::vector<std::string> lines = String.Explode(buffer, '\n');
    
    // Start with a clean inventory (same number of slots)
    const unsigned int slotCount = static_cast<unsigned int>(mCount.size());
    for (unsigned int i = 0; i < slotCount; ++i) {
        if (i < mItems.size()) {
            mItems[i].clear();
        }
        if (i < mCount.size()) {
            mCount[i] = 0;
        }
        if (i < mSlotSprites.size()) {
            mSlotSprites[i]->isActive = false;
        }
    }
    
    // Default selector
    mSelectorIndex = 0;
    
    // Parse lines: key=value
    for (unsigned int i = 0; i < lines.size(); ++i) {
        if (lines[i].empty())
            continue;
        
        std::vector<std::string> lineSplit = String.Explode(lines[i], '=');
        if (lineSplit.size() < 2)
            continue;
        
        const std::string& key   = lineSplit[0];
        const std::string& value = lineSplit[1];
        
        // Version
        if (key == "inventory_version") {
            version = value;
            continue;
        }
        
        // Slots (we ignore mismatch; layout is defined by Initiate)
        if (key == "slots") {
            // int savedSlots = String.ToInt(value);
            // (You could compare savedSlots to slotCount here if you want)
            continue;
        }
        
        // Selector index
        if (key == "selector_index") {
            int idx = String.ToInt(value);
            if (idx < 0) idx = 0;
            if (idx >= static_cast<int>(slotCount)) idx = 0;
            mSelectorIndex = static_cast<unsigned int>(idx);
            continue;
        }
        
        // Slot lines: "slotN=name,count"
        if (key.rfind("slot", 0) == 0) {  // starts with "slot"
            // Get slot index from key
            std::string indexStr = key.substr(4); // after "slot"
            int idx = String.ToInt(indexStr);
            if (idx < 0 || idx >= static_cast<int>(slotCount))
                continue;
            
            // Split "name,count"
            std::vector<std::string> valSplit = String.Explode(value, ',');
            if (valSplit.size() < 2)
                continue;
            
            std::string itemName = valSplit[0];
            int count = String.ToInt(valSplit[1]);
            
            if (itemName.empty() || count <= 0)
                continue;
            
            // Clamp count to stackMax if we know this item class
            unsigned int finalCount = static_cast<unsigned int>(count);
            auto itCls = mItemClass.find(itemName);
            if (itCls != mItemClass.end()) {
                const ItemClass& cls = itCls->second;
                if (finalCount > cls.stackMax)
                    finalCount = cls.stackMax;
            }
            
            // If item class doesn’t exist anymore, skip the slot
            if (mItemClass.find(itemName) == mItemClass.end()) {
                Engine.console.Print("InventorySystem::LoadFromFile: unknown item class '" + itemName + "'");
                continue;
            }
            
            // Store into this slot
            mItems[idx] = itemName;
            mCount[idx] = finalCount;
            
            // Load / assign sprite texture like AddItem does
            if (idx < static_cast<int>(mSlotSprites.size())) {
                TextureTag* matTag = Resources.FindTextureTag(itemName);
                if (matTag != nullptr) {
                    matTag->Load();
                    mSlotSprites[idx]->texture->UploadTextureToGPU(
                        matTag->buffer,
                        matTag->width,
                        matTag->height,
                        GL_NEAREST,
                        GL_NEAREST
                    );
                    mSlotSprites[idx]->isActive = true;
                } else {
                    Engine.console.Print("InventorySystem::LoadFromFile: missing texture for '" + itemName + "'");
                    mSlotSprites[idx]->isActive = false;
                    mItems[idx].clear();
                    mCount[idx] = 0;
                }
            }
            
            continue;
        }
    }
    
    return true;
}

