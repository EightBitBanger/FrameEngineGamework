#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::LoadChunk(Chunk& chunk) {
    
    std::string chunkPosStr = Float.ToString( chunk.x ) + "_" + Float.ToString( chunk.y );
    std::string worldChunks = "worlds/" + world.name + "/chunks/";
    std::string worldStatic = "worlds/" + world.name + "/static/";
    
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
                
                float posX = String.ToFloat( lineArray[0] );
                float posY = String.ToFloat( lineArray[1] );
                float posZ = String.ToFloat( lineArray[2] );
                
                glm::vec3 pos(posX, posY, posZ);
                
                unsigned long long int age = String.ToLongUint( lineArray[3] );
                
                GameObject* actorObject = SpawnActor(posX, posY, posZ);
                Actor* actorPtr = actorObject->GetComponent<Actor>();
                
                AI.genomes.InjectGenome(actorPtr, lineArray[4]);
                
                actorPtr->SetAge(age);
                
                continue;
            }
            
        }
        
    }
    
    
    // Load static
    
    if (Serializer.CheckExists( staticName )) {
        
        unsigned int fileSize = Serializer.GetFileSize(staticName);
        
        if (fileSize != 0) {
            
            std::string dataBuffer;
            
            dataBuffer.resize(fileSize);
            
            Serializer.Deserialize(staticName, (void*)dataBuffer.data(), fileSize);
            
            std::vector<std::string> bufferArray = String.Explode(dataBuffer, '\n');
            
            unsigned int numberOfLines = bufferArray.size();
            
            MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
            Mesh* staticMesh = staticRenderer->mesh;
            
            for (unsigned int i=0; i < numberOfLines; i++) {
                
                std::string lineString = bufferArray[i];
                
                std::vector<std::string> lineArray = String.Explode(lineString, '~');
                
                float posX = String.ToFloat( lineArray[1] );
                float posY = String.ToFloat( lineArray[2] );
                float posZ = String.ToFloat( lineArray[3] );
                
                float colR = String.ToFloat( lineArray[4] );
                float colG = String.ToFloat( lineArray[5] );
                float colB = String.ToFloat( lineArray[6] );
                
                StaticObject staticObj;
                
                staticObj.x = posX;
                staticObj.y = posY;
                staticObj.z = posZ;
                
                staticObj.r = colR;
                staticObj.g = colG;
                staticObj.b = colB;
                
                if (lineArray[0] == "grass") {
                    
                    staticObj.type = DECORATION_GRASS;
                    chunk.statics.push_back(staticObj);
                    
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshGrassHorz, false);
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshGrassVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color color;
                    color = Color(colR, colG, colB);
                    
                    staticMesh->ChangeSubMeshColor(index, color);
                    staticMesh->ChangeSubMeshColor(index-1, color);
                    
                    continue;
                }
                
                if (lineArray[0] == "grass_thick") {
                    
                    staticObj.type = DECORATION_GRASS_THICK;
                    chunk.statics.push_back(staticObj);
                    
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallHorz, false);
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color color;
                    color = Color(colR, colG, colB);
                    
                    staticMesh->ChangeSubMeshColor(index, color);
                    staticMesh->ChangeSubMeshColor(index-1, color);
                    
                    continue;
                }
                
                if (lineArray[0] == "grass_thin") {
                    
                    staticObj.type = DECORATION_GRASS_THIN;
                    chunk.statics.push_back(staticObj);
                    
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemHorz, false);
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshStemVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color color;
                    color = Color(colR, colG, colB);
                    
                    staticMesh->ChangeSubMeshColor(index, color);
                    staticMesh->ChangeSubMeshColor(index-1, color);
                    
                    continue;
                }
                
                if (lineArray[0] == "tree") {
                    
                    staticObj.type = DECORATION_TREE;
                    chunk.statics.push_back(staticObj);
                    
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshTree, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color color;
                    color = Color(colR, colG, colB);
                    
                    staticMesh->ChangeSubMeshColor(index, color);
                    
                    continue;
                }
                
                
                if (lineArray[0] == "leaves") {
                    
                    staticObj.type = DECORATION_LEAVES;
                    chunk.statics.push_back(staticObj);
                    
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallHorz, false);
                    staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshWallVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color color;
                    color = Color(colR, colG, colB);
                    
                    staticMesh->ChangeSubMeshColor(index, color);
                    staticMesh->ChangeSubMeshColor(index-1, color);
                    
                    continue;
                }
                
                
                
                continue;
            }
            
            staticMesh->Load();
            
        }
        
    }
    
    return 1;
}

