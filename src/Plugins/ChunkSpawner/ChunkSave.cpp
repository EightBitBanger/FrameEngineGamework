#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SaveChunk(Chunk& chunk, bool doClearActors) {
    
    std::string chunkPosStr = Float.ToString( chunk.x ) + "_" + Float.ToString( chunk.y );
    std::string worldChunks = "worlds/" + world.name + "/chunks/";
    std::string worldStatic = "worlds/" + world.name + "/static/";
    
    std::string chunkName = worldChunks + chunkPosStr;
    std::string staticName = worldStatic + chunkPosStr;
    
    std::string buffer = "";
    
    
    // Save actors within chunk range
    
    unsigned int numberOfActors = actors.size();
    if (numberOfActors > 0) {
        for (unsigned int a=0; a < numberOfActors; a++) {
            
            GameObject* actorObject = actors[a];
            
            glm::vec3 actorPos = actorObject->GetPosition();
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            
            float chunkSz = chunkSize * 0.5f;
            
            Actor* actorPtr = actorObject->GetComponent<Actor>();
            
            // Check actor within chunk bounds
            if (((actorPos.x < (chunkPos.x - chunkSz)) | actorPos.x > (chunkPos.x + chunkSz)) | 
                ((actorPos.z < (chunkPos.z - chunkSz)) | actorPos.z > (chunkPos.z + chunkSz)))
                continue;
            
            // Check mark as saved
            if (actorPtr->GetUserBitmask() == 'S') 
                continue;
            
            actorPtr->SetUserBitmask('S');
            
            if (actorPtr->GetName() == "") 
                continue;
            
            if (!actorPtr->GetActive()) 
                continue;
            
            std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                    Float.ToString(actorPos.y) + "~" + 
                                    Float.ToString(actorPos.z) + "~";
            
            std::string actorAge = IntLong.ToString( actorPtr->GetAge() ) + "~";
            
            std::string actorGenome = AI.genomes.ExtractGenome(actorPtr);
            
            buffer += actorPosStr + actorAge + actorGenome + '\n';
            
            if (!doClearActors) 
                continue;
            
            actorPtr->SetUserBitmask(0);
            
            KillActor( actorObject );
            
            continue;
        }
        
    }
    
    unsigned int bufferSz = buffer.size();
    
    if (bufferSz != 0) 
        Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
    
    // Save static objects
    
    buffer = "";
    
    unsigned int numberOfStatics = chunk.statics.size();
    
    if (numberOfStatics > 0) {
        
        for (unsigned int s=0; s < numberOfStatics; s++) {
            
            std::string staticXpos = Float.ToString(chunk.statics[s].x);
            std::string staticYpos = Float.ToString(chunk.statics[s].y);
            std::string staticZpos = Float.ToString(chunk.statics[s].z);
            
            std::string staticRcol = Float.ToString(chunk.statics[s].r);
            std::string staticGcol = Float.ToString(chunk.statics[s].g);
            std::string staticBcol = Float.ToString(chunk.statics[s].b);
            
            std::string staticType;
            
            switch (chunk.statics[s].type) {
                
                case DECORATION_GRASS:         staticType = "grass"; break;
                case DECORATION_GRASS_THICK:   staticType = "grass_thick"; break;
                case DECORATION_GRASS_THIN:    staticType = "grass_thin"; break;
                case DECORATION_TREE:          staticType = "tree"; break;
                case DECORATION_LEAVES:        staticType = "leaves"; break;
                
                default: staticType = "undefined"; continue;
            }
            
            buffer += staticType + "~" + staticXpos + "~" + staticYpos + "~" + staticZpos + "~";
            buffer += staticRcol + "~" + staticGcol + "~" + staticBcol + '\n';
            
            continue;
        }
        
    }
    
    bufferSz = buffer.size();
    
    if (bufferSz != 0) 
        Serializer.Serialize(staticName, (void*)buffer.data(), bufferSz);
    
    return 0;
}
