#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk* chunk, bool doClearActors) {
    std::string chunkPosStr = Float.ToString( chunk->x ) + "_" + Float.ToString( chunk->y );
    std::string worldChunks = "worlds\\" + world.name + "\\chunks\\";
    std::string worldStatic = "worlds\\" + world.name + "\\static\\";
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    std::string buffer = "";
    
    // Save actors within chunk range
    unsigned int saveCounter = 0;
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    if (numberOfActors > 0) {
        std::vector<Actor*> terminationList;
        for (unsigned int a = 0; a < numberOfActors; a++) {
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
            if (Physics.Raycast(actorPos, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) {
                actorPos.y = hit.point.y;
                if (((GameObject*)hit.collider->getUserData()) != chunk->gameObject) 
                    continue;
            } else {
                Engine.console.Print("TODO DEBUG :: actor cannot be saved, not in a valid chunk?");
                actorPos.y -= 500.0f;
            }
            
            // Position
            std::string posStrX = Float.ToString(actorPos.x);
            std::string posStrY = Float.ToString(actorPos.y);
            std::string posStrZ = Float.ToString(actorPos.z);
            
            // Current actor age
            std::string age = IntLong.ToString( actor->physical.GetAge() );
            
            // Inventory items
            std::string items = "";
            unsigned int numberOfItems = actor->inventory.itemClassList.size();
            if (!actor->inventory.itemClassList.empty()) {
                for (unsigned int i = 0; i < numberOfItems; i++) {
                    items += actor->inventory.itemClassList[i] + "#";
                }
            } else {
                items = "none";
            }
            
            // Genome
            std::string genome = AI.genomes.ExtractGenome(actor);
            
            // Memories
            std::string memories;
            unsigned int numberOfMemories = actor->memories.GetNumberOfMemories();
            if (numberOfMemories == 0) {
                memories = "none";
            } else {
                for (unsigned int i = 0; i < numberOfMemories; i++) {
                    memories += actor->memories.GetMemoryNameByIndex(i) + "=" + 
                                actor->memories.GetMemoryValueByIndex(i) + "|";
                }
            }
            
            // Assemble final string
            buffer += posStrX + "~" + posStrY + "~" + posStrZ + "~" + age + "~" + items + "~" + genome + "~" + memories + '\n';
            if (doClearActors) 
                terminationList.push_back(actor);
            
            actor->isSaved = true;
            saveCounter++;
        }
        
        // Delete the saved actors if requested
        for (unsigned int a = 0; a < terminationList.size(); a++) 
            KillActor( terminationList[a] );
        
        unsigned int bufferSz = buffer.size();
        if (bufferSz != 0) 
            Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
    }
    
    // Save static objects
    unsigned int numberOfStatics = chunk->statics.size();
    if (numberOfStatics > 0) {
        std::vector<StaticElement> staticElements(numberOfStatics);
        for (unsigned int s = 0; s < numberOfStatics; s++) {
            staticElements[s].position = chunk->statics[s].position;
            staticElements[s].rotation = chunk->statics[s].rotation;
            staticElements[s].scale    = chunk->statics[s].scale;
            staticElements[s].color    = chunk->statics[s].color;
            staticElements[s].function = chunk->statics[s].function;
            
            staticElements[s].mesh     = static_cast<unsigned int>(chunk->statics[s].mesh);
            staticElements[s].type     = static_cast<unsigned int>(chunk->statics[s].type);
        }
        
        Serializer.Serialize(staticName, (void*)staticElements.data(), sizeof(StaticElement) * numberOfStatics);
    }
    
    return true;
}
