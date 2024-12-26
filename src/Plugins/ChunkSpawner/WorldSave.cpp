#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

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
    
    std::string dataBuffer = "";
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    float playerYaw   = cameraPtr->mouseLookAngle.x;
    float playerPitch = cameraPtr->mouseLookAngle.y;
    
    dataBuffer += Float.ToString(playerPosition.x) + "," + Float.ToString(playerPosition.y) + "," + Float.ToString(playerPosition.z) + "\n";
    dataBuffer += Float.ToString(playerYaw) + "," + Float.ToString(playerPitch) + "\n";
    dataBuffer += Int.ToString(worldSeed) + "\n";
    
    if (world.doAutoBreeding) {dataBuffer += "true\n";} else {dataBuffer += "false\n";}
    
    Serializer.Serialize(worldName + "/world.dat", (void*)dataBuffer.data(), dataBuffer.size());
    
    return 1;
}

