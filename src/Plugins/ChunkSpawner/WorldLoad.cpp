#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadWorld(void) {
    
    world.doGenerateChunks = true;
    
    std::string worldName   = "worlds/" + world.name;
    std::string worldChunks = "worlds/" + world.name + "/chunks";
    std::string worldStatic = "worlds/" + world.name + "/static";
    
    // Check world directory structure exists
    if (!fs.DirectoryExists(worldName)) {
        
        fs.DirectoryCreate(worldName);
        
        if (!fs.DirectoryExists(worldChunks)) 
            fs.DirectoryCreate(worldChunks);
        
        if (!fs.DirectoryExists(worldStatic)) 
            fs.DirectoryCreate(worldStatic);
        
    }
    
    // Load world data file
    
    unsigned int fileSize = Serializer.GetFileSize(worldName + "/world.dat");
    
    if (fileSize == 0) 
        return false;
    
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
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    cameraPtr->mouseLookAngle = glm::vec2(yaw, pitch);
    
    Engine.cameraController->SetPosition( glm::vec3(posX, posY, posZ) );
    
    return true;
}

