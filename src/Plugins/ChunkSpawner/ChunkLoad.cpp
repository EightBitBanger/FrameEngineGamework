#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadChunk(Chunk& chunk) {
    std::string chunkPosStr = Float.ToString( chunk.x ) + "_" + Float.ToString( chunk.y );
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
        MeshRenderer* meshRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
        Mesh* staticMesh = meshRenderer->mesh;
        
        unsigned int fileSize = Serializer.GetFileSize(staticName);
        unsigned int numberOfStaticElements = fileSize / sizeof(StaticElement);
        
        StaticElement staticElements[numberOfStaticElements];
        Serializer.Deserialize(staticName, (void*)staticElements, fileSize);
        
        for (unsigned int i=0; i < numberOfStaticElements; i++) {
            
            float posX = staticElements[i].position.x;
            float posY = staticElements[i].position.y;
            float posZ = staticElements[i].position.z;
            
            float rotY = staticElements[i].rotation.x;
            float rotP = staticElements[i].rotation.y;
            float rotR = staticElements[i].rotation.z;
            
            float scaleX = staticElements[i].scale.x;
            float scaleY = staticElements[i].scale.y;
            float scaleZ = staticElements[i].scale.z;
            
            float colR = staticElements[i].color.r;
            float colG = staticElements[i].color.g;
            float colB = staticElements[i].color.b;
            
            StaticObject staticObj;
            staticObj.position = staticElements[i].position;
            staticObj.rotation = staticElements[i].rotation;
            staticObj.scale    = staticElements[i].scale;
            staticObj.color    = staticElements[i].color;
            chunk.statics.push_back(staticObj);
            
            staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallHorz, false);
            staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallVert, false);
            
            unsigned int index = staticMesh->GetSubMeshCount() - 1;
            
            Color color;
            color = Color(colR, colG, colB);
            
            staticMesh->ChangeSubMeshColor(index,   color);
            staticMesh->ChangeSubMeshColor(index-1, color);
            
            staticMesh->ChangeSubMeshScale(index,   scaleX, scaleY, scaleZ);
            staticMesh->ChangeSubMeshScale(index-1, scaleX, scaleY, scaleZ);
            
            staticMesh->ChangeSubMeshRotation(index,   rotY, glm::vec3(0, 0, 1.0f));
            staticMesh->ChangeSubMeshRotation(index-1, rotY, glm::vec3(0, 0, 1.0f));
            staticMesh->ChangeSubMeshRotation(index,   rotP, glm::vec3(1.0f, 0, 0));
            staticMesh->ChangeSubMeshRotation(index-1, rotP, glm::vec3(1.0f, 0, 0));
            staticMesh->ChangeSubMeshRotation(index,   rotR, glm::vec3(0, 1.0f, 0));
            staticMesh->ChangeSubMeshRotation(index-1, rotR, glm::vec3(0, 1.0f, 0));
            
            
            /*
            switch (type) {
                
            case DECORATION_CUSTOM: {
                chunk.statics.push_back(staticObj);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemHorz, false);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                staticMesh->ChangeSubMeshColor(index-1, color);
                
                float length = 15.0f;
                staticMesh->ChangeSubMeshScale(index,   1.0f, length, 1.0f);
                staticMesh->ChangeSubMeshScale(index-1, 1.0f, length, 1.0f);
                
                staticMesh->ChangeSubMeshRotation(index,   rotY, glm::vec3(0, 0, 1.0f));
                staticMesh->ChangeSubMeshRotation(index-1, rotY, glm::vec3(0, 0, 1.0f));
                staticMesh->ChangeSubMeshRotation(index,   rotP, glm::vec3(1.0f, 0, 0));
                staticMesh->ChangeSubMeshRotation(index-1, rotP, glm::vec3(1.0f, 0, 0));
                
                break;
            }
            
            case DECORATION_GRASS: {
                chunk.statics.push_back(staticObj);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshGrassHorz, false);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshGrassVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                staticMesh->ChangeSubMeshColor(index-1, color);
                break;
            }
            
            case DECORATION_GRASS_THICK:{
                chunk.statics.push_back(staticObj);
                
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallHorz, false);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                staticMesh->ChangeSubMeshColor(index-1, color);
                break;
            }
            
            case DECORATION_GRASS_THIN: {
                chunk.statics.push_back(staticObj);
                
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemHorz, false);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                staticMesh->ChangeSubMeshColor(index-1, color);
                break;
            }
            
            case DECORATION_TREE: {
                chunk.statics.push_back(staticObj);
                
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshTree, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                break;
            }
            
            case DECORATION_LEAVES: {
                chunk.statics.push_back(staticObj);
                
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallHorz, false);
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                staticMesh->ChangeSubMeshColor(index-1, color);
                break;
              }
            }
            */
            
            
        }
        
        staticMesh->Load();
    }
    
    return 1;
}

