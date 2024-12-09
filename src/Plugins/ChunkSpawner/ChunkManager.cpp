#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

ChunkManager::ChunkManager() : 
    
    renderDistance(8),
    
    chunkSize(32),
    
    worldSeed(100),
    
    ChunkCounterX(0),
    ChunkCounterZ(0)
{
    
    return;
}

void ChunkManager::Initiate(void) {
    
    // Source meshes for world construction
    
    glm::vec3 normalUp(0, 1, 0);
    
    Engine.meshes.wallHorizontal->SetNormals(normalUp);
    Engine.meshes.wallVertical->SetNormals(normalUp);
    
    Engine.meshes.grassHorz->SetNormals(normalUp);
    Engine.meshes.grassVert->SetNormals(normalUp);
    
    Engine.meshes.stemHorz->SetNormals(normalUp);
    Engine.meshes.stemVert->SetNormals(normalUp);
    
    Engine.meshes.log->GetSubMesh(2, subMeshTree);
    
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical  ->GetSubMesh(0, subMeshWallVert);
    
    Engine.meshes.grassHorz->GetSubMesh(0, subMeshGrassHorz);
    Engine.meshes.grassVert->GetSubMesh(0, subMeshGrassVert);
    
    Engine.meshes.stemHorz->GetSubMesh(0, subMeshStemHorz);
    Engine.meshes.stemVert->GetSubMesh(0, subMeshStemVert);
    
    watermesh     = Engine.Create<Mesh>();
    watermaterial = Engine.Create<Material>();
    
    watermesh->isShared     = true;
    watermaterial->isShared = true;
    
    return;
}


Chunk ChunkManager::CreateChunk(float x, float y) {
    Chunk chunk;
    
    chunk.x = x;
    chunk.y = y;
    
    chunk.gameObject = Engine.Create<GameObject>();
    chunk.staticObject = Engine.Create<GameObject>();
    
    chunk.gameObject->renderDistance   = renderDistance * chunkSize * 1.2f;
    chunk.staticObject->renderDistance = renderDistance * chunkSize * 0.8f;
    
    // Add renderers
    chunk.gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    chunk.staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->AddMeshRendererToSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    // Chunk renderer
    
    Transform* chunkTransform = chunk.gameObject->GetComponent<Transform>();
    
    chunkTransform->position = glm::vec3( x, 0, y);
    chunkTransform->scale = glm::vec3( 1, 1, 1 );
    
    chunkRenderer->mesh = Engine.Create<Mesh>();
    chunkRenderer->mesh->isShared = false;
    chunkRenderer->EnableFrustumCulling();
    
    chunkRenderer->material = Engine.Create<Material>();
    chunkRenderer->material->isShared = false;
    
    chunkRenderer->material->diffuse = Colors.gray;
    chunkRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
    
    chunkRenderer->material->shader = Engine.shaders.color;
    
    // Static renderer
    
    Transform* staticTransform = chunk.staticObject->GetComponent<Transform>();
    
    staticTransform->position = glm::vec3( x, 0, y);
    staticTransform->scale = glm::vec3( 1, 1, 1 );
    
    staticRenderer->mesh = Engine.Create<Mesh>();
    staticRenderer->mesh->isShared = false;
    staticRenderer->EnableFrustumCulling();
    
    staticRenderer->material = Engine.Create<Material>();
    staticRenderer->material->isShared = false;
    staticRenderer->material->DisableCulling();
    
    staticRenderer->material->diffuse = Colors.gray;
    staticRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
    
    staticRenderer->material->shader = Engine.shaders.color;
    
    
    // Generate perlin
    
    float heightField [ (chunkSize+1) * (chunkSize+1) ];
    glm::vec3 colorField  [ (chunkSize+1) * (chunkSize+1) ];
    
    Engine.SetHeightFieldValues(heightField, chunkSize+1, chunkSize+1, 0);
    Engine.SetColorFieldValues(colorField, chunkSize+1, chunkSize+1, Colors.white);
    
    unsigned int numberOfLayers = perlin.size();
    
    float min=0.0;
    
    for (unsigned int l=0; l < numberOfLayers; l++) {
        
        Perlin* perlinLayer = &perlin[l];
        
        min = 
        Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize+1, chunkSize+1, 
                                            perlinLayer->noiseWidth, 
                                            perlinLayer->noiseHeight, 
                                            perlinLayer->heightMultuplier, 
                                            x, y, worldSeed);
        
        continue;
    }
    
    // Generate water below water level
    if (min < (world.waterLevel - 32)) {
        
        chunk.waterObject = Engine.Create<GameObject>();
        
        chunk.waterObject->renderDistance = renderDistance * chunkSize * 0.9;
        
        chunk.waterObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        MeshRenderer* waterRenderer = chunk.waterObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
        
        // Water renderer
        Transform* waterTransform = chunk.waterObject->GetComponent<Transform>();
        
        waterTransform->position = glm::vec3( x, world.waterLevel, y);
        waterTransform->scale = glm::vec3( 32, 1, 32 );
        
        waterRenderer->mesh = Engine.meshes.plain;
        waterRenderer->EnableFrustumCulling();
        
        waterRenderer->material = Engine.Create<Material>();
        waterRenderer->material->isShared = false;
        waterRenderer->material->DisableCulling();
        waterRenderer->material->EnableBlending();
        
        waterRenderer->material->diffuse = Colors.blue * Colors.MakeGrayScale(0.4f);
        
        waterRenderer->material->shader = Engine.shaders.water;
        
    }
    
    // Generate terrain color
    
    Color colorLow;
    Color colorHigh;
    
    colorLow  = Colors.brown * Colors.green * Colors.MakeGrayScale(0.4f);
    colorHigh = Colors.brown * Colors.MakeGrayScale(0.2f);
    
    Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize+1, chunkSize+1, colorLow, colorHigh, 0.024f);
    
    Engine.GenerateWaterTableFromHeightField(heightField, chunkSize+1, chunkSize+1, 0);
    
    // Finalize chunk
    
    Engine.AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSize+1, chunkSize+1, 0, 0, 1, 1);
    
    chunkRenderer->mesh->Load();
    
    // Physics
    
    chunk.rigidBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
    
    chunk.rigidBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setType(rp3d::BodyType::STATIC);
    
    rp3d::Transform bodyTransform = rp3d::Transform::identity();
    bodyTransform.setPosition( rp3d::Vector3(x, 0, y) );
    chunk.rigidBody->setTransform(bodyTransform);
    
    // Generate a height field collider
    
    MeshCollider*  meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSize+1, chunkSize+1);
    
    rp3d::Collider* bodyCollider = chunk.rigidBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
    bodyCollider->setUserData( (void*)chunk.gameObject );
    bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
    bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
    
    chunk.bodyCollider = bodyCollider;
    chunk.meshCollider = meshCollider;
    
    // Decorate chunk
    
    Decorate(chunk, x, y, staticRenderer->mesh);
    
    staticRenderer->mesh->Load();
    
    return chunk;
}

bool ChunkManager::DestroyChunk(Chunk& chunk) {
    
    Engine.Destroy<GameObject>( chunk.gameObject );
    Engine.Destroy<GameObject>( chunk.staticObject );
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    // Destroy water if it was generated
    if (chunk.waterObject != nullptr) {
        
        Engine.Destroy<GameObject>( chunk.waterObject );
        
        MeshRenderer* waterRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
    }
    
    Physics.DestroyRigidBody( chunk.rigidBody );
    Physics.DestroyHeightFieldMap(chunk.meshCollider);
    
    // Wrangle any associated actors for saving
    
    for (unsigned int i=0; i < actors.size(); i++) {
        
        GameObject* actorObject = actors[i];
        glm::vec3 actorPos = actorObject->GetPosition();
        glm::vec3 chunkPos = glm::vec3(chunk.x, 0, chunk.y);
        
        actorPos.y = 0;
        
        if (glm::distance(actorPos, chunkPos) <= chunkSize * 3.0f) 
            KillActor(actorObject);
        
        // TODO: Send the save data into a threaded saving/loading system
        
        continue;
    }
    
    return true;
}




bool ChunkManager::SaveChunk(Chunk& chunk) {
    
    std::string chunkPos = Int.ToString( chunk.x ) + "_" + Int.ToString( chunk.y );
    std::string worldChunks = "worlds/" + world.name + "/chunks";
    
    std::string chunkName = worldChunks + "/" + chunkPos;
    
    std::string buffer = "";
    
    unsigned int numberOfActors = actors.size();
    
    for (unsigned int a=0; a < numberOfActors; a++) {
        
        GameObject* actorObject = actors[a];
        
        glm::vec3 actorPos = actorObject->GetPosition();
        
        // Check actor within chunk bounds
        int chunkArea = chunkSize * 2;
        
        if ( ((actorPos.x > (chunk.x + chunkArea)) | (actorPos.x < (chunk.x - chunkArea))) | 
             ((actorPos.z > (chunk.y + chunkArea)) | (actorPos.z < (chunk.y - chunkArea))) ) {
            continue;
        }
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        std::string actorPosStr = Float.ToString(actorPos.x) + "~" + 
                                  Float.ToString(actorPos.y) + "~" + 
                                  Float.ToString(actorPos.z) + "~";
        
        std::string actorGenome = AI.genomes.ExtractGenome(actorPtr);
        
        buffer += actorPosStr + actorGenome + '\n';
        
        KillActor( actorObject );
        
        continue;
    }
    
    unsigned int bufferSz = buffer.size();
    
    Serializer.Serialize(chunkName, (void*)buffer.data(), bufferSz);
    
    return 0;
}

bool ChunkManager::LoadChunk(Chunk& chunk) {
    
    
    
    
    
    return 0;
}



bool ChunkManager::SaveWorld(void) {
    
    std::string worldName   = "worlds/" + world.name;
    std::string worldChunks = "worlds/" + world.name + "/chunks";
    
    // Check world directory structure exists
    if (!Directory.CheckExists(worldName)) 
        Directory.Create(worldName);
    if (!Directory.CheckExists(worldChunks)) 
        Directory.Create(worldChunks);
    
    // Save world chunks
    
    unsigned int numberOfChunks = chunks.size();
    
    for (unsigned int c=0; c < numberOfChunks; c++) {
        
        Chunk chunkPtr = chunks[c];
        
        SaveChunk( chunkPtr );
        
        continue;
    }
    
    return 1;
}

bool ChunkManager::LoadWorld(void) {
    
    
    
    
    
    return 0;
}

void ChunkManager::ClearWorld(void) {
    
    for (unsigned int i=0; i < chunks.size(); i++) 
        DestroyChunk( chunks[i] );
    
    for (unsigned int a=0; a < actors.size(); a++) 
        actors[a]->Deactivate();
    
    chunks.clear();
    
    ChunkCounterX = 0;
    ChunkCounterZ = 0;
    
    return;
}


GameObject* ChunkManager::SpawnActor(float x, float y, float z) {
    
    GameObject* actorObject = nullptr;
    
    for (unsigned int a=0; a < actors.size(); a++) {
        
        if (actors[a]->isActive) 
            continue;
        
        actorObject = actors[a];
        actorObject->SetPosition(x, y, z);
        
        actorObject->Activate();
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        actorPtr->SetTargetPoint(glm::vec3(x, y, z));
        
        break;
    }
    
    if (actorObject == nullptr) {
        
        actorObject = Engine.CreateAIActor( glm::vec3(x, y, z) );
        
        actors.push_back( actorObject );
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        actorPtr->SetTargetPoint(glm::vec3(x, y, z));
        
    }
    
    return actorObject;
}

bool ChunkManager::KillActor(GameObject* actorObject) {
    
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    actorObject->Deactivate();
    
    actorPtr->ClearGenome();
    
    if (actorPtr == nullptr) 
        return false;
    
    actorPtr->SetName("");
    actorPtr->SetChanceToChangeDirection(0);
    actorPtr->SetChanceToFocusOnActor(0);
    actorPtr->SetChanceToStopWalking(0);
    actorPtr->SetChanceToWalk(0);
    actorPtr->SetTargetPoint(glm::vec3(0, 0, 0));
    
    actorPtr->SetSpeed(0);
    actorPtr->SetSpeedMultiplier(0);
    
    actorPtr->SetYouthScale(0);
    actorPtr->SetAdultScale(0);
    
    actorPtr->ClearGenome();
    actorPtr->ClearMemories();
    actorPtr->ClearWeightedLayers();
    
    for (unsigned int a=0; a < actors.size(); a++) {
        
        if (actors[a] == actorObject) {
            
            actors.erase( actors.begin() + a );
            
            break;
        }
        
    }
    
    return true;
}
    
