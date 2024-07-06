// Chunk management class

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>

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
    
    WorldGeneration() : 
        actorsPerChunk(0),
        staticPerChunk(0),
        
        snowCapHeight(50),
        
        treeHeightLow(5),
        treeHeightHigh(8),
        
        leafSpreadArea(4.0f),
        leafSpreadHeight(1.0f),
        leafHeightOffset(-0.8f),
        numberOfLeaves(15)
        
        
    {}
    
    /// Number of actors to spawn.
    unsigned int actorsPerChunk;
    
    /// Number of static objects to spawn.
    unsigned int staticPerChunk;
    
    /// Height at which the world will generate snow on mountain tops
    float snowCapHeight;
    
    /// Perlin layers to apply to the world
    std::vector<Perlin> perlinGraph;
    
    
    // Tree trunk
    
    /// Tree trunk minimum height
    unsigned int treeHeightLow;
    
    /// Tree trunk maximum height
    unsigned int treeHeightHigh;
    
    
    // Tree generation
    
    /// Areal spread of the tree leaves.
    float leafSpreadArea;
    
    /// Height spread of the tree leaves.
    float leafSpreadHeight;
    
    /// Height starting offset for leaf spread.
    float leafHeightOffset;
    
    /// Number of leaves to add to the trees.
    unsigned int numberOfLeaves;
    
    
    /// Add a layer of perlin noise to the world generation
    void AddPerlinLayer(Perlin& perlinLayer) {
        perlinGraph.push_back(perlinLayer);
        return;
    }
    
};




class ENGINE_API ChunkManager {
    
public:
    
    /// Should the chunks be loaded around the player or a fixed position.
    bool doUpdateWithPlayerPosition;
    
    /// Distance to stop generating chunks.
    int generationDistance;
    int destructionDistance;
    
    /// Chunk size (must be a multiple of eight).
    int chunkSize;
    
    /// Current chunk being generated.
    int currentChunkX;
    int currentChunkZ;
    
    /// Current actor being processed.
    unsigned int currentActorIndex;
    
    /// Chunk update cycle index.
    unsigned int chunkIndex;
    
    /// Render distance multiplier
    float renderDistance;
    
    /// World generation parameters
    WorldGeneration world;
    
    
    ChunkManager() : 
        doUpdateWithPlayerPosition(true),
        
        generationDistance(200),
        destructionDistance(200),
        
        chunkSize(128),
        
        currentChunkX(0),
        currentChunkZ(0),
        
        currentActorIndex(0),
        
        chunkIndex(0),
        
        renderDistance(1),
        
        mMaterial(nullptr),
        
        mNumberOfChunksToGenerate(1),
        mMaxChunksToGenerate(10),
        
        mNumberOfChunksToPurge(1),
        mMaxChunksToPurge(4)
    {}
    
    
    void SetMaterial(Material* material) {
        mMaterial = material;
        return;
    }
    
    void AddChunk(Chunk chunk) {
        mChunkList.push_back( chunk );
        return;
    }
    
    void RemoveChunk(unsigned int index) {
        mChunkList.erase( (mChunkList.begin() + index) );
        return;
    }
    
    int FindChunk(glm::vec2 position) {
        for (unsigned int i=0; i < mChunkList.size(); i++) {
            if (mChunkList[i].position != position) 
                continue;
            return i;
        }
        return -1;
    }
    
    
    
    GameObject* CreateBaseChunk(float x, float z) {
        
        Mesh* chunkMesh = Engine.Create<Mesh>();
        
        GameObject* baseChunk = Engine.Create<GameObject>();
        
        baseChunk->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, mMaterial ) );
        
        MeshRenderer* plainRenderer = baseChunk->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer, RENDER_QUEUE_BACKGROUND );
        plainRenderer->mesh->SetPrimitive( MESH_TRIANGLES );
        
        baseChunk->renderDistance = renderDistance;
        
        baseChunk->SetPosition(x, 0, z);
        
        return baseChunk;
    }
    
    
    void Update(void) {
        
        //
        // Purge chunks
        //
        
        if (doUpdateWithPlayerPosition) {
            
            // Decrease chunk update rate
            mNumberOfChunksToPurge--;
            
            if (mNumberOfChunksToPurge < 1) 
                mNumberOfChunksToPurge = 1;
            
            for (unsigned int i=0; i < mNumberOfChunksToPurge; i++) {
                
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
                
                if (glm::distance(chunkPosition, playerPosition) > destructionDistance) {
                    
                    int index = FindChunk( glm::vec2(chunkPosition.x, chunkPosition.z) );
                    
                    if (index == -1) 
                        continue;
                    
                    // Destroy chunk actors
                    unsigned int numberOfActors = chunk.actorList.size();
                    
                    if (numberOfActors > 0) {
                        
                        for (unsigned int i=0; i < numberOfActors; i++) {
                            
                            GameObject* actorObject = chunk.actorList[i];
                            
                            Engine.Destroy<GameObject>( actorObject );
                            
                        }
                        
                    }
                    
                    // Destroy chunk static objects
                    MeshRenderer* staticRenderer = chunk.staticObjects->GetComponent<MeshRenderer>();
                    
                    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_DEFAULT );
                    
                    Engine.Destroy<GameObject>( chunk.staticObjects );
                    
                    
                    // Remove the chunk from the chunk index
                    RemoveChunk( index );
                    
                    // Destroy physics objects
                    chunk.rigidBody->removeCollider( chunk.bodyCollider );
                    
                    Physics.world->destroyRigidBody( chunk.rigidBody );
                    
                    Physics.common.destroyHeightFieldShape( chunk.collider->heightFieldShape );
                    
                    // Destroy height field collider and associated buffer
                    MeshCollider* collider = chunk.collider;
                    Physics.DestroyHeightFieldMap( collider );
                    
                    // Marked for destruction
                    Engine.Destroy<GameObject>( chunk.gameObject );
                    
                    // Increase chunk purge rate
                    mNumberOfChunksToPurge++;
                    
                    if (mNumberOfChunksToPurge > mMaxChunksToPurge) 
                        mNumberOfChunksToPurge = mMaxChunksToPurge;
                    
                }
                
                continue;
            }
            
        }
        
        
        
        
        
        //
        // Generate chunks
        //
        
        float playerChunkX = 0;
        float playerChunkZ = 0;
        
        if (doUpdateWithPlayerPosition) {
            playerChunkX = glm::round( Engine.sceneMain->camera->transform.position.x / (chunkSize - 1));
            playerChunkZ = glm::round( Engine.sceneMain->camera->transform.position.z / (chunkSize - 1));
        }
        
        // Decrease chunk update rate
        mNumberOfChunksToGenerate--;
        
        if (mNumberOfChunksToGenerate < 1) 
            mNumberOfChunksToGenerate = 1;
        
        float     heightField [ chunkSize * chunkSize ];
        glm::vec3 colorField  [ chunkSize * chunkSize ];
        
        
        for (unsigned int i=0; i < mNumberOfChunksToGenerate; i++) {
            
            float chunkX = ((currentChunkX + playerChunkX) * (chunkSize - 1)) - (generationDistance / 2);
            float chunkZ = ((currentChunkZ + playerChunkZ) * (chunkSize - 1)) - (generationDistance / 2);
            
            glm::vec2 chunkPosition = glm::vec2(chunkX, chunkZ);
            
            // Chunk counter
            
            currentChunkX++;
            
            if (currentChunkX > generationDistance / chunkSize) {
                
                currentChunkX = 0;
                
                currentChunkZ++;
                
                if (currentChunkZ > generationDistance / chunkSize) 
                    currentChunkZ = 0;
                
            }
            
            // Check chunk does not exist
            if (FindChunk( chunkPosition ) > -1) 
                continue;
            
            // Increase chunk update rate
            mNumberOfChunksToGenerate++;
            
            if (mNumberOfChunksToGenerate > mMaxChunksToGenerate) 
                mNumberOfChunksToGenerate = mMaxChunksToGenerate;
            
            //
            // Generate chunk
            //
            
            Engine.SetHeightFieldValues(heightField, chunkSize, chunkSize, 0);
            Engine.SetColorFieldValues(colorField, chunkSize, chunkSize, Colors.white);
            
            // Generate base chunk object
            
            Chunk chunk;
            
            GameObject* baseObject = CreateBaseChunk(chunkX, chunkZ);
            
            // Chunk material
            MeshRenderer* baseRenderer = baseObject->GetComponent<MeshRenderer>();
            baseRenderer->isActive = false;
            
            
            // Main noise channels
            
            for (unsigned int n=0; n < world.perlinGraph.size(); n++) {
                
                float noiseWidth  = world.perlinGraph[n].noiseWidth;
                float noiseHeight = world.perlinGraph[n].noiseHeight;
                
                float heightMul   = world.perlinGraph[n].heightMultuplier;
                
                if (world.perlinGraph[n].equation == 0) {
                    
                    Engine.AddHeightFieldFromPerlinNoise(heightField, 
                                                        chunkSize, chunkSize, 
                                                        noiseWidth, noiseHeight, heightMul, 
                                                        chunkX, chunkZ);
                    
                } else {
                    
                    Engine.AverageHeightFieldFromPerlinNoise(heightField, 
                                                             chunkSize, chunkSize, 
                                                             noiseWidth, noiseHeight, heightMul, 
                                                             chunkX, chunkZ);
                    
                }
                
            }
            
            
            // Biome range
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.001,  0.001, 100, chunkX, chunkZ);
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize, 0.0001, 0.0001, 500, chunkX, chunkZ);
            
            
            
            //
            // Biome effect on terrain color
            //
            
            //Engine.SetColorFieldFromPerlinNoise(colorField, chunkSize, chunkSize, 0.01, 0.01, 0.4, Colors.blue, Colors.red, chunkX, chunkZ);
            
            
            
            // Apply terrain color by height value
            Color colorLow;
            Color colorHigh;
            
            colorLow  = Colors.dkgreen * 0.07f;
            colorHigh = Colors.brown   * 0.07f;
            
            Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.024f);
            
            // Snow cap
            Engine.AddColorFieldSnowCap(colorField, heightField, chunkSize, chunkSize, Colors.white, world.snowCapHeight, 7.0f);
            
            
            // Generate rigid body
            
            RigidBody* chunkBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
            
            chunkBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
            chunkBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
            chunkBody->setType(rp3d::BodyType::STATIC);
            
            rp3d::Transform bodyTransform = rp3d::Transform::identity();
            bodyTransform.setPosition( rp3d::Vector3(chunkX, 0, chunkZ) );
            chunkBody->setTransform(bodyTransform);
            
            
            
            // Generate a height field collider
            MeshCollider*   meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSize, chunkSize);
            
            rp3d::Collider* BodyCollider = chunkBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
            
            
            // Add the chunk to the mesh
            
            Engine.AddHeightFieldToMesh(baseRenderer->mesh, heightField, colorField, chunkSize, chunkSize, 0, 0);
            
            baseRenderer->mesh->UploadToGPU();
            
            
            
            //
            // Generate actors
            //
            
            for (unsigned int a=0; a < world.actorsPerChunk; a++) {
                
                float actorX = chunkX + (Random.Range(0, chunkSize / 2) - Random.Range(0, chunkSize / 2));
                float actorZ = chunkZ + (Random.Range(0, chunkSize / 2) - Random.Range(0, chunkSize / 2));
                
                GameObject* actorObject = Engine.CreateAIActor( glm::vec3(actorX, 0, actorZ) );
                
                chunk.actorList.push_back( actorObject );
                
                // Actor
                Actor* actor = actorObject->GetComponent<Actor>();
                
                // Use sheep actor preset
                AI.genomes.SheepGene( actor );
                
                //actor->SetChanceToWalk(0);
                //actor->SetChanceToChangeDirection(0);
                //actor->SetChanceToStopWalking(0);
                //actor->SetChanceToFocusOnActor(0);
                
                //actor->SetActive(false);
                
            }
            
            
            // Chunk mesh for static object batching
            Mesh* staticMesh = Engine.Create<Mesh>();
            
            SubMesh subMeshHorz;
            SubMesh subMeshVert;
            SubMesh subMeshTree;
            
            Engine.meshes.log->GetSubMesh(2, subMeshTree);
            
            //Engine.meshes.grass->GetSubMesh(0, subMeshHorz);
            //Engine.meshes.grass->GetSubMesh(1, subMeshVert);
            
            Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshHorz);
            Engine.meshes.wallVertical->GetSubMesh(0, subMeshVert);
            
            staticMesh->isShared = false;
            
            
            
            // Chunk material
            
            Material* staticMaterial = Engine.Create<Material>();
            
            staticMaterial->shader = Engine.shaders.color;
            staticMaterial->DisableCulling();
            
            staticMaterial->ambient = Colors.Make(0.27, 0.27, 0.27);
            staticMaterial->diffuse = Colors.white;
            staticMaterial->specular = Colors.white;
            
            staticMaterial->isShared = false;
            
            
            // Generate the static object container
            
            GameObject* staticObjectContainer = Engine.Create<GameObject>();
            staticObjectContainer->renderDistance = renderDistance;
            
            Transform* transform = staticObjectContainer->GetComponent<Transform>();
            
            transform->position.x = chunkX;
            transform->position.z = chunkZ;
            
            // Assemble the object components
            staticObjectContainer->AddComponent( Engine.CreateComponentMeshRenderer(staticMesh, staticMaterial) );
            
            MeshRenderer* staticRenderer = staticObjectContainer->GetComponent<MeshRenderer>();
            
            chunk.staticObjects = staticObjectContainer;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer);
            
            staticObjectContainer->Deactivate();
            
            
            
            //
            // Trees
            //
            
            for (unsigned int a=0; a < world.staticPerChunk; a++) {
                
                float xx = Random.Range(0, chunkSize) - Random.Range(0, chunkSize);
                float zz = Random.Range(0, chunkSize) - Random.Range(0, chunkSize);
                
                if ((xx >  (chunkSize/2))   | (zz >  (chunkSize/2)) | 
                    (xx < -(chunkSize/2)) | (zz < (-chunkSize/2))) 
                    continue;
                
                // Ray cast here to find the ground
                glm::vec3 from(chunkX + xx, 1000, chunkZ + zz);
                glm::vec3 direction(0, -1, 0);
                
                Hit hit;
                
                float distance = 2000;
                float height = 0;
                
                if (Physics.Raycast(from, direction, distance, hit)) 
                    height = hit.point.y;
                
                if (height > world.snowCapHeight) 
                    continue;
                
                if (height == 0) 
                    continue;
                
                
                // Logs
                float logHeight = Random.Range((float)world.treeHeightLow, (float)world.treeHeightHigh);
                
                for (unsigned int s=0; s < logHeight; s++) {
                    
                    staticMesh->AddSubMesh(0, 0, 0, subMeshTree, false);
                    
                    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                    
                    unsigned int index = numberOfSubMeshes - 1;
                    
                    Color finalColor;
                    Color lowTrunk;
                    Color highTrunk;
                    
                    lowTrunk  = (Colors.brown * 0.01f)  + (Colors.green * 0.001);
                    highTrunk = (Colors.brown * 0.008f) + (Colors.green * 0.01);
                    
                    finalColor = Colors.Lerp(lowTrunk, highTrunk, (s * 0.087f));
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    
                    staticMesh->ChangeSubMeshPosition(index, xx, height + s, zz);
                    
                }
                
                
                
                for (unsigned int z=0; z < world.numberOfLeaves; z++) {
                    
                    float offset_xx = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                    float offset_yy = Random.Range(0.0f, world.leafSpreadHeight) - Random.Range(0.0f, world.leafSpreadHeight);
                    float offset_zz = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                    
                    staticMesh->AddSubMesh(0, 0, 0, subMeshVert, false);
                    staticMesh->AddSubMesh(0, 0, 0, subMeshHorz, false);
                    
                    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                    unsigned int index = numberOfSubMeshes - 1;
                    
                    Color finalColor;
                    Color lowTrunk;
                    Color highTrunk;
                    
                    lowTrunk  = (Colors.green * 0.01f)  + (Colors.green * 0.01);
                    highTrunk = (Colors.green * 0.008f) + (Colors.green * 0.01);
                    
                    finalColor = Colors.Lerp(lowTrunk, highTrunk, 0.087f);
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    staticMesh->ChangeSubMeshColor(index-1, finalColor);
                    
                    staticMesh->ChangeSubMeshPosition(index,   xx + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, zz + offset_zz);
                    staticMesh->ChangeSubMeshPosition(index-1, xx + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, zz + offset_zz);
                    
                }
                
                continue;
            }
            
            
            
            
            
            
            
            
            
            //
            // Grass
            //
            
            for (unsigned int a=0; a < world.staticPerChunk; a++) {
                
                float xx = Random.Range(0, chunkSize) - Random.Range(0, chunkSize);
                float zz = Random.Range(0, chunkSize) - Random.Range(0, chunkSize);
                
                if ((xx > (chunkSize/2)) | (zz > (chunkSize/2)) | 
                    (xx < -(chunkSize/2)) | (zz < (-chunkSize/2))) 
                    continue;
                
                // Ray cast here to find the ground
                glm::vec3 from(chunkX + xx, 1000, chunkZ + zz);
                glm::vec3 direction(0, -1, 0);
                
                Hit hit;
                
                float distance = 2000;
                float height = 0;
                
                if (Physics.Raycast(from, direction, distance, hit)) 
                    height = hit.point.y;
                
                if (height > world.snowCapHeight) 
                    continue;
                
                if (height == 0) 
                    continue;
                
                staticMesh->AddSubMesh(0, 0, 0, subMeshVert, false);
                staticMesh->AddSubMesh(0, 0, 0, subMeshHorz, false);
                
                unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                
                unsigned int index = numberOfSubMeshes - 1;
                
                Color finalColor;
                Color lowGreen;
                Color highGreen;
                
                lowGreen  = (Colors.dkgreen + Colors.MakeRandomGrayScale()) * 0.01f;
                highGreen = (Colors.green   + Colors.MakeRandomGrayScale()) * 0.08f;
                
                finalColor = Colors.Lerp(lowGreen, highGreen, 0.087f);
                
                staticMesh->ChangeSubMeshColor(index, finalColor);
                staticMesh->ChangeSubMeshColor(index-1, finalColor);
                
                staticMesh->ChangeSubMeshPosition(index, xx, height, zz);
                staticMesh->ChangeSubMeshPosition(index-1, xx, height, zz);
                
                continue;
            }
            
            
            staticMesh->UploadToGPU();
            
            
            // Add chunk to chunk list
            chunk.position     = chunkPosition;
            chunk.gameObject   = baseObject;
            chunk.rigidBody    = chunkBody;
            chunk.collider     = meshCollider;
            chunk.bodyCollider = BodyCollider;
            
            AddChunk( chunk );
            
            continue;
        }
        
        return;
    }
    
private:
    
    /// Material used for rendering the world chunks.
    Material* mMaterial;
    
    /// List of chunks in the world.
    std::vector<Chunk> mChunkList;
    
    // Number of chunks being updated per frame
    unsigned int mNumberOfChunksToGenerate;
    unsigned int mMaxChunksToGenerate;
    
    // Number of chunks being destroyed per frame
    unsigned int mNumberOfChunksToPurge;
    unsigned int mMaxChunksToPurge;
    
};