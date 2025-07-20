#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

#include <GameEngineFramework/plugins.h>



bool ChunkManager::LoadWorld(void) {
    
    world.doGenerateChunks = true;
    
    // Setup world directory structure
    if (!WorldDirectoryInitiate()) {
        // Initiate new world settings
        
        AddWorldRule("do_auto_breeding", "true");
        
    }
    
    // Load world data file
    std::string worldName   = "worlds/" + world.name;
    
    unsigned int worldFileSize = Serializer.GetFileSize(worldName + "/world.dat");
    unsigned int rulesFileSize = Serializer.GetFileSize(worldName + "/rules.dat");
    if (worldFileSize == 0 || rulesFileSize == 0) 
        return false;
    
    std::string worldDataBuffer;
    worldDataBuffer.resize(worldFileSize + 1);
    
    Serializer.Deserialize(worldName + "/world.dat", (void*)worldDataBuffer.data(), worldFileSize);
    std::vector<std::string> bufferArray = String.Explode(worldDataBuffer, '\n');
    
    unsigned int numberOfLines = bufferArray.size();
    for (unsigned int i=0; i < numberOfLines; i++) {
        std::vector<std::string> lineSplit = String.Explode(bufferArray[i], '=');
        if (lineSplit.size() < 2) 
            continue;
        
        if (lineSplit[0] == "player_position") {
            std::vector<std::string> playerPositionString = String.Explode(lineSplit[1], ',');
            if (playerPositionString.size() < 3) 
                continue;
            float posX = String.ToFloat(playerPositionString[0]);
            float posY = String.ToFloat(playerPositionString[1]);
            float posZ = String.ToFloat(playerPositionString[2]);
            Engine.cameraController->SetPosition( glm::vec3(posX, posY, posZ) );
            continue;
        }
        
        if (lineSplit[0] == "looking_angle") {
            std::vector<std::string> playerLookAngleString = String.Explode(lineSplit[1], ',');
            if (playerLookAngleString.size() < 2) 
                continue;
            float yaw   = String.ToFloat(playerLookAngleString[0]);
            float pitch = String.ToFloat(playerLookAngleString[1]);
            Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
            cameraPtr->mouseLookAngle = glm::vec2(yaw, pitch);
            continue;
        }
        
        if (lineSplit[0] == "world_seed") {
            worldSeed = String.ToFloat(lineSplit[1]);
            continue;
        }
        
        if (lineSplit[0] == "world_time") {
            float worldTime = String.ToInt(lineSplit[1]);
            Weather.SetTime( worldTime );
            continue;
        }
        
        if (lineSplit[0] == "weather_current") {
            int weatherCurrent = String.ToInt(lineSplit[1]);
            Weather.SetWeather( (WeatherType)weatherCurrent );
            continue;
        }
        
        if (lineSplit[0] == "weather_next") {
            int weatherNext = String.ToInt(lineSplit[1]);
            Weather.SetWeatherNext( (WeatherType)weatherNext );
            continue;
        }
        
        if (lineSplit[0] == "weather_cycle") {
            float weatherCycle = String.ToFloat(lineSplit[1]);
            Weather.SetWeatherCycleCounter( weatherCycle );
            continue;
        }
        
    }
    
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

