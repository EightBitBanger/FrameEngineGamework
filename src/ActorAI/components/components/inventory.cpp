#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;

InventoryManager::InventoryManager() : 
    inHandItemName(""),
    damageMul(1.0f),
    defenseMul(1.0f),
    
    handPosition(0.0f, 0.0f, 0.0f),
    handOffset(0.0f, 0.0f, 0.0f),
    handRotation(0.0f, 0.0f, 0.0f),
    handScale(1.0f, 1.0f, 1.0f),
    holdingRenderer(nullptr)
{}

void InventoryManager::GiveItem(const std::string& itemClassification) {
    if (itemClassification.size() == 0) 
        return;
    
    // Check to initiate the mesh renderer
    if (holdingRenderer == nullptr) {
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        
        newRenderer->mesh     = Renderer.CreateMesh();
        newRenderer->material = Renderer.CreateMaterial();
        
        newRenderer->material->shader = Resources.shaders.color;
        
        newRenderer->material->ambient = Colors.white;
        newRenderer->material->diffuse = Colors.white;
        
        AI.sceneMain->AddMeshRendererToSceneRoot(newRenderer);
        holdingRenderer = newRenderer;
    }
    Mesh* handMesh = holdingRenderer->mesh;
    
    std::vector<std::string> itemData = String.Explode(itemClassification, ',');
    if (itemData.size() == 0) 
        return;
    
    // Keep item information
    itemClassList.push_back(itemClassification);
    
    for (unsigned int i=0; i < itemData.size(); i++) {
        std::vector<std::string> kvPair = String.Explode(itemData[i], ':');
        if (kvPair.size() < 3) 
            continue;
        std::string& key = kvPair[0];
        std::string& value = kvPair[1];
        String.RemoveWhiteSpace(key);
        
        if (key == "damage")  {damageMul  = String.ToFloat(value);} else 
        if (key == "defense") {defenseMul = String.ToFloat(value);} else 
        if (key == "name")    {inHandItemName   = value;}
        
        if (key == "build") {
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
    
    holdingRenderer->mesh->Load();
    holdingRenderer->isActive = true;
}
