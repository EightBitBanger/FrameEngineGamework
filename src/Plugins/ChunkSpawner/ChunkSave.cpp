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
        std::vector<Actor*> terminationList;
        
        for (unsigned int a=0; a < numberOfActors; a++) {
            Actor* actor = AI.GetActorFromSimulation(a);
            if (actor->isSaved) 
                continue;
            
            if (!actor->isActive || actor->isGarbage) 
                continue;
            
            float chunkSz = chunkSize / 1.5f;
            glm::vec3 actorPos = actor->navigation.GetPosition();
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            
            // Check actor within chunk bounds
            if (actorPos.x >= (chunkPos.x - chunkSz) && actorPos.x <= (chunkPos.x + chunkSz) && 
                actorPos.z >= (chunkPos.z - chunkSz) && actorPos.z <= (chunkPos.z + chunkSz)) {
                
                // Position
                std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                          Float.ToString(actorPos.y) + "~" + 
                                          Float.ToString(actorPos.z) + "~";
                
                // Current actor age
                std::string actorAge = IntLong.ToString( actor->physical.GetAge() ) + "~";
                
                // Genome
                std::string actorGenome = AI.genomes.ExtractGenome(actor);
                buffer += actorPosStr + actorAge + actorGenome + '\n';
                
                if (doClearActors) 
                    terminationList.push_back(actor);
                
                actor->isSaved = true;
            }
            
        }
        
        // Delete the actors
        for (unsigned int a=0; a < terminationList.size(); a++) {
            Actor* actor = terminationList[a];
            
            KillActor( terminationList[a] );
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
            
            staticElements[s].position.x = chunk.statics[s].x;
            staticElements[s].position.y = chunk.statics[s].y;
            staticElements[s].position.z = chunk.statics[s].z;
            
            staticElements[s].color.x = chunk.statics[s].r;
            staticElements[s].color.y = chunk.statics[s].g;
            staticElements[s].color.z = chunk.statics[s].b;
            
            staticElements[s].type = chunk.statics[s].type;
            
            continue;
        }
        
        Serializer.Serialize(staticName, (void*)staticElements, sizeof(StaticElement) * numberOfStatics);
        
    }
    
    
    return 0;
}
