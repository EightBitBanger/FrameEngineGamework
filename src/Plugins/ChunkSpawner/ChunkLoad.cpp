#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadChunk(Chunk* chunk) {
    std::string chunkPosStr = Float.ToString( chunk->x ) + "_" + Float.ToString( chunk->y );
    std::string worldChunks = "worlds\\" + world.name + "\\chunks\\";
    std::string worldStatic = "worlds\\" + world.name + "\\static\\";
    std::string worldItems  = "worlds\\" + world.name + "\\items\\";
    
    std::string chunkName  = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    std::string itemsName  = worldItems + chunkPosStr;
    
    // Load actors
    if (Serializer.CheckExists( chunkName )) {
        unsigned int fileSize = Serializer.GetFileSize(chunkName);
        if (fileSize != 0) {
            std::string dataBuffer;
            dataBuffer.resize(fileSize);
            Serializer.Deserialize(chunkName, (void*)dataBuffer.data(), fileSize);
            std::vector<std::string> bufferArray = String.Explode(dataBuffer, '\n');
            
            unsigned int numberOfLines = bufferArray.size();
            for (unsigned int i = 0; i < numberOfLines; i++) {
                std::string lineString = bufferArray[i];
                std::vector<std::string> lineArray = String.Explode(lineString, '~');
                if (lineArray.size() < 17) 
                    continue;
                
                // Position
                float posX = String.ToFloat( lineArray[0] );
                float posY = String.ToFloat( lineArray[1] );
                float posZ = String.ToFloat( lineArray[2] );
                glm::vec3 pos(posX, posY, posZ);
                
                Actor* actor = SummonActor(pos);
                
                // Set current age
                unsigned long long int age = String.ToLongUint( lineArray[3] );
                actor->physical.SetAge(age);
                
                actor->biological.health     = String.ToFloat( lineArray[4] );
                actor->biological.hunger     = String.ToFloat( lineArray[5] );
                actor->biological.defense    = String.ToFloat( lineArray[6] );
                actor->biological.strength   = String.ToFloat( lineArray[7] );
                actor->biological.saturation = String.ToFloat( lineArray[8] );
                
                actor->sleep.SetCurrentSleepStart( String.ToFloat( lineArray[9] ) );
                actor->sleep.SetCurrentDuration( String.ToFloat( lineArray[10] ) );
                
                actor->counters.SetCoolDownAttack(     String.ToFloat(lineArray[11]) );
                actor->counters.SetCoolDownBreeding(   String.ToFloat(lineArray[12]) );
                actor->counters.SetCoolDownMovement(   String.ToFloat(lineArray[13]) );
                actor->counters.SetCoolDownObservation(String.ToFloat(lineArray[14]) );
                actor->counters.SetCoolDownSocial(     String.ToFloat(lineArray[15]) );
                
                // Load inventory items
                std::string& itemClassList = lineArray[16];
                std::vector<std::string> itemList = String.Explode(itemClassList, '#');
                for (unsigned int a = 0; a < itemList.size(); a++) {
                    actor->inventory.GiveItem( itemList[a] );
                }
                
                // Set genome
                AI.genomes.InjectGenome(actor, lineArray[17]);
                actor->RebuildGeneticExpression();
                actor->CalculateBoundingRegionFromGenome();
                
                actor->isActive = true;
                
                // Memories
                std::vector<std::string> memories = String.Explode(lineArray[18], '|');
                for (unsigned int m = 0; m < memories.size(); m++) {
                    const std::string& memory = memories[m];
                    if (!memory.empty()) {
                        std::vector<std::string> pairSplit = String.Explode(memory, '=');
                        if (pairSplit.size() == 2) {
                            actor->memories.Add(pairSplit[0], pairSplit[1]);
                        }
                    }
                }
            }
        }
    }
    
    // Load static objects
    if (Serializer.CheckExists( staticName )) {
        MeshRenderer* meshRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
        Mesh* staticMesh = meshRenderer->mesh;
        
        unsigned int fileSize = Serializer.GetFileSize(staticName);
        unsigned int numberOfStaticElements = fileSize / sizeof(StaticElement);
        if (numberOfStaticElements > 0) {
            std::vector<StaticElement> staticElements(numberOfStaticElements);
            Serializer.Deserialize(staticName, (void*)staticElements.data(), fileSize);
            
            for (unsigned int i = 0; i < numberOfStaticElements; i++) {
                glm::vec3 position = staticElements[i].position;
                glm::vec3 rotation = staticElements[i].rotation;
                glm::vec3 scale    = staticElements[i].scale;
                glm::vec3 color    = staticElements[i].color;
                int function       = staticElements[i].function;
                
                AddDecor(chunk, 
                         mStaticIndexToMesh[staticElements[i].mesh], 
                         world.classIndexToName[staticElements[i].type], 
                         position, 
                         rotation, 
                         scale, 
                         color, 
                         function);
            }
        }
        
        staticMesh->Load();
    }
    
    // Load item pickups
    if (Serializer.CheckExists(itemsName)) {
        unsigned int fileSize = Serializer.GetFileSize(itemsName);
        if (fileSize > 0) {
            std::string dataBuffer;
            dataBuffer.resize(fileSize);
            Serializer.Deserialize(itemsName, (void*)dataBuffer.data(), fileSize);
            
            std::vector<std::string> lines = String.Explode(dataBuffer, '\n');
            for (const std::string& line : lines) {
                std::vector<std::string> parts = String.Explode(line, '~');
                if (parts.size() < 7) 
                    continue;
                
                glm::vec3 localPos(
                    String.ToFloat(parts[0]),
                    String.ToFloat(parts[1]),
                    String.ToFloat(parts[2])
                );
                glm::vec3 localRot(
                    String.ToFloat(parts[3]),
                    String.ToFloat(parts[4]),
                    String.ToFloat(parts[5])
                );
                std::string itemClassification = parts[6];
                
                StaticPickup pickup;
                pickup.position       = localPos;
                pickup.rotation       = localRot;
                pickup.scale          = glm::vec3(0.5f);
                pickup.classification = itemClassification;
                pickup.renderer       = nullptr;
                
                chunk->pickups.push_back(pickup);
            }
            
            RebuildPickupMesh(chunk);
        }
    }
    return true;
}
