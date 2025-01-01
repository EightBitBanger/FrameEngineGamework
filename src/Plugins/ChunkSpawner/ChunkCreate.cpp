#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk ChunkManager::CreateChunk(float x, float y) {
    
    Chunk chunk;
    
    chunk.isActive = true;
    
    chunk.x = x;
    chunk.y = y;
    
    chunk.gameObject = Engine.Create<GameObject>();
    chunk.staticObject = Engine.Create<GameObject>();
    
    chunk.gameObject->name = Float.ToString(x) + "_" + Float.ToString(y);
    
    chunk.gameObject->renderDistance   = renderDistance * chunkSize * 0.5f;
    chunk.staticObject->renderDistance = staticDistance * chunkSize * 0.5f;
    
    // Add renderers
    chunk.gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    chunk.staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    // Bounding box area
    glm::vec3 boundMin(-chunkSize, -100, -chunkSize);
    glm::vec3 boundMax(chunkSize, 100, chunkSize);
    
    chunkRenderer->SetBoundingBoxMin(boundMin);
    chunkRenderer->SetBoundingBoxMax(boundMax);
    
    staticRenderer->SetBoundingBoxMin(boundMin);
    staticRenderer->SetBoundingBoxMax(boundMax);
    
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->AddMeshRendererToSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    // Chunk renderer
    
    Transform* chunkTransform = chunk.gameObject->GetComponent<Transform>();
    
    chunkTransform->position = glm::vec3( x, 0, y);
    chunkTransform->scale = glm::vec3( 1, 1, 1 );
    
    chunkRenderer->mesh = Engine.Create<Mesh>();
    chunkRenderer->mesh->isShared = false;
    chunkRenderer->EnableFrustumCulling();
    
    chunkRenderer->material = worldMaterial;
    
    // Static renderer
    
    Transform* staticTransform = chunk.staticObject->GetComponent<Transform>();
    
    staticTransform->position = glm::vec3( x, 0, y);
    staticTransform->scale = glm::vec3( 1, 1, 1 );
    
    staticRenderer->mesh = Engine.Create<Mesh>();
    staticRenderer->mesh->isShared = false;
    staticRenderer->EnableFrustumCulling();
    
    staticRenderer->material = staticMaterial;
    
    
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
        
        /*
        
        chunk.waterObject = Engine.Create<GameObject>();
        
        chunk.waterObject->renderDistance = renderDistance * chunkSize * 0.99;
        
        chunk.waterObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        MeshRenderer* waterRenderer = chunk.waterObject->GetComponent<MeshRenderer>();
        
        //Engine.sceneMain->AddMeshRendererToSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
        
        // Water renderer
        Transform* waterTransform = chunk.waterObject->GetComponent<Transform>();
        
        waterTransform->position = glm::vec3( x, world.waterLevel, y);
        waterTransform->scale = glm::vec3( chunkSize, 1, chunkSize );
        
        waterRenderer->mesh = Engine.meshes.plain;
        waterRenderer->EnableFrustumCulling();
        
        waterRenderer->material = waterMaterial;
        
        */
    }
    
    // Generate terrain color
    
    Color colorLow;
    Color colorHigh;
    
    colorLow  = Colors.brown * Colors.green * Colors.MakeGrayScale(0.4f);
    colorHigh = Colors.brown * Colors.MakeGrayScale(0.2f);
    
    Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize+1, chunkSize+1, colorLow, colorHigh, 0.024f);
    
    Engine.AddColorFieldSnowCap(colorField, heightField, chunkSize+1, chunkSize+1, Colors.white, 80.0f, 2.0f);
    
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
    
    return chunk;
}

