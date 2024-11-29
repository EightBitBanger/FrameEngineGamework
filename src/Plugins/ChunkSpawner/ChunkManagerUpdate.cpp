#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

void ChunkManager::Update(void) {
    
    if (Engine.cameraController == nullptr) 
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    playerPosition.y = 0;
    
    //
    // Destroy chunks
    
    for (unsigned int c=0; c < chunkList.size(); c++) {
        
        Chunk& chunkPtr = chunkList[ c ];
        
        glm::vec3 chunkPos = glm::vec3(chunkPtr.x, 0, chunkPtr.y);
        
        if (glm::distance(chunkPos, playerPosition) < (renderDistance * chunkSize) * 1.024f ) 
            continue;
        
        DestroyChunk( chunkPtr );
        
        chunkList.erase( chunkList.begin() + c );
        
        continue;
    }
    
    
    //
    // Generate chunks
    
    for (ChunkCounterX=0; ChunkCounterX <= renderDistance; ChunkCounterX++) {
        
        if (ChunkCounterX >= renderDistance) {
            
            ChunkCounterX = 0;
            break;
        }
        
        int xx = ChunkCounterX;
        
        for (ChunkCounterZ=0; ChunkCounterZ <= renderDistance; ChunkCounterZ++) {
            
            if (ChunkCounterZ >= renderDistance) {
                ChunkCounterZ = 0;
                break;
            }
            
            int zz = ChunkCounterZ;
            
            float chunkX = Math.Round( playerPosition.x / chunkSize + xx);
            float chunkZ = Math.Round( playerPosition.z / chunkSize + zz);
            
            glm::vec2 chunkPosition;
            chunkPosition.x = (chunkX * chunkSize) - (renderDistance * (chunkSize / 2));
            chunkPosition.y = (chunkZ * chunkSize) - (renderDistance * (chunkSize / 2));
            
            bool chunkFound = false;
            
            for (unsigned int c=0; c < chunkList.size(); c++) {
                
                Chunk chunkPtr = chunkList[c];
                
                if (glm::vec3(chunkPtr.x, 0, chunkPtr.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y)) {
                    
                    chunkFound = true;
                    
                    break;
                }
                
                
            }
            
            if (chunkFound) 
                continue;
            
            if (glm::distance(glm::vec3(chunkPosition.x, 0, chunkPosition.y), playerPosition) > (renderDistance * (chunkSize / 2))) 
                continue;
            
            
            Chunk chunk;
            
            chunk.gameObject = Engine.Create<GameObject>();
            chunk.staticObject = Engine.Create<GameObject>();
            
            chunk.gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            chunk.staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            
            MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
            MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
            
            Transform* chunkTransform = chunk.gameObject->GetComponent<Transform>();
            Transform* staticTransform = chunk.staticObject->GetComponent<Transform>();
            
            chunkTransform->position = glm::vec3( chunkPosition.x, 0, chunkPosition.y);
            chunkTransform->scale = glm::vec3( 1, 1, 1 );
            
            staticTransform->position = glm::vec3( chunkPosition.x, 0, chunkPosition.y);
            staticTransform->scale = glm::vec3( 1, 1, 1 );
            
            
            chunk.x = chunkPosition.x;
            chunk.y = chunkPosition.y;
            
            chunkRenderer->mesh = Engine.Create<Mesh>();
            chunkRenderer->mesh->isShared = false;
            chunkRenderer->EnableFrustumCulling();
            
            staticRenderer->mesh = Engine.Create<Mesh>();
            staticRenderer->mesh->isShared = false;
            staticRenderer->EnableFrustumCulling();
            
            
            // Generate chunk from perlin
            
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
                                                    chunkPosition.x, chunkPosition.y, worldSeed);
                
                continue;
            }
            
            
            // Material
            
            chunkRenderer->material = Engine.Create<Material>();
            chunkRenderer->material->isShared = false;
            
            chunkRenderer->material->diffuse = Colors.gray;
            chunkRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
            
            chunkRenderer->material->shader = Engine.shaders.color;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot( chunkRenderer );
            
            
            // Chunk color
            Color colorLow;
            Color colorHigh;
            
            colorLow  = Colors.brown * Colors.green * Colors.MakeGrayScale(0.4f);
            colorHigh = Colors.brown * Colors.MakeGrayScale(0.2f);
            
            Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize+1, chunkSize+1, colorLow, colorHigh, 0.024f);
            
            Engine.GenerateWaterTableFromHeightField(heightField, chunkSize+1, chunkSize+1, 0);
            
            // Water table
            
            /*
            
            chunk.waterObject = Engine.Create<GameObject>();
            chunk.waterObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            MeshRenderer* waterRenderer = chunk.waterObject->GetComponent<MeshRenderer>();
            Transform* waterTransform = chunk.waterObject->GetComponent<Transform>();
            
            waterTransform->position = glm::vec3( chunkPosition.x, world.waterLevel, chunkPosition.y);
            waterTransform->scale = glm::vec3( 16, 1, 16 );
            
            waterRenderer->mesh = Engine.Create<Mesh>();
            waterRenderer->mesh->isShared = false;
            waterRenderer->EnableFrustumCulling();
            
            float maxWaterLayers  = 40.0f;
            float maxWaterSpacing = (1.0f / 3.0f);
            
            float layerThickness = 0.1f;
            
            for (float c=0.0f; c <= maxWaterLayers; c += maxWaterSpacing) {
                
                Color layerColor = world.waterColorHigh;
                layerColor *= Colors.MakeGrayScale( -(c * (layerThickness * 32)) );
                
                layerColor *= 0.1;
                
                waterRenderer->mesh->AddPlain(0, 
                                            (c - maxWaterLayers) + maxWaterSpacing, 
                                            0, 
                                            1, 1, 
                                            layerColor);
                
            }
            
            waterRenderer->mesh->Load();
            
            
            // Water material
            waterRenderer->material = Engine.Create<Material>();
            waterRenderer->material->isShared = false;
            
            waterRenderer->material->diffuse = Colors.MakeGrayScale(0.01f);
            waterRenderer->material->ambient = Colors.MakeGrayScale(0.01f);
            
            waterRenderer->material->shader = Engine.shaders.water;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
            
            // Water blending
            waterRenderer->material->EnableBlending();
            waterRenderer->material->DisableCulling();
            
            
            */
            
            
            
            
            
            // Physics
            
            rp3d::RigidBody* chunkBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
            
            chunkBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
            chunkBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
            chunkBody->setType(rp3d::BodyType::STATIC);
            
            rp3d::Transform bodyTransform = rp3d::Transform::identity();
            bodyTransform.setPosition( rp3d::Vector3(chunkPosition.x, 0, chunkPosition.y) );
            chunkBody->setTransform(bodyTransform);
            
            // Generate a height field collider
            MeshCollider*  meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSize+1, chunkSize+1);
            
            rp3d::Collider* bodyCollider = chunkBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
            bodyCollider->setUserData( (void*)chunk.gameObject );
            bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
            bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
            
            chunk.rigidBody = chunkBody;
            chunk.bodyCollider = bodyCollider;
            chunk.meshCollider = meshCollider;
            
            
            // Chunk material
            
            chunkRenderer->material = Engine.Create<Material>();
            chunkRenderer->material->isShared = false;
            
            chunkRenderer->material->diffuse = Colors.gray;
            chunkRenderer->material->ambient = Colors.MakeGrayScale(0.1f);
            
            chunkRenderer->material->shader = Engine.shaders.color;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
            
            
            // Static material
            staticRenderer->material = Engine.Create<Material>();
            staticRenderer->material->isShared = false;
            
            staticRenderer->material->diffuse = Colors.gray;
            staticRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
            staticRenderer->material->DisableCulling();
            
            staticRenderer->material->shader = Engine.shaders.color;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
            
            
            // Decorate chunk
            
            Decorate(chunk, chunkPosition.x, chunkPosition.y, staticRenderer->mesh);
            
            staticRenderer->mesh->Load();
            
            
            
            
            Engine.AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSize+1, chunkSize+1, 0, 0, 1, 1);
            
            chunkRenderer->mesh->Load();
            
            chunkList.push_back( chunk );
            
            continue;
        }
        
        continue;
    }
    
    return;
}



void DecodeGenome(Decoration& decor, Actor* actorPtr) {
    if (decor.name == "Sheep")  {AI.genomes.Sheep( actorPtr );}
    if (decor.name == "Bear")   {AI.genomes.Bear( actorPtr );}
    return;
}



void ChunkManager::Decorate(Chunk& chunk, int chunkX, int chunkZ, Mesh* staticMesh) {
    
    if (world.mDecorations.size() == 0) 
        return;
    
    for (int xx=0; xx < chunkSize-1; xx++) {
        
        for (int zz=0; zz < chunkSize-1; zz++) {
            
            int xp = xx - (chunkSize / 2);
            int zp = zz - (chunkSize / 2);
            
            int staticX = (chunkX - xp);
            int staticZ = (chunkZ - zp);
            
            glm::vec3 from(staticX, 0, staticZ);
            glm::vec3 direction(0, -1, 0);
            
            Hit hit;
            
            float distance = 2000;
            float height = 0;
            
            if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) 
                height = hit.point.y;
            
            if (height > world.staticHeightCutoff) 
                continue;
            
            if (height == 0) 
                continue;
            
            unsigned int decorIndex = Random.Range(0, world.mDecorations.size());
            
            Decoration decor = world.mDecorations[ decorIndex ];
            
            // Spawn density
            if (Random.Range(0, 10000) > decor.density) 
                continue;
            
            // World spawn height range
            if ((height > decor.spawnHeightMaximum) | 
                (height < decor.spawnHeightMinimum)) 
                continue;
            
            // Stack height
            unsigned int stackHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
            
            switch (decor.type) {
                
                // Thin grass
                case 0: {
                    
                    for (unsigned int c=0; c < stackHeight; c++) {
                        
                        staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemHorz, false);
                        staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemVert, false);
                        
                        unsigned int index = staticMesh->GetSubMeshCount() - 1;
                        
                        Color finalColor;
                        finalColor = Colors.green * 0.018f;
                        
                        finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                                Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                                Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                        
                        staticMesh->ChangeSubMeshColor(index, finalColor);
                        staticMesh->ChangeSubMeshColor(index-1, finalColor);
                        
                    }
                    
                    continue;
                }
                
                // Grass
                case 1: {
                    
                    for (unsigned int c=0; c < stackHeight; c++) {
                        
                        staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshGrassHorz, false);
                        staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshGrassVert, false);
                        
                        unsigned int index = staticMesh->GetSubMeshCount() - 1;
                        
                        Color finalColor;
                        finalColor = Colors.green * 0.04f;
                        
                        staticMesh->ChangeSubMeshColor(index, finalColor);
                        staticMesh->ChangeSubMeshColor(index-1, finalColor);
                        
                    }
                    
                    continue;
                }
                
                // Thicker grass
                case 2: {
                    
                    staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallHorz, false);
                    staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.05f;
                    
                    if (Random.Range(0, 100) < 20) finalColor = Colors.yellow * 0.05f;
                    if (Random.Range(0, 100) < 20) finalColor = Colors.orange * 0.01f;
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    staticMesh->ChangeSubMeshColor(index-1, finalColor);
                    
                    continue;
                }
                
                // Trees
                case 3: {
                    
                    // Tree logs
                    
                    float logHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                    
                    for (unsigned int s=0; s < logHeight; s++) {
                        
                        staticMesh->AddSubMesh(-xp, height + s, -zp, subMeshTree, false);
                        
                        unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                        
                        unsigned int index = numberOfSubMeshes - 1;
                        
                        Color finalColor;
                        Color lowTrunk;
                        Color highTrunk;
                        
                        lowTrunk  = (Colors.brown * 0.1f) + (Colors.green * 0.01);
                        highTrunk = (Colors.brown * 0.8f) + (Colors.green * 0.01);
                        
                        finalColor = Colors.Lerp(lowTrunk, highTrunk, (s * 0.087f));
                        
                        staticMesh->ChangeSubMeshColor(index, finalColor);
                        
                        staticMesh->ChangeSubMeshPosition(index, -xp, height + s - 1.0f, -zp);
                        
                    }
                    
                    // Leaves
                    
                    float leafCount = Random.Range(((float)world.numberOfLeaves) / 2, (float)world.numberOfLeaves);
                    
                    unsigned int leafAccent = Random.Range(0, 100);
                    
                    for (unsigned int z=0; z < leafCount; z++) {
                        
                        float offset_xx = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                        float offset_yy = Random.Range(0.0f, world.leafSpreadHeight) - Random.Range(0.0f, world.leafSpreadHeight);
                        float offset_zz = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                        
                        staticMesh->AddSubMesh(0, 0, 0, subMeshWallHorz, false);
                        staticMesh->AddSubMesh(0, 0, 0, subMeshWallVert, false);
                        
                        unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                        unsigned int index = numberOfSubMeshes - 1;
                        
                        Color finalColor;
                        Color lowLeaves;
                        Color highLeaves;
                        
                        lowLeaves  = Colors.green * 0.08f;
                        highLeaves = Colors.green * 0.01f;
                        
                        if ((leafAccent > 70) & (leafAccent < 80)) 
                            lowLeaves = Colors.orange * 0.1f;
                        
                        if ((leafAccent > 20) & (leafAccent < 40)) 
                            lowLeaves = Colors.yellow * 0.1f;
                        
                        finalColor = Colors.Lerp(lowLeaves, highLeaves, Random.Range(0, 100) * 0.01f);
                        
                        staticMesh->ChangeSubMeshColor(index, finalColor);
                        staticMesh->ChangeSubMeshColor(index-1, finalColor);
                        
                        staticMesh->ChangeSubMeshPosition(index,   -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                        staticMesh->ChangeSubMeshPosition(index-1, -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                        
                    }
                    
                    
                    continue;
                }
                
                // Actor generation
                case 4: {
                    
                    GameObject* actorObject = Engine.CreateAIActor( glm::vec3(from.x, 0, from.z) );
                    
                    chunk.actorList.push_back( actorObject );
                    
                    Actor* actor = actorObject->GetComponent<Actor>();
                    
                    actor->SetHeightPreferenceMin(world.waterLevel);
                    actor->SetHeightPreferenceMax(40.0f);
                    
                    DecodeGenome(decor, actor);
                    
                    actor->SetAge( 1000 );
                    
                    // Generate youth
                    if (Random.Range(0, 100) > 90) {
                        
                        unsigned int numberOfChildren = Random.Range(1, 3);
                        
                        for (unsigned int c=0; c < numberOfChildren; c++) {
                            
                            float offsetX = (Random.Range(0, 1) - Random.Range(0, 1));
                            float offsetZ = (Random.Range(0, 1) - Random.Range(0, 1));
                            
                            GameObject* youthActorObject = Engine.CreateAIActor( glm::vec3(from.x + offsetX, 
                                                                                           0, 
                                                                                           from.z + offsetZ) );
                            
                            chunk.actorList.push_back( youthActorObject );
                            
                            Actor* youthActor = youthActorObject->GetComponent<Actor>();
                            
                            youthActor->SetHeightPreferenceMin(world.waterLevel);
                            youthActor->SetHeightPreferenceMax(40.0f);
                            
                            DecodeGenome(decor, youthActor);
                            
                            youthActor->SetAge(0);
                        }
                        
                        continue;
                    }
                    
                    continue;
                }
                
                
                default:
                    break;
            }
            
        }
        
    }
    
    return;
}

