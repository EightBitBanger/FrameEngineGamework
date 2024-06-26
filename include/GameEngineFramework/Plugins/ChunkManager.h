// Chunk management class

#include <GameEngineFramework/Engine/Engine.h>

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



class ENGINE_API Chunk {
    
public:
    
    /// Position of the chunk in the world.
    glm::vec2 position;
    
    /// Associated game object.
    GameObject*   gameObject;
    
    /// Associated rigid body.
    rp3d::RigidBody* rigidBody;
    
    /// Associated collider.
    rp3d::Collider* bodyCollider;
    
    /// Height field collision buffer.
    MeshCollider* collider;
    
};


class Perlin {
    
public:
    
    Perlin() : 
        heightMultuplier(0.2),
        noiseWidth(0.2),
        noiseHeight(0.2)
    {}
    
    /// Mountain height multiplier
    float heightMultuplier;
    
    /// Perlin noise value horizontal
    float noiseWidth;
    
    /// Perlin noise value vertical
    float noiseHeight;
    
};


class ENGINE_API WorldGeneration {
    
public:
    
    WorldGeneration() : 
        actorsPerChunk(10)
    {}
    
    /// How many actors should be spawned per chunk
    unsigned int actorsPerChunk;
    
    /// Perlin layers to apply to the world
    std::vector<Perlin> perlinGraph;
    
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
    int chunkIndex;
    
    /// Render distance multiplier
    float renderDistance;
    
    /// World generation parameters
    WorldGeneration world;
    
    
    ChunkManager() : 
        doUpdateWithPlayerPosition(true),
        
        generationDistance(200),
        destructionDistance(200),
        
        chunkSize(64),
        
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
        
        baseChunk->renderDistance = 1000 * renderDistance;
        
        if (doUpdateWithPlayerPosition) 
            baseChunk->renderDistance = generationDistance * 0.87 * renderDistance;
        
        baseChunk->SetPosition(x, 0, z);
        
        return baseChunk;
    }
    
    
    
    void Update(void) {
        
        //
        // Check actors
        //
        /*
        int numberOfActors = mActorList.size() - 1;
        
        if (numberOfActors > 0) {
            if (currentActorIndex >= numberOfActors) 
                currentActorIndex = 0;
            
            GameObject* actorObject = mActorList[ currentActorIndex ];
            
            if (glm::distance( actorObject->GetComponent<Transform>()->position, Engine.sceneMain->camera->transform.position ) > 50) {
                
                Engine.Destroy<GameObject>( actorObject );
                
                mActorList.erase( mActorList.begin() + currentActorIndex );
                
            }
            currentActorIndex++;
        }
        
        */
        
        
        /*
        
        //
        // Purge chunks
        //
        
        if (doUpdateWithPlayerPosition) {
            
            // Decrease chunk update rate
            mNumberOfChunksToPurge--;
            
            if (mNumberOfChunksToPurge < 1) 
                mNumberOfChunksToPurge = 1;
            
            for (unsigned int i=0; i < mNumberOfChunksToPurge; i++) {
                
                if (mChunkList.size() > 0) {
                    
                    if (chunkIndex >= mChunkList.size()) 
                        chunkIndex = 0;
                    
                    Chunk chunk = mChunkList[chunkIndex];
                    Transform* transform = chunk.gameObject->GetComponent<Transform>();
                    
                    glm::vec3 chunkPosition  = transform->position;
                    glm::vec3 playerPosition = Engine.sceneMain->camera->transform.position;
                    
                    // Ignore height
                    chunkPosition.y = 0;
                    playerPosition.y = 0;
                    
                    if (glm::distance(chunkPosition, playerPosition) > destructionDistance) {
                        
                        int index = FindChunk( glm::vec2(chunkPosition.x, chunkPosition.z) );
                        
                        if (index >= 0) {
                            
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
                        
                    }
                    
                    chunkIndex++;
                }
                
                continue;
            }
            
        }
        
        */
        
        
        //
        // Generation chunks
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
            
            // Generate game object
            
            GameObject* baseObject = CreateBaseChunk(chunkX, chunkZ);
            
            MeshRenderer* baseRenderer = baseObject->GetComponent<MeshRenderer>();
            
            baseRenderer->isActive = false;
            
            
            
            
            
            // Main noise channels
            
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.002, 0.002,  240 * heightMul, chunkX, chunkZ);
            
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,   0.01,  0.01,   80 * heightMul, chunkX, chunkZ);
            
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,   0.03,  0.03,   10 * heightMul, chunkX, chunkZ);
            
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,   0.08,  0.08,    4 * heightMul, chunkX, chunkZ);
            
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,    0.2,   0.2,    1 * heightMul, chunkX, chunkZ);
            
            for (unsigned int n=0; n < world.perlinGraph.size(); n++) {
                
                float noiseWidth  = world.perlinGraph[n].noiseWidth;
                float noiseHeight = world.perlinGraph[n].noiseHeight;
                
                float heightMul   = world.perlinGraph[n].heightMultuplier;
                
                Engine.AddHeightFieldFromPerlinNoise(heightField, 
                                                     chunkSize, chunkSize, 
                                                     noiseWidth, noiseHeight, heightMul, 
                                                     chunkX, chunkZ);
                
            }
            
            
            // Biome range
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.001,  0.001, 100, chunkX, chunkZ);
            //Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize, 0.0001, 0.0001, 500, chunkX, chunkZ);
            
            
            
            //
            // Biome effect on terrain color
            //
            
            //Engine.SetColorFieldFromPerlinNoise(colorField, chunkSize, chunkSize, 0.01, 0.01, 0.4, Colors.blue, Colors.red, chunkX, chunkZ);
            
            
            
            // Apply terrain color by height value
            Color colorLow = Colors.green;
            colorLow *= Colors.MakeGrayScale(0.1);
            
            Color colorHigh = Colors.brown;
            
            Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.008f);
            
            // Snow cap
            Engine.AddColorFieldSnowCap(colorField, heightField, chunkSize, chunkSize, Colors.white, 50, 7.0);
            
            
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
                
                mActorList.push_back( actorObject );
                
                actorObject->renderDistance = generationDistance * 0.7;
                
                // Collision
                BoxShape* boxShape = Physics.CreateColliderBox(0.3, 0.3, 0.3);
                actorObject->AddColliderBox(boxShape, 0, 0.3, 0, LayerMask::Actor);
                
                
                // Actor
                Actor* actor = actorObject->GetComponent<Actor>();
                
                // Use sheep actor preset
                AI.genomes.SheepGene( actor );
                
                //actor->SetChanceToWalk(0);
                //actor->SetChanceToChangeDirection(0);
                //actor->SetChanceToStopWalking(0);
                //actor->SetChanceToFocusOnActor(0);
                
                actor->SetActive(false);
            }
            
            
            // Add chunk to chunk list
            
            Chunk chunk;
            
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
    
    /// List of actor objects in the world.
    std::vector<GameObject*> mActorList;
    
    // Number of chunks being updated per frame
    unsigned int mNumberOfChunksToGenerate;
    unsigned int mMaxChunksToGenerate;
    
    // Number of chunks being destroyed per frame
    unsigned int mNumberOfChunksToPurge;
    unsigned int mMaxChunksToPurge;
    
};
