#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk* chunk, bool doClearActors) {
    std::string chunkPosStr = Float.ToString( chunk->x ) + "_" + Float.ToString( chunk->y );
    std::string worldChunks = "worlds\\" + world.name + "\\chunks\\";
    std::string worldStatic = "worlds\\" + world.name + "\\static\\";
    std::string worldItems  = "worlds\\" + world.name + "\\items\\";
    
    std::string chunkName   = worldChunks + chunkPosStr;
    std::string staticName  = worldStatic + chunkPosStr;
    std::string itemsName   = worldItems + chunkPosStr;
    
    std::string buffer = "";
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
            
            // Calculate chunk coordinates
            int targetChunkX = (int)Math.Round(actorPos.x / (float)chunkSize) * chunkSize;
            int targetChunkZ = (int)Math.Round(actorPos.z / (float)chunkSize) * chunkSize;
            
            // Ground height adjustment using raycast
            Hit hit;
            if (Physics.Raycast(actorPos + glm::vec3(0, 500.0f, 0), glm::vec3(0, -1, 0), 2000.0f, hit, LayerMask::Ground)) 
                actorPos.y = hit.point.y;
            
            // Serialize actor attributes
            std::string posStrX = Float.ToString(actorPos.x);
            std::string posStrY = Float.ToString(actorPos.y);
            std::string posStrZ = Float.ToString(actorPos.z);
            
            std::string age = IntLong.ToString( actor->physical.GetAge() );
            
            std::string health     = Float.ToString( actor->biological.health );
            std::string hunger     = Float.ToString( actor->biological.hunger );
            std::string defense    = Float.ToString( actor->biological.defense );
            std::string strength   = Float.ToString( actor->biological.strength );
            std::string saturation = Float.ToString( actor->biological.saturation );
            
            std::string items = "";
            unsigned int numberOfItems = actor->inventory.itemClassList.size();
            if (!actor->inventory.itemClassList.empty()) {
                for (unsigned int i = 0; i < numberOfItems; i++) {
                    items += actor->inventory.itemClassList[i] + "#";
                }
            } else {
                items = "none";
            }
            
            std::string genome = AI.genomes.ExtractGenome(actor);
            
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
            
            std::string actorLine = posStrX + "~" + 
                                    posStrY + "~" + 
                                    posStrZ + "~" + 
                                    
                                    age + "~" + 
                                    health + "~" + 
                                    hunger + "~" + 
                                    defense + "~" + 
                                    strength + "~" + 
                                    saturation + "~" + 
                                    
                                    items + "~" + 
                                    genome + "~" + 
                                    memories + '\n';
            
            // Check belongs to this chunk
            if (targetChunkX == (int)chunk->x && targetChunkZ == (int)chunk->y) {
                buffer += actorLine;
                if (doClearActors) 
                    terminationList.push_back(actor);
                actor->isSaved = true;
                saveCounter++;
                continue;
            }
            
            // Check belongs to a loaded chunk
            Chunk* targetChunk = FindChunk(targetChunkX, targetChunkZ);
            if (targetChunk != nullptr) {
                // Skip; targetChunk will handle saving this actor during its own SaveChunk pass
                continue;
            }
            
            // Check belongs to an unloaded chunk
            std::string targetChunkPosStr = Float.ToString((float)targetChunkX) + "_" + Float.ToString((float)targetChunkZ);
            std::string targetChunkFile = worldChunks + targetChunkPosStr;
            
            std::string existingData = "";
            if (Serializer.CheckExists(targetChunkFile)) {
                unsigned int fileSize = Serializer.GetFileSize(targetChunkFile);
                if (fileSize > 0) {
                    existingData.resize(fileSize);
                    Serializer.Deserialize(targetChunkFile, (void*)existingData.data(), fileSize);
                }
            }
            
            existingData += actorLine;
            Serializer.Serialize(targetChunkFile, (void*)existingData.data(), existingData.size());
            
            // Always terminate orphaned actors that step out of loaded memory boundaries
            terminationList.push_back(actor);
            actor->isSaved = true;
            saveCounter++;
        }
        
        // Despawn saved actors marked for removal
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
    
    // Save pickup items
    if (!chunk->pickups.empty()) {
        std::string itemBuffer = "";
        for (const StaticPickup& pickup : chunk->pickups) {
            itemBuffer += Float.ToString(pickup.position.x) + "~" +
                        Float.ToString(pickup.position.y) + "~" +
                        Float.ToString(pickup.position.z) + "~" +
                        pickup.classification + "\n";
        }
        
        Serializer.Serialize(itemsName, (void*)itemBuffer.data(), itemBuffer.size());
    } else {
        // If no pickups remain in this chunk, remove the saved file from disk
        if (Serializer.CheckExists(itemsName)) {
            fs.FileDelete(itemsName);
        }
    }
    return true;
}
