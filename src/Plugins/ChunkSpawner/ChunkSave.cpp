#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk* chunk, bool doClearActors) {
    std::string chunkPosStr = Float.ToString( chunk->x ) + "_" + Float.ToString( chunk->y );
    std::string worldChunks = "worlds\\" + world.name + "\\chunks\\";
    std::string worldStatic = "worlds\\" + world.name + "\\static\\";
    
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    std::string buffer = "";
    
    // Save actors within chunk range
    unsigned int saveCounter=0;
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    if (numberOfActors > 0) {
        std::vector<Actor*> terminationList;
        
        for (unsigned int a=0; a < numberOfActors; a++) {
            Actor* actor = AI.GetActor(a);
            if (actor == nullptr) 
                continue;
            
            if (actor->isSaved) 
                continue;
            
            if (!actor->isActive || actor->isGarbage) 
                continue;
            
            glm::vec3 actorPos = actor->navigation.GetPosition();
            actorPos.y += 500.0f;
            
            // Check query points
            Hit hit;
            // Get point height
            if (Physics.Raycast(actorPos, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) {
                actorPos.y = hit.point.y;
                if (((GameObject*)hit.collider->getUserData()) != chunk->gameObject) 
                    continue;
            } else {
                // TODO handle actors that are not in any chunk..
                //Engine.console.Print("DEBUG :: actor cannot be saved, not in a valid chunk?");
                
                actorPos.y -= 500.0f;
            }
            
            // Position
            std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                      Float.ToString(actorPos.y) + "~" + 
                                      Float.ToString(actorPos.z) + "~";
            
            // Current actor age
            std::string actorAge = IntLong.ToString( actor->physical.GetAge() ) + "~";
            
            // Genome
            std::string actorGenome = AI.genomes.ExtractGenome(actor) + "~";
            
            // Memories
            std::string actorMemories;
            unsigned int numberOfMemories = actor->memories.GetNumberOfMemories();
            for (unsigned int i=0; i < numberOfMemories; i++) {
                
                actorMemories += actor->memories.GetMemoryNameByIndex(i) + "=" + 
                                 actor->memories.GetMemoryValueByIndex(i) + "|";
                
            }
            if (numberOfMemories == 0) 
                actorMemories = "none";
            
            buffer += actorPosStr + actorAge + actorGenome + actorMemories + '\n';
            
            if (doClearActors) 
                terminationList.push_back(actor);
            
            actor->isSaved = true;
            saveCounter++;
        }
        
        // Delete the actors
        for (unsigned int a=0; a < terminationList.size(); a++) 
            KillActor( terminationList[a] );
        
        // Check to delete the save file if no actors where counted
        if (saveCounter == 0) {
            
            if (fs.FileExists(chunkName)) 
                fs.FileDelete(chunkName);
        }
        
        unsigned int bufferSz = buffer.size();
        if (bufferSz != 0) 
            Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
    } else {
        
        // File should not exists as no actors exist in this chunk
        if (fs.FileExists(chunkName)) 
            fs.FileDelete(chunkName);
    }
    
    // Save static objects
    unsigned int numberOfStatics = chunk->statics.size();
    StaticElement staticElements[numberOfStatics];
    if (numberOfStatics > 0) {
        
        for (unsigned int s=0; s < numberOfStatics; s++) {
            staticElements[s].position = chunk->statics[s].position;
            staticElements[s].rotation = chunk->statics[s].rotation;
            staticElements[s].scale    = chunk->statics[s].scale;
            staticElements[s].color    = chunk->statics[s].color;
            staticElements[s].type     = chunk->statics[s].type;
            continue;
        }
        
        Serializer.Serialize(staticName, (void*)staticElements, sizeof(StaticElement) * numberOfStatics);
    }
    
    return 0;
}
