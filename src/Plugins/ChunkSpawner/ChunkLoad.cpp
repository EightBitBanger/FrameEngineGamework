#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadChunk(Chunk* chunk) {
    std::string chunkPosStr = Float.ToString( chunk->x ) + "_" + Float.ToString( chunk->y );
    std::string worldChunks = "worlds\\" + world.name + "\\chunks\\";
    std::string worldStatic = "worlds\\" + world.name + "\\static\\";
    
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    // Load actors
    if (Serializer.CheckExists( chunkName )) {
        unsigned int fileSize = Serializer.GetFileSize(chunkName);
        if (fileSize != 0) {
            std::string dataBuffer;
            dataBuffer.resize(fileSize);
            
            Serializer.Deserialize(chunkName, (void*)dataBuffer.data(), fileSize);
            std::vector<std::string> bufferArray = String.Explode(dataBuffer, '\n');
            
            unsigned int numberOfLines = bufferArray.size();
            for (unsigned int i=0; i < numberOfLines; i++) {
                std::string lineString = bufferArray[i];
                std::vector<std::string> lineArray = String.Explode(lineString, '~');
                
                // Position
                float posX = String.ToFloat( lineArray[0] );
                float posY = String.ToFloat( lineArray[1] );
                float posZ = String.ToFloat( lineArray[2] );
                
                glm::vec3 pos(posX, posY, posZ);
                
                // Set current age
                unsigned long long int age = String.ToLongUint( lineArray[3] );
                
                Actor* actor = SummonActor(pos);
                
                // Set genome
                AI.genomes.InjectGenome(actor, lineArray[4]);
                actor->RebuildGeneticExpression();
                
                actor->physical.SetAge(age);
                actor->isActive = true;
                
                actor->physical.UpdatePhysicalCollider();
                
                // Memories
                std::vector<std::string> memorySplit = String.Explode(lineArray[5], '=');
                
                if (memorySplit.size() == 2) 
                    actor->memories.Add(memorySplit[0], memorySplit[1]);
                
                continue;
            }
            
        }
        
    }
    
    
    // Load static
    
    if (Serializer.CheckExists( staticName )) {
        MeshRenderer* meshRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
        Mesh* staticMesh = meshRenderer->mesh;
        
        unsigned int fileSize = Serializer.GetFileSize(staticName);
        unsigned int numberOfStaticElements = fileSize / sizeof(StaticElement);
        
        StaticElement staticElements[numberOfStaticElements];
        Serializer.Deserialize(staticName, (void*)staticElements, fileSize);
        
        unsigned int currentIndex = sizeof(unsigned int)-1;
        
        for (unsigned int i=0; i < numberOfStaticElements; i++) {
            glm::vec3 position = staticElements[i].position;
            glm::vec3 rotation = staticElements[i].rotation;
            glm::vec3 scale    = staticElements[i].scale;
            glm::vec3 color    = staticElements[i].color;
            
            AddDecor(chunk, mStaticIndexToMesh[staticElements[i].mesh], world.classIndexToName[staticElements[i].type], position, rotation);
        }
        
        staticMesh->Load();
    }
    
    return 1;
}

