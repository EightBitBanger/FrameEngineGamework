#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

extern WeatherSystem  weather;


bool ChunkManager::LoadWorld(void) {
    
    world.doGenerateChunks = true;
    
    // Setup world directory structure
    WorldDirectoryInitiate();
    
    // Load world data file
    std::string worldName   = "worlds/" + world.name;
    
    unsigned int worldFileSize = Serializer.GetFileSize(worldName + "/world.dat");
    unsigned int rulesFileSize = Serializer.GetFileSize(worldName + "/rules.dat");
    
    if (worldFileSize == 0) 
        return false;
    
    if (rulesFileSize == 0) 
        return false;
    
    isInitiated = true;
    
    std::string worldDataBuffer;
    worldDataBuffer.resize(worldFileSize + 1);
    
    Serializer.Deserialize(worldName + "/world.dat", (void*)worldDataBuffer.data(), worldFileSize);
    
    std::vector<std::string> bufferArray = String.Explode(worldDataBuffer, '\n');
    
    std::vector<std::string> position = String.Explode(bufferArray[0], ',');
    std::vector<std::string> lookingAngle = String.Explode(bufferArray[1], ',');
    
    float posX = String.ToFloat(position[0]);
    float posY = String.ToFloat(position[1]);
    float posZ = String.ToFloat(position[2]);
    
    float yaw   = String.ToFloat(lookingAngle[0]);
    float pitch = String.ToFloat(lookingAngle[1]);
    worldSeed = String.ToInt(bufferArray[2]);
    float worldTime = String.ToInt(bufferArray[3]);
    // Weather
    int weatherCurrent = String.ToInt(bufferArray[4]);
    int weatherNext = String.ToInt(bufferArray[5]);
    float weatherCycleCounter = String.ToFloat(bufferArray[6]);
    
    weather.SetWeather( (WeatherType)weatherCurrent );
    weather.SetWeatherNext( (WeatherType)weatherNext );
    weather.SetWeatherCycleCounter(weatherCycleCounter);
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    cameraPtr->mouseLookAngle = glm::vec2(yaw, pitch);
    
    Engine.cameraController->SetPosition( glm::vec3(posX, posY, posZ) );
    
    weather.SetTime( worldTime );
    
    // Load world rules
    
    std::string rulesDataBuffer;
    rulesDataBuffer.resize(rulesFileSize + 1);
    
    Serializer.Deserialize(worldName + "/rules.dat", (void*)rulesDataBuffer.data(), rulesFileSize);
    
    std::vector<std::string> ruleList = String.Explode(rulesDataBuffer, '\n');
    
    unsigned int numberOfRules = ruleList.size();
    
    for (unsigned int i=0; i < numberOfRules; i++) {
        
        std::vector<std::string> rule = String.Explode(ruleList[i], '=');
        
        if (rule.size() < 2) 
            continue;
        
        std::pair<std::string, std::string> rulePair( rule[0], rule[1] );
        
        mWorldRules.push_back(rulePair);
        
        continue;
    }
    
    return true;
}

