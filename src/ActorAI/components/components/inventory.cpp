#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;

InventoryManager::InventoryManager() : 
    damageMul(1.0f),
    defenseMul(1.0f),
    inHandItemClass(""),
    handPosition(0.0f, 0.0f, 0.0f),
    handOffset(0.0f, 0.0f, 0.0f),
    handRotation(0.0f, 0.0f, 0.0f),
    handScale(1.0f, 1.0f, 1.0f),
    holdingRenderer(nullptr),
    doEquipWeapon(false),
    doBuildHandMesh(false)
{}

bool InventoryManager::AddItem(const std::string& itemClassification) {
    if (itemClassification.empty()) 
        return false;
    itemClassList.push_back(itemClassification);
    return true;
}

bool InventoryManager::GiveItem(const std::string& itemClassification) {
    AddItem(itemClassification);
    EquipItem(itemClassification);
    return true;
}

bool InventoryManager::RemoveItem(unsigned int index) {
    if (index >= itemClassList.size()) 
        return false;
    if (!inHandItemClass.empty() && itemClassList[index] == inHandItemClass) {
        UnequipItem();
    }
    
    itemClassList.erase(itemClassList.begin() + index);
    return true;
}

bool InventoryManager::EquipItem(unsigned int index) {
    if (index >= itemClassList.size()) 
        return false;
    std::string itemClassification = itemClassList[index];
    
    inHandItemClass = itemClassification;
    doBuildHandMesh = true;
    
    return true;
}

bool InventoryManager::EquipItem(const std::string& itemClassification) {
    for (unsigned int i = 0; i < itemClassList.size(); ++i) {
        if (itemClassList[i] == itemClassification) {
            return EquipItem(i);
        }
    }
    return false;
}

bool InventoryManager::EquipWeapon() {
    if (itemClassList.empty()) 
        return false;
    int maxIndex = -1;
    float maxDamage = -1.0f;
    
    for (unsigned int i = 0; i < itemClassList.size(); ++i) {
        float itemDamage = 1.0f;
        std::vector<std::string> itemData = String.Explode(itemClassList[i], ',');
        
        for (unsigned int j = 0; j < itemData.size(); ++j) {
            std::vector<std::string> kvPair = String.Explode(itemData[j], ':');
            if (kvPair.size() >= 2) {
                std::string key = kvPair[0];
                String.RemoveWhiteSpace(key);
                if (key == "damage") {
                    itemDamage = String.ToFloat(kvPair[1]);
                }
            }
        }
        
        if (itemDamage > maxDamage) {
            maxDamage = itemDamage;
            maxIndex = static_cast<int>(i);
        }
    }
    
    if (maxIndex != -1) {
        return EquipItem(static_cast<unsigned int>(maxIndex));
    }
    return false;
}

void InventoryManager::UnequipItem() {
    damageMul = 1.0f;
    defenseMul = 1.0f;
    inHandItemClass = "";
    doBuildHandMesh = false;
    
    if (holdingRenderer != nullptr) {
        holdingRenderer->isActive = false;
    }
}

void InventoryManager::BuildHandMesh(const std::string& itemClassification) {
    if (itemClassification.empty()) 
        return;
    
    // Allocate the MeshRenderer on first build if it doesn't exist
    if (holdingRenderer == nullptr) {
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        newRenderer->mesh     = Renderer.CreateMesh();
        newRenderer->material = Renderer.CreateMaterial();
        
        newRenderer->material->shader = Resources.shaders.color;
        newRenderer->material->ambient = Colors.white;
        newRenderer->material->diffuse = Colors.white;
        AI.sceneMain->AddMeshRendererToSceneRoot(newRenderer, RENDER_QUEUE_GEOMETRY);
        holdingRenderer = newRenderer;
    }
    
    Mesh* handMesh = holdingRenderer->mesh;
    handMesh->ClearSubMeshes();
    std::vector<std::string> itemData = String.Explode(itemClassification, ',');
    if (itemData.empty()) 
        return;
        
    for (unsigned int i = 0; i < itemData.size(); i++) {
        std::vector<std::string> kvPair = String.Explode(itemData[i], ':');
        if (kvPair.size() < 2) 
            continue;
        std::string key = kvPair[0];
        String.RemoveWhiteSpace(key);
        
        if (key == "damage") { 
            damageMul = String.ToFloat(kvPair[1]);
        } else if (key == "defense") { 
            defenseMul = String.ToFloat(kvPair[1]);
        } else if (key == "build" && kvPair.size() >= 4) {
            std::string& position = kvPair[1];
            std::string& scale    = kvPair[2];
            std::string& color    = kvPair[3];
            
            String.RemoveWhiteSpace(position);
            String.RemoveWhiteSpace(scale);
            String.RemoveWhiteSpace(color);
            std::vector<std::string> vecPosStr   = String.Explode(position, '`');
            std::vector<std::string> vecScaleStr = String.Explode(scale, '`');
            std::vector<std::string> vecColorStr = String.Explode(color, '`');
            if (vecPosStr.size() != 3 || vecScaleStr.size() != 3 || vecColorStr.size() != 3) 
                continue;
            float posX = String.ToFloat(vecPosStr[0]);
            float posY = String.ToFloat(vecPosStr[1]);
            float posZ = String.ToFloat(vecPosStr[2]);
            
            float scaleX = String.ToFloat(vecScaleStr[0]);
            float scaleY = String.ToFloat(vecScaleStr[1]);
            float scaleZ = String.ToFloat(vecScaleStr[2]);
            
            float colorX = String.ToFloat(vecColorStr[0]);
            float colorY = String.ToFloat(vecColorStr[1]);
            float colorZ = String.ToFloat(vecColorStr[2]);
            Color colorVec(colorX, colorY, colorZ);
            handMesh->AddCube(posX, posY, posZ, scaleX, scaleY, scaleZ, colorVec);
        }
    }
    
    handPosition = glm::vec3(0.0f, -0.5f, 0.0f);
    handOffset   = glm::vec3(-0.01f, 0.3f, 0.0f);
    handRotation = glm::vec3(1.570795f, 0.0f, 0.0f);
    handScale    = glm::vec3(0.1f, 0.8f, 0.1f);
    handMesh->Load();
    holdingRenderer->isActive = true;
}
