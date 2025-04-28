#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk& chunk, bool doClearActors) {
    
    std::string chunkPosStr = Float.ToString( chunk.x ) + "_" + Float.ToString( chunk.y );
    std::string worldChunks = "worlds/" + world.name + "/chunks/";
    std::string worldStatic = "worlds/" + world.name + "/static/";
    
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    std::string buffer = "";
    
    
    // Save actors within chunk range
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    if (numberOfActors > 0) {
        for (unsigned int a=0; a < numberOfActors; a++) {
            
            Actor* actor = AI.GetActorFromSimulation(a);
            //GameObject* actorObject = (GameObject*)actor->user.GetUserDataA();
            
            glm::vec3 actorPos = actor->navigation.GetPosition();
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            
            float chunkSz = chunkSize * 0.5f;
            
            // Check actor within chunk bounds
            if (((actorPos.x < (chunkPos.x - chunkSz)) | actorPos.x > (chunkPos.x + chunkSz)) | 
                ((actorPos.z < (chunkPos.z - chunkSz)) | actorPos.z > (chunkPos.z + chunkSz)))
                continue;
            
            // Position
            std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                      Float.ToString(actorPos.y) + "~" + 
                                      Float.ToString(actorPos.z) + "~";
            
            // Current actor age
            std::string actorAge = IntLong.ToString( actor->physical.GetAge() ) + "~";
            
            // Genome
            std::string actorGenome = AI.genomes.ExtractGenome(actor);
            buffer += actorPosStr + actorAge + actorGenome + '\n';
            
            if (!doClearActors) 
                continue;
            
            actor->user.SetUserBitmask(0);
            AI.KillActor( actor );
            
            continue;
        }
        
        unsigned int bufferSz = buffer.size();
        
        if (bufferSz != 0) 
            Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
        
    }
    
    
    
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
        
        Serializer.Serialize(staticName, (void*)staticElements, sizeof(StaticElement) * numberOfStatics);
        
    }
    
    
    return 0;
}
