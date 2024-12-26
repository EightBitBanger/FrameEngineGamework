#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadWorld(void) {
    
    world.doGenerateChunks = true;
    
    // Setup world directory structure
    InitiateWorld();
    
    // Load world data file
    std::string worldName   = "worlds/" + world.name;
    
    unsigned int fileSize = Serializer.GetFileSize(worldName + "/world.dat");
    
    if (fileSize == 0) 
        return false;
    
    isInitiated = true;
    
    std::string dataBuffer;
    dataBuffer.resize(fileSize + 1);
    
    Serializer.Deserialize(worldName + "/world.dat", (void*)dataBuffer.data(), fileSize);
    
    std::vector<std::string> bufferArray = String.Explode(dataBuffer, '\n');
    
    std::vector<std::string> position = String.Explode(bufferArray[0], ',');
    std::vector<std::string> lookingAngle = String.Explode(bufferArray[1], ',');
    
    float posX = String.ToFloat(position[0]);
    float posY = String.ToFloat(position[1]);
    float posZ = String.ToFloat(position[2]);
    
    float yaw   = String.ToFloat(lookingAngle[0]);
    float pitch = String.ToFloat(lookingAngle[1]);
    worldSeed = String.ToInt(bufferArray[2]);
    
    if (bufferArray[3] == "true") {world.doAutoBreeding = true;} else {world.doAutoBreeding = false;}
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    cameraPtr->mouseLookAngle = glm::vec2(yaw, pitch);
    
    Engine.cameraController->SetPosition( glm::vec3(posX, posY, posZ) );
    
    return true;
}

