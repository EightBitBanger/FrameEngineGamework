#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

extern WeatherSystem  weather;


bool ChunkManager::SaveWorld(void) {
    
    if (!world.doGenerateChunks) 
        return false;
    
    // Setup world directory structure
    InitiateWorld();
    
    std::string worldName   = "worlds/" + world.name;
    
    // Save world chunks
    
    unsigned int numberOfChunks = chunks.size();
    unsigned int numberOfActors = actors.size();
    
    for (unsigned int c=0; c < numberOfChunks; c++) 
        SaveChunk( chunks[c], false );
    
    // Reset actor save marker
    for (unsigned int a=0; a < numberOfActors; a++) {
        
        GameObject* actorObject = actors[a];
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        actorPtr->SetUserBitmask(0);
        
        continue;
    }
    
    // Save world data file
    
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    float playerYaw   = cameraPtr->mouseLookAngle.x;
    float playerPitch = cameraPtr->mouseLookAngle.y;
    
    std::string worldDataBuffer = "";
    worldDataBuffer += Float.ToString(playerPosition.x) + "," + Float.ToString(playerPosition.y) + "," + Float.ToString(playerPosition.z) + "\n";
    worldDataBuffer += Float.ToString(playerYaw) + "," + Float.ToString(playerPitch) + "\n";
    worldDataBuffer += Int.ToString(worldSeed) + "\n";
    worldDataBuffer += Float.ToString(weather.GetTime()) + "\n";
    // Weather
    worldDataBuffer += Int.ToString((int)weather.GetWeatherCurrent()) + "\n";
    worldDataBuffer += Int.ToString((int)weather.GetWeatherNext()) + "\n";
    worldDataBuffer += Float.ToString(weather.GetWeatherCycleCounter()) + "\n";
    
    Serializer.Serialize(worldName + "/world.dat", (void*)worldDataBuffer.data(), worldDataBuffer.size());
    
    // Save world rules
    
    std::string rulesDataBuffer = "";
    
    unsigned int sizeofRulesList = mWorldRules.size();
    
    for (unsigned int i=0; i < sizeofRulesList; i++) {
        
        std::pair<std::string, std::string> rulePair = mWorldRules[i];
        
        rulesDataBuffer += rulePair.first + "=" + rulePair.second + "\n";
        
        continue;
    }
    
    Serializer.Serialize(worldName + "/rules.dat", (void*)rulesDataBuffer.data(), rulesDataBuffer.size());
    
    return 1;
}


