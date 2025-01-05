#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk& chunk, bool doClearActors) {
    
    std::string chunkPosStr = Float.ToString( chunk.x ) + "_" + Float.ToString( chunk.y );
    std::string worldChunks = "worlds/" + world.name + "/chunks/";
    std::string worldStatic = "worlds/" + world.name + "/static/";
    
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    std::string buffer = "";
    
    
    // Save actors within chunk range
    
    unsigned int numberOfActors = actors.size();
    if (numberOfActors > 0) {
        for (unsigned int a=0; a < numberOfActors; a++) {
            
            GameObject* actorObject = actors[a];
            
            glm::vec3 actorPos = actorObject->GetPosition();
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            
            float chunkSz = chunkSize * 0.5f;
            
            Actor* actorPtr = actorObject->GetComponent<Actor>();
            
            // Check actor within chunk bounds
            if (((actorPos.x < (chunkPos.x - chunkSz)) | actorPos.x > (chunkPos.x + chunkSz)) | 
                ((actorPos.z < (chunkPos.z - chunkSz)) | actorPos.z > (chunkPos.z + chunkSz)))
                continue;
            
            // Check mark as saved
            if (actorPtr->GetUserBitmask() == 'S') 
                continue;
            
            actorPtr->SetUserBitmask('S');
            
            if (actorPtr->GetName() == "") 
                continue;
            
            if (!actorPtr->GetActive()) 
                continue;
            
            std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                      Float.ToString(actorPos.y) + "~" + 
                                      Float.ToString(actorPos.z) + "~";
            
            std::string actorAge = IntLong.ToString( actorPtr->GetAge() ) + "~";
            
            std::string actorGenome = AI.genomes.ExtractGenome(actorPtr);
            
            buffer += actorPosStr + actorAge + actorGenome + '\n';
            
            if (!doClearActors) 
                continue;
            
            actorPtr->SetUserBitmask(0);
            
            KillActor( actorObject );
            
            continue;
        }
        
    }
    
    unsigned int bufferSz = buffer.size();
    
    if (bufferSz != 0) 
        Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
    
    
    // Save static objects
    
    unsigned int numberOfStatics = chunk.statics.size();
    
    StaticElement staticElements[numberOfStatics];
    
    if (numberOfStatics > 0) {
        
        for (unsigned int s=0; s < numberOfStatics; s++) {
            
            staticElements[s].position = glm::vec3(chunk.statics[s].x, 
                                                   chunk.statics[s].y, 
                                                   chunk.statics[s].z);
            
            staticElements[s].color = glm::vec3(chunk.statics[s].r, 
                                                chunk.statics[s].g, 
                                                chunk.statics[s].b);
            
            staticElements[s].type = chunk.statics[s].type;
            
            continue;
        }
        
    }
    
    Serializer.Serialize(staticName, (void*)staticElements, sizeof(StaticElement) * numberOfStatics);
    
    return 0;
}
