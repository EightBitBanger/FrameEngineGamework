#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern NumberGeneration     Random;
extern Logger               Log;
extern Timer                PhysicsTime;
extern Timer                Time;

extern EngineSystemManager  Engine;
extern PlatformLayer        Platform;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;


class ENGINE_API WorldGeneration {
    
public:
    
    float snowCapHeight;
    
    Color waterColor;
    
    float waterLevel;
    
    std::vector<Decoration> mDecorations;
    
    
    
    
    // Static plant generation
    
    unsigned int staticDensity;
    float staticHeightCutoff;
    
    // Tree generation
    
    unsigned int treeDensity;
    float treeHeightCutoff;
    float treeHeightLow;
    float treeHeightHigh;
    
    unsigned int numberOfLeaves;
    float leafSpreadArea;
    float leafSpreadHeight;
    float leafHeightOffset;
    
    // Actors
    unsigned int actorDensity;
    float actorHeightCutoff;
    
    
    WorldGeneration() :
        snowCapHeight(60.0f),
        waterColor(Colors.blue),
        waterLevel(-21.0f),
        
        staticDensity(200),
        staticHeightCutoff(50),
        
        treeDensity(40),
        treeHeightCutoff(50),
        treeHeightLow(5),
        treeHeightHigh(8),
        
        numberOfLeaves(15),
        leafSpreadArea(3.0f),
        leafSpreadHeight(1.4f),
        leafHeightOffset(-0.4f),
        
        actorDensity(10),
        
        actorHeightCutoff(20.0f)
        
    {
        
    }
    
};




class ENGINE_API ChunkManager {
    
public:
    
    int chunkSize;
    
    float currentChunkX;
    float currentChunkZ;
    
    float renderDistance;
    float renderDistanceStatic;
    
    unsigned int generateCounter;
    unsigned int purgeCounter;
    
    unsigned int updateChunkCounter;
    
    WorldGeneration world;
    
    
    /// Create a new chunk at the given position.
    Chunk* CreateChunk(float chunkX, float chunkZ);
    
    /// Destroy a chunk freeing any memory therein.
    bool DestroyChunk(Chunk* chunkPtr);
    
    /// Check if the chunk exists at a given position.
    Chunk* CheckChunk(glm::vec2 position);
    
    unsigned int GetNumberOfChunks(void);
    
    /// Prepare the chunk generator.
    void Initiate(void);
    
    /// Check all the chunks in the world.
    void Update(void);
    
    /// Generate new chunks around the player position.
    void GenerateChunks(glm::vec2 position);
    
    /// Destroy old chunks beyond the player render distance.
    void PurgeChunks(glm::vec2 position);
    
    
    /// Add a perlin noise layer to the generator.
    void AddPerlinNoiseLayer(Perlin& layer);
    
    
    /// World generation meshes.
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    
    SubMesh subMeshGrassHorz;
    SubMesh subMeshGrassVert;
    
    SubMesh subMeshStemHorz;
    SubMesh subMeshStemVert;
    
    SubMesh subMeshTree;
    
    /// Water mesh
    Mesh* watermesh;
    Material* watermaterial;
    
    
    ChunkManager() : 
        
        chunkSize(32),
        
        currentChunkX(0),
        currentChunkZ(0),
        
        renderDistance(16),
        renderDistanceStatic(13),
        
        generateCounter(0),
        purgeCounter(0),
        updateChunkCounter(0)
        
    {}
    
private:
    
    // Chunk allocator
    PoolAllocator<Chunk> mChunkList;
    
    std::vector<Chunk*> mActiveChunks;
    std::vector<Perlin> mPerlinLayers;
    
};

Chunk* ChunkManager::CreateChunk(float chunkX, float chunkZ) {
    
    Chunk* newChunk = mChunkList.Create();
    
    newChunk->gameObject = Engine.Create<GameObject>();
    
    newChunk->gameObject->AddComponent( Engine.CreateComponentMeshRenderer( Engine.Create<Mesh>(), Engine.Create<Material>() ) );
    
    MeshRenderer* baseRenderer = newChunk->gameObject->GetComponent<MeshRenderer>();
    
    baseRenderer->mesh->isShared = false;
    
    baseRenderer->material->isShared = false;
    baseRenderer->material->shader = Engine.shaders.color;
    
    baseRenderer->EnableFrustumCulling();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( baseRenderer, RENDER_QUEUE_DEFAULT );
    
    //
    // Static object container
    
    GameObject* staticObjectContainer = Engine.Create<GameObject>();
    staticObjectContainer->renderDistance = (renderDistanceStatic * (chunkSize / 2)) * 0.9f;
    
    Transform* transform = staticObjectContainer->GetComponent<Transform>();
    
    transform->position.x = chunkX;
    transform->position.z = chunkZ;
    
    // Static mesh
    Mesh* staticMesh = Engine.Create<Mesh>();
    
    staticMesh->isShared = false;
    
    // Static material
    Material* staticMaterial = Engine.Create<Material>();
    staticMaterial->DisableCulling();
    
    staticMaterial->shader = Engine.shaders.color;
    
    staticMaterial->ambient = Colors.Make(0.1f, 0.1f, 0.1f);
    staticMaterial->diffuse = Colors.white;
    staticMaterial->specular = Colors.white;
    
    staticObjectContainer->AddComponent( Engine.CreateComponentMeshRenderer(staticMesh, staticMaterial) );
    
    MeshRenderer* staticRenderer = staticObjectContainer->GetComponent<MeshRenderer>();
    staticRenderer->EnableFrustumCulling();
    
    newChunk->staticObjects = staticObjectContainer;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer, RENDER_QUEUE_DEFAULT);
    
    
    
    //
    // Water table generation
    //
    
    newChunk->waterObject = Engine.Create<GameObject>();
    newChunk->waterObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    MeshRenderer* waterRenderer = newChunk->waterObject->GetComponent<MeshRenderer>();
    waterRenderer->EnableFrustumCulling();
    
    Transform* transformWater = newChunk->waterObject->GetComponent<Transform>();
    transformWater->position.y = world.waterLevel;
    
    transformWater->scale = glm::vec3(15.5, 1.0f, 15.5);
    
    float maxWaterLayers  = 16.0f;
    float maxWaterSpacing = (1.0f / 3.0f);
    
    float layerThickness = 0.3f;
    
    // Water layers
    
    waterRenderer->mesh = Engine.Create<Mesh>();
    
    for (float c=0.0f; c <= maxWaterLayers; c += maxWaterSpacing) {
        
        Color layerColor = Colors.MakeGrayScale( -(c * (layerThickness * 16.0f)) );
        
        if (c >= maxWaterLayers) 
            layerColor = Colors.red;
        
        waterRenderer->mesh->AddPlain(0, 
                                      (c - maxWaterLayers) + maxWaterSpacing, 
                                      0, 
                                      1, 1, 
                                      layerColor);
        
    }
    
    //waterRenderer->mesh->AddPlain(0, 0, 0, 1, 1, Colors.white);
    waterRenderer->mesh->Load();
    waterRenderer->mesh->isShared = false;
    
    // Water material
    waterRenderer->material = Engine.Create<Material>();
    waterRenderer->material->shader = Engine.shaders.colorUnlit;
    waterRenderer->material->isShared = false;
    
    // Blending
    waterRenderer->material->EnableBlending();
    waterRenderer->material->DisableCulling();
    
    waterRenderer->material->diffuse = Color(0.000024f, 0.000024f, 0.000024f);
    
    waterRenderer->material->ambient = Colors.blue * 0.005f;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( waterRenderer, RENDER_QUEUE_FOREGROUND);
    
    return newChunk;
}

bool ChunkManager::DestroyChunk(Chunk* chunkPtr) {
    
    MeshRenderer* chunkRenderer = chunkPtr->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* waterRenderer = chunkPtr->waterObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunkPtr->staticObjects->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_DEFAULT );
    Engine.Destroy( chunkPtr->gameObject );
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_DEFAULT );
    Engine.Destroy( chunkPtr->staticObjects );
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( waterRenderer, RENDER_QUEUE_FOREGROUND);
    Engine.Destroy( chunkPtr->waterObject );
    
    Physics.world->destroyRigidBody( chunkPtr->rigidBody );
    
    // Destroy physics collider
    Physics.DestroyHeightFieldMap( chunkPtr->collider );
    
    // Destroy actors
    
    unsigned int numberOfActors = chunkPtr->actorList.size();
    
    for (unsigned int i=0; i < numberOfActors; i++) {
        
        Engine.Destroy( chunkPtr->actorList[i] );
        
    }
    
    chunkPtr->actorList.clear();
    
    return mChunkList.Destroy(chunkPtr);
}

Chunk* ChunkManager::CheckChunk(glm::vec2 position) {
    
    unsigned int numberOfChunks = mChunkList.Size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk* chunk = mChunkList[i];
        
        if (position == chunk->position) 
            return chunk;
        
    }
    
    return nullptr;
}

unsigned int ChunkManager::GetNumberOfChunks(void) {
    
    return mChunkList.Size();
}


void ChunkManager::AddPerlinNoiseLayer(Perlin& layer) {
    
    mPerlinLayers.push_back(layer);
    
    return;
}



void ChunkManager::Update(void) {
    
    glm::vec2 cameraPosition = glm::vec2(Engine.cameraController->GetPosition().x, Engine.cameraController->GetPosition().z);
    
    //
    // Chunk generation
    //
    
    GenerateChunks(cameraPosition);
    
    
    
    //
    // Update world chunks
    unsigned int numberOfChunks = mChunkList.Size();
    updateChunkCounter++;
    
    if (numberOfChunks >= updateChunkCounter) 
        updateChunkCounter=0;
    
    Chunk* chunk = mChunkList[updateChunkCounter];
    
    return;
}









/*

    void Update(void) {
        
        //
        // Update chunks
        //
        
        for (unsigned int i=0; i < 8; i++) {
            
            if (mChunkList.size() == 0) 
                break;
            
            Chunk chunk = mChunkList[chunkIndex];
            
            chunkIndex++;
            
            if (chunkIndex >= mChunkList.size()) 
                chunkIndex = 0;
            
            Transform* transform = chunk.gameObject->GetComponent<Transform>();
            
            glm::vec3 chunkPosition  = transform->position;
            glm::vec3 playerPosition = Engine.sceneMain->camera->transform.position;
            
            // Ignore height
            chunkPosition.y = 0;
            playerPosition.y = 0;
            
            // Check chunk exists
            int index = FindChunk( (int)chunkPosition.x, (int)chunkPosition.z );
            
            if (index == -1) 
                continue;
            
            // Update lighting
            float chanceToUpdateLighting = 2.0f;
            
            MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
            
            // Update chunk lighting
            if (Random.Range(0.0f, 10.0f) > chanceToUpdateLighting) {
                
                chunkRenderer->material->ambient = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.staticColorMul);
                chunkRenderer->material->diffuse = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.staticColorMul);
                
            }
            
            // Update static object lighting
            if (Random.Range(0.0f, 10.0f) > chanceToUpdateLighting) {
                
                MeshRenderer* staticRenderer = chunk.staticObjects->GetComponent<MeshRenderer>();
                staticRenderer->material->ambient = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.staticColorMul);
                staticRenderer->material->diffuse = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.staticColorMul);
                
            }
            
            // Update actor lighting
            unsigned int actorCount = chunk.actorList.size();
            
            if (Random.Range(0.0f, 10.0f) > chanceToUpdateLighting) {
            
                for (unsigned int a=0; a < actorCount; a++) {
                    
                    GameObject* actorObject = chunk.actorList[a];
                    
                    Actor* actor = actorObject->GetComponent<Actor>();
                    
                    unsigned int numberOfRenderers = actor->GetNumberOfMeshRenderers();
                    
                    for (unsigned int z=0; z < numberOfRenderers; z++) {
                        
                        MeshRenderer* actorRenderer = actor->GetMeshRendererAtIndex(z);
                        
                        actorRenderer->material->ambient = Colors.Lerp(world.actorColorLow, world.actorColorHigh, (world.staticColorMul * 0.87f));
                        actorRenderer->material->diffuse = Colors.Lerp(world.actorColorLow, world.actorColorHigh, (world.staticColorMul * 0.24f));
                        
                    }
                    
                }
                
            }
            
            
            // Update level of detail
            if (glm::distance(chunkPosition, playerPosition) < levelOfDetailDistance) {
                
                // High level of detail
                chunkRenderer->mesh = chunk.lodHigh;
                
                chunkRenderer->material->EnableCulling();
                
                transform->scale = glm::vec3(1, 1, 1);
                transform->position.y = 0.0f;
                
            } else {
                
                // Low level of detail
                chunkRenderer->mesh = chunk.lodLow;
                
                chunkRenderer->material->DisableCulling();
                
                transform->scale = glm::vec3(2.0f, 1.1f, 2.0f);
                transform->position.y = -5.0f;
                
            }
            
        }
        
        
        
        


*/

void ChunkManager::GenerateChunks(glm::vec2 position) {
    
    float noiseWidth  = 0.01f;
    float noiseHeight = 0.01f;
    float heightMul   = 70;
    
    Color colorLow;
    Color colorHigh;
    
    colorLow  = Colors.dkgreen * 0.07f;
    colorHigh = Colors.brown   * 0.07f;
    
    float cameraX = Math.Round(position.x / chunkSize);
    float cameraZ = Math.Round(position.y / chunkSize);
    
    float heightField [ chunkSize * chunkSize ];
    glm::vec3 colorField  [ chunkSize * chunkSize ];
    
    unsigned int chunksPerFrame = 3;
    unsigned int chunksFrameCounter = 0;
    
    bool breakout = false;
    
    while (1) {
        
        chunksFrameCounter++;
        if (chunksFrameCounter > chunksPerFrame) {
            
            chunksFrameCounter = 0;
            
            break;
        }
        
        if (currentChunkX >= renderDistance) {
            
            currentChunkX = 0;
            
            if (currentChunkZ >= renderDistance) {
                
                currentChunkZ = 0;
                
                // Keep this here, chunks should only be destroyed
                // after a full cycle of generation.. or else...
                
                PurgeChunks( glm::vec2(Engine.cameraController->GetPosition().x, Engine.cameraController->GetPosition().z) );
                
                break;
                
            } else {
                
                currentChunkZ++;
            }
            
        } else {
            
            currentChunkX++;
        }
        
        int x = currentChunkX;
        int z = currentChunkZ;
        
        float chunkX = ((x + cameraX) * chunkSize) - ((renderDistance / 2) * chunkSize);
        float chunkZ = ((z + cameraZ) * chunkSize) - ((renderDistance / 2) * chunkSize);
        
        int chunkAbsX = (chunkX / chunkSize) * (chunkSize);
        int chunkAbsZ = (chunkZ / chunkSize) * (chunkSize);
        
        int chunkWorldX = (chunkX / chunkSize) * (chunkSize - 1);
        int chunkWorldZ = (chunkZ / chunkSize) * (chunkSize - 1);
        
        Chunk* chunk = CheckChunk(glm::vec2(chunkX, chunkZ));
        
        if (chunk != nullptr) 
            continue;
        
        chunk = CreateChunk(chunkWorldX, chunkWorldZ);
        
        chunk->position = glm::vec2(chunkX, chunkZ);
        chunk->gameObject->SetPosition(chunkX, 0, chunkZ);
        
        Engine.SetHeightFieldValues(heightField, chunkSize, chunkSize, 0);
        Engine.SetColorFieldValues(colorField, chunkSize, chunkSize, Colors.white);
        
        Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.024f);
        
        
        //
        // Perlin noise layers
        
        chunk->gameObject->SetPosition(chunkWorldX, 0, chunkWorldZ);
        
        MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
        
        // Chunk perlin
        unsigned int numberOfLayers = mPerlinLayers.size();
        for (unsigned int l=0; l < numberOfLayers; l++) {
            
            Perlin* perlinLayer = &mPerlinLayers[l];
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize, 
                                                 perlinLayer->noiseWidth, 
                                                 perlinLayer->noiseHeight, 
                                                 perlinLayer->heightMultuplier, 
                                                 chunkWorldX, chunkWorldZ);
            
        }
        
        //
        // Biome test
        
        //Engine.SetColorFieldFromPerlinNoise(colorField, chunkSize, chunkSize, 0.01, 0.01, 0.4, Colors.blue, Colors.red, chunkX, chunkZ);
        
        Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.024f);
        
        //
        // Snow cap
        
        Engine.AddColorFieldSnowCap(colorField, heightField, chunkSize, chunkSize, Colors.white, world.snowCapHeight, 7.0f);
        
        //
        // Water table
        
        Engine.AddColorFieldWaterTable(colorField, heightField, chunkSize, chunkSize, world.waterColor, world.waterLevel, 0.1f, world.waterLevel);
        
        chunk->waterObject->renderDistance = (renderDistance * (chunkSize / 2)) * 2.0f;
        
        Transform* transformWater = chunk->waterObject->GetComponent<Transform>();
        transformWater->position.x = chunkWorldX;
        transformWater->position.z = chunkWorldZ;
        
        
        
        
        //
        // Finalize chunk mesh
        
        Engine.AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSize, chunkSize, 0, 0, 1, 1);
        chunkRenderer->mesh->Load();
        
        
        
        
        // Generate rigid body
        
        RigidBody* chunkBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
        chunk->rigidBody = chunkBody;
        
        chunkBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        chunkBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        chunkBody->setType(rp3d::BodyType::STATIC);
        
        rp3d::Transform bodyTransform = rp3d::Transform::identity();
        bodyTransform.setPosition( rp3d::Vector3(chunkWorldX, 0, chunkWorldZ) );
        chunkBody->setTransform(bodyTransform);
        
        // Generate a height field collider
        MeshCollider*  meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSize, chunkSize);
        chunk->collider = meshCollider;
        
        rp3d::Collider* bodyCollider = chunkBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
        bodyCollider->setUserData( (void*)chunk->gameObject );
        //bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
        //bodyCollider->setCollideWithMaskBits(0);
        
        chunk->bodyCollider = bodyCollider;
        
        
        
        //
        // Generate world decorations and actors
        //
        
        Mesh* staticMesh = chunk->staticObjects->GetComponent<MeshRenderer>()->mesh;
        
        for (int xx=0; xx < chunkSize-1; xx++) {
            
            for (int zz=0; zz < chunkSize-1; zz++) {
                
                int xp = xx - (chunkSize / 2);
                int zp = zz - (chunkSize / 2);
                
                int staticX = (chunkWorldX - xp);
                int staticZ = (chunkWorldZ - zp);
                
                glm::vec3 from(staticX, 0, staticZ);
                glm::vec3 direction(0, -1, 0);
                
                Hit hit;
                
                float distance = 2000;
                float height = 0;
                
                if (Physics.Raycast(from, direction, distance, hit)) 
                    height = hit.point.y;
                
                if (height > world.staticHeightCutoff) 
                    continue;
                
                if (height == 0) 
                    continue;
                
                unsigned int stackHeight = Random.Range(0, 5);
                
                unsigned int decorIndex = Random.Range(0, world.mDecorations.size());
                
                Decoration decor = world.mDecorations[ decorIndex ];
                
                if (Random.Range(0, 1000) > decor.density) 
                    continue;
                
                if ((height > decor.spawnHeightMaximum) & 
                    (height < decor.spawnHeightMinimum)) 
                    continue;
                
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
                        
                        break;
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
                        
                        break;
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
                        
                        break;
                    }
                    
                    // Tree
                    case 3: {
                        
                        // Tree logs
                        
                        float logHeight = Random.Range((float)world.treeHeightLow, (float)world.treeHeightHigh);
                        
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
                        
                        
                        break;
                    }
                    
                    // Tree
                    case 4: {
                        
                        GameObject* actorObject = Engine.CreateAIActor( glm::vec3(from.x, 0, from.z) );
                        
                        chunk->actorList.push_back( actorObject );
                        
                        Actor* actor = actorObject->GetComponent<Actor>();
                        actor->SetHeightPreferenceMin(world.waterLevel);
                        actor->SetHeightPreferenceMax(40.0f);
                        
                        // Use sheep actor preset
                        AI.genomes.SheepGene( actor );
                        
                        break;
                    }
                    
                    
                    default:
                        break;
                }
                
                
                
                
                
                
                
                
                /*
                // Actor generation dencity
                if (Random.Range(0, 1000) < world.actorDensity) {
                    
                    GameObject* actorObject = Engine.CreateAIActor( glm::vec3(from.x, 0, from.z) );
                    
                    chunk->actorList.push_back( actorObject );
                    
                    Actor* actor = actorObject->GetComponent<Actor>();
                    actor->SetHeightPreferenceMin(world.waterLevel);
                    actor->SetHeightPreferenceMax(40.0f);
                    
                    // Use sheep actor preset
                    AI.genomes.SheepGene( actor );
                    
                    continue;
                }
                
                // Static generation density
                if (Random.Range(0, 1000) < world.treeDensity) {
                    
                    // Tree logs
                    
                    float logHeight = Random.Range((float)world.treeHeightLow, (float)world.treeHeightHigh);
                    
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
                */
                
                
            }
            
        }
        
        staticMesh->Load();
        
        mActiveChunks.push_back(chunk);
        
    }
    
    return;
}


void ChunkManager::PurgeChunks(glm::vec2 position) {
    
    unsigned int numberOfChunks = mActiveChunks.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        for (std::vector<Chunk*>::iterator it = mActiveChunks.begin(); it != mActiveChunks.end(); ++it) {
            
            Chunk* chunkPtr = *it;
            
            if (glm::distance(position, chunkPtr->position) < ((renderDistance * 1.24f) * chunkSize)) 
                continue;
            
            DestroyChunk( chunkPtr );
            
            mActiveChunks.erase( it );
            
            break;
        }
        
    }
    
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
    
    return;
}

