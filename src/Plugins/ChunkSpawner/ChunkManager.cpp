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
    
    // Chunk material
    
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
    
    // Static material
    
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
    
    for (unsigned int l=0; l < numberOfLayers; l++) {
        
        Perlin* perlinLayer = &perlin[l];
        
        Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize+1, chunkSize+1, 
                                            perlinLayer->noiseWidth, 
                                            perlinLayer->noiseHeight, 
                                            perlinLayer->heightMultuplier, 
                                            x, y, worldSeed);
        
        continue;
    }
    
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

bool ChunkManager::DestroyChunk(Chunk& chunkPtr) {
    
    Engine.Destroy<GameObject>( chunkPtr.gameObject );
    Engine.Destroy<GameObject>( chunkPtr.staticObject );
    
    MeshRenderer* chunkRenderer = chunkPtr.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunkPtr.staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    for (unsigned int a=0; a < chunkPtr.actorList.size(); a++) {
        
        GameObject* actorObject = chunkPtr.actorList[a];
        
        Engine.Destroy<GameObject>( actorObject );
    }
    
    chunkPtr.actorList.clear();
    
    Physics.DestroyRigidBody( chunkPtr.rigidBody );
    Physics.DestroyHeightFieldMap(chunkPtr.meshCollider);
    
    return true;
}


bool ChunkManager::RemoveActorFromWorld(GameObject* actorObject) {
    
    unsigned int numberOfChunks = chunkList.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk chunk = chunkList[i];
        
        unsigned int numberOfActors = chunk.actorList.size();
        
        for (unsigned int a=0; a < numberOfActors; a++) {
            
            GameObject* actorListObject = chunk.actorList[a];
            
            if (actorListObject == actorObject) {
                
                chunk.actorList.erase( chunk.actorList.begin() + a );
                
                return 1;
            }
            
            continue;
        }
        
    }
    
    return 0;
}



bool ChunkManager::SaveWorld(std::string worldname) {
    
    
    
    
    
    return 0;
}

bool ChunkManager::LoadWorld(std::string worldname) {
    
    
    
    
    
    return 0;
}

void ChunkManager::ClearWorld(void) {
    
    unsigned int numberOfChunks = chunkList.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk chunkPtr = chunkList[i];
        
        MeshRenderer* chunkRenderer = chunkPtr.gameObject->GetComponent<MeshRenderer>();
        MeshRenderer* staticRenderer = chunkPtr.staticObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
        
        Engine.Destroy<GameObject>( chunkPtr.gameObject );
        Engine.Destroy<GameObject>( chunkPtr.staticObject );
        
        Physics.DestroyRigidBody( chunkPtr.rigidBody );
        Physics.DestroyHeightFieldMap(chunkPtr.meshCollider);
        
        if (chunkPtr.actorList.size() > 0) 
            for (unsigned int a=0; a < chunkPtr.actorList.size(); a++) 
                Engine.Destroy<GameObject>( chunkPtr.actorList[a] );
        
    }
    
    chunkList.clear();
    
    ChunkCounterX = 0;
    ChunkCounterZ = 0;
    
    return;
}






