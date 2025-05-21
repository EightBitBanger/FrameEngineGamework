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
                
                // Position
                float posX = String.ToFloat( lineArray[0] );
                float posY = String.ToFloat( lineArray[1] );
                float posZ = String.ToFloat( lineArray[2] );
                
                glm::vec3 pos(posX, posY, posZ);
                
                // Set current age
                unsigned long long int age = String.ToLongUint( lineArray[3] );
                
                Actor* actor = AI.CreateActor();
                actor->navigation.SetPosition(pos);
                actor->navigation.SetTargetPoint(pos);
                
                // Set genome
                AI.genomes.InjectGenome(actor, lineArray[4]);
                actor->RebuildGeneticExpression();
                
                actor->physical.SetAge(age);
                actor->isActive = true;
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
            
            float colR = staticElements[i].color.r;
            float colG = staticElements[i].color.g;
            float colB = staticElements[i].color.b;
            
            uint8_t type = staticElements[i].type;
            
            StaticObject staticObj;
            
            staticObj.x = posX;
            staticObj.y = posY;
            staticObj.z = posZ;
            
            staticObj.r = colR;
            staticObj.g = colG;
            staticObj.b = colB;
            
            staticObj.type = type;
            
            
            if (type == DECORATION_GRASS) {
                
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
            
            if (type == DECORATION_GRASS_THICK) {
                
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
            
            if (type == DECORATION_GRASS_THIN) {
                
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
            
            if (type == DECORATION_TREE) {
                
                chunk.statics.push_back(staticObj);
                
                staticMesh->AddSubMesh(-posX, posY, -posZ, subMeshTree, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color color;
                color = Color(colR, colG, colB);
                
                staticMesh->ChangeSubMeshColor(index, color);
                
                continue;
            }
            
            
            if (type == DECORATION_LEAVES) {
                
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
            
        }
        
        staticMesh->Load();
        
    }
    
    return 1;
}

