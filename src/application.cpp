// User application code space
//

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



// User functions



// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

GameObject* directionalLight;
Transform*  lightTransform;

Text* text[20];

Transform* bendJoint = nullptr;


Actor* testActor;

Material* plainMaterial;



MeshRenderer* combineRenderer;
SubMesh submesh;







// Day night cycle

bool cycleDirection = false;

float ambientLight = 0.3;

Material* skyMaterial;












//
// Chunk management
//

class Chunk {
    
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
    
    /// List of actors in this chunk
    std::vector<Actor*> mActors;
    
};






class ChunkManager {
    
public:
    
    std::vector<Chunk> chunkList;
    
    /// Distance to stop generating chunks.
    int generationDistance;
    int destructionDistance;
    
    /// Chunk size (must be a multiple of eight).
    int chunkSize;
    
    /// Current chunk being generated.
    int currentChunkX;
    int currentChunkZ;
    
    /// Chunk update cycle index.
    int chunkIndex;
    
    /// Material used for rendering the world chunks.
    Material* mMaterial;
    
    ChunkManager() : 
        generationDistance(900),
        destructionDistance(900),
        
        chunkSize(64),
        
        currentChunkX(0),
        currentChunkZ(0),
        
        chunkIndex(0),
        
        mMaterial(nullptr),
        
        mNumberOfChunksToGenerate(1),
        mMaxChunksToGenerate(10),
        
        mNumberOfChunksToPurge(1),
        mMaxChunksToPurge(3)
    {}
    
    
    void SetMaterial(Material* material) {
        mMaterial = material;
        return;
    }
    
    void AddChunk(Chunk chunk) {
        chunkList.push_back( chunk );
        return;
    }
    
    void RemoveChunk(unsigned int index) {
        chunkList.erase( (chunkList.begin() + index) );
        return;
    }
    
    int FindChunk(glm::vec2 position) {
        for (unsigned int i=0; i < chunkList.size(); i++) {
            if (chunkList[i].position != position) 
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
        
        baseChunk->renderDistance = generationDistance * 0.9;
        
        baseChunk->SetPosition(x, 0, z);
        
        return baseChunk;
    }
    
    
    
    void Update(void) {
        
        //
        // Purge chunks
        //
        
        // Decrease chunk update rate
        mNumberOfChunksToPurge--;
        
        if (mNumberOfChunksToPurge < 1) 
            mNumberOfChunksToPurge = 1;
        
        for (unsigned int i=0; i < mNumberOfChunksToPurge; i++) {
            
            if (chunkList.size() > 0) {
                
                if (chunkIndex >= chunkList.size()) 
                    chunkIndex = 0;
                
                Chunk chunk = chunkList[chunkIndex];
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
        
        
        
        
        //
        // Generation chunks
        //
        
        //float playerChunkX = glm::round( Engine.sceneMain->camera->transform.position.x / (chunkSize - 1));
        //float playerChunkZ = glm::round( Engine.sceneMain->camera->transform.position.z / (chunkSize - 1));
        
        float playerChunkX = 0;
        float playerChunkZ = 0;
        
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
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.002, 0.002,  240, chunkX, chunkZ);
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.01, 0.01,     80, chunkX, chunkZ);
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.03, 0.03,     10, chunkX, chunkZ);
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.08, 0.08,      4, chunkX, chunkZ);
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.2, 0.2,        1, chunkX, chunkZ);
            
            
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
    
    // Number of chunks being updated per frame
    unsigned int mNumberOfChunksToGenerate;
    unsigned int mMaxChunksToGenerate;
    
    // Number of chunks being destroyed per frame
    unsigned int mNumberOfChunksToPurge;
    unsigned int mMaxChunksToPurge;
    
};

ChunkManager chunkManager;







void functionTest(std::vector<std::string> args) {
    std::string combine;
    
    for (unsigned int i=0; i < args.size(); i++) 
        combine += args[i] + "+";
    
    text[15]->text = combine;
    
    return;
}





// Application entry point
//

void Start() {
    
    //Engine.EnablePhysicsDebugRenderer();
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    
    Transform* skyTransform = skyObject->GetComponent<Transform>();
    
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer, RENDER_QUEUE_SKY );
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(0, 30, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
    
    // Attach the sky object to the camera controller
    Transform* cameraTransform = cameraController->GetComponent<Transform>();
    skyTransform->parent = cameraTransform;
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 1 );
    cameraController->SetMass( 10 );
    cameraController->DisableGravity();
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    
    
    //
    // Directional light
    directionalLight = Engine.Create<GameObject>();
    lightTransform = directionalLight->GetComponent<Transform>();
    lightTransform->RotateAxis(1, Vector3(0.8, -0.7, 0.0));
    
    directionalLight->AddComponent( Engine.CreateComponent<Light>() );
    Light* sunLight = directionalLight->GetComponent<Light>();
    
    Engine.sceneMain->AddLightToSceneRoot( sunLight );
    sunLight->intensity  = 1;
    sunLight->type       = LIGHT_TYPE_DIRECTIONAL;
    sunLight->color      = Colors.white;
    
    
    
    //
    // Scene overlay
    
    sceneOverlay = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    sceneOverlay->camera = Engine.Create<Camera>();
    sceneOverlay->camera->isOrthographic = true;
    sceneOverlay->camera->isFixedAspect  = true;
    
    sceneOverlay->camera->viewport.w = Renderer.displaySize.x;
    sceneOverlay->camera->viewport.h = Renderer.displaySize.y;
    
    sceneOverlay->camera->clipFar  =  100;
    sceneOverlay->camera->clipNear = -100;
    
    
    
    
    // Initiate text elements
    for (int i=0; i < 20; i++) {
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.white, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
    }
    
    
    
    
    
    //
    // Chunk material
    //
    
    plainMaterial = Engine.Create<Material>();
    
    plainMaterial->shader = Engine.shaders.color;
    plainMaterial->isShared = true;
    
    chunkManager.SetMaterial( plainMaterial );
    
    
    
    
    
    
    
    // Register a command function into the console
    Engine.ConsoleRegisterCommand("shay", functionTest);
    
    
    
    
    
    
    
    
    
    return;
    
    
    
    //
    // Generate AI actors
    //
    
    unsigned int spread = 100;
    
    
    for (unsigned int i=0; i < 800; i++) {
        
        float xx = (Random.Range(0, spread) * 0.5) - (Random.Range(0, spread) * 0.5);
        float yy = -10;
        float zz = (Random.Range(0, spread) * 0.5) - (Random.Range(0, spread) * 0.5);
        
        GameObject* actorObject = Engine.CreateAIActor( glm::vec3(xx, yy, zz) );
        
        actorObject->renderDistance = 300;
        
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
    
    return;
}









//
// Application loop
//

glm::vec3 force(0);

unsigned int meshRendererCount = 0;
unsigned int meshCount  = 0;
bool init = false;


void Run() {
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    //
    // Pausing
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Platform.Pause();
        
        if (Platform.isPaused) {
            
            Engine.EnableConsole();
            
            mainCamera->DisableMouseLook();
            
            Input.ClearKeys();
            
            Platform.ShowMouseCursor();
            
        } else {
            
            Engine.DisableConsole();
            Engine.ConsoleClearInputString();
            
            mainCamera->EnableMouseLook();
            
            // Reset mouse position
            Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
            Platform.HideMouseCursor();
            
            // Reset timers
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    
    
    
    //
    // Profiling
    //
    
    text[1]->text  = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[1]->color = Colors.white;
    if (Profiler.profileRenderSystem > 10) text[1]->color = Colors.yellow;
    if (Profiler.profileRenderSystem > 20) text[1]->color = Colors.orange;
    if (Profiler.profileRenderSystem > 30) text[1]->color = Colors.red;
    
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[2]->color = Colors.white;
    if (Profiler.profilePhysicsSystem > 10) text[2]->color = Colors.yellow;
    if (Profiler.profilePhysicsSystem > 20) text[2]->color = Colors.orange;
    if (Profiler.profilePhysicsSystem > 30) text[2]->color = Colors.red;
    
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    text[4]->text = "Draw calls - " + Float.ToString( Renderer.GetNumberOfDrawCalls() );
    
    text[6]->text = "GameObject------- " + Int.ToString( Engine.GetNumberOfGameObjects() );
    text[7]->text = "Component-------- " + Int.ToString( Engine.GetNumberOfComponents() );
    text[8]->text = "MeshRenderer ---- " + Int.ToString( Renderer.GetNumberOfMeshRenderers() );
    text[9]->text = "Mesh ------------ " + Int.ToString( Renderer.GetNumberOfMeshes() );
    
    //text[6]->text = "x - " + Int.ToString( cameraController->GetComponent<Transform>()->position.x );
    //text[7]->text = "y - " + Int.ToString( cameraController->GetComponent<Transform>()->position.y );
    //text[8]->text = "z - " + Int.ToString( cameraController->GetComponent<Transform>()->position.z );
    
    
    
    
    
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    float lightingMax       = 0.87;
    float lightingMin       = 0.087;
    
    float skyLightingMax    = 0.87;
    float skyLightingMin    = 0.0034;
    
    float worldLightingMax  = 0.87;
    float worldLightingMin  = 0.34;
    
    
    
    bool adjustCycle = false;
    
    if (!Platform.isPaused) {
        if (Input.CheckKeyCurrent(VK_I)) {cycleDirection = true;  adjustCycle = true;}
        if (Input.CheckKeyCurrent(VK_K)) {cycleDirection = false; adjustCycle = true;}
    }
    
    if (adjustCycle) {
        if (cycleDirection) {
            ambientLight += 0.01f;
        } else {
            ambientLight -= 0.01f;
        }
    }
    
    
    // Ambient limits
    if (ambientLight > 0.87f) ambientLight = 0.87f;
    if (ambientLight < 0.0f)  ambientLight = 0.0f;
    
    // Sky brightness
    skyMaterial->ambient = Math.Lerp(skyLightingMin, skyLightingMax, ambientLight);
    
    // World brightness
    plainMaterial->diffuse = Math.Lerp(worldLightingMin, worldLightingMax, ambientLight);
    
    
    Light* sunLight = directionalLight->GetComponent<Light>();
    sunLight->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);;
    
    // Light direction
    lightTransform = directionalLight->GetComponent<Transform>();
    
    
    
    //
    // DEBUG - Manually adjust light direction
    //
    
    if (!Platform.isPaused) {
        if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
        if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
    }
    
    
    
    
    
    
    
    
    
    //
    // Map generation
    //
    
    chunkManager.Update();
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.03;
    float forceDecelerate = 0.02;
    float forceMax        = 18;
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
        bool moving = false;
        if (!Platform.isPaused) {
            if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward; moving = true;}
            if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward; moving = true;}
            if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right; moving = true;}
            if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right; moving = true;}
            
            // Elevation
            if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up; moving = true;}
            if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up; moving = true;}
        }
        
        // Accelerate
        if (glm::length(force) < forceMax) {
            force += (force * forceAccelerate) * glm::vec3(0.1);
        } else {
            
            // Check velocity caps
            if (force.x > forceMax) force.x = forceMax;
            if (force.y > forceMax) force.y = forceMax;
            if (force.z > forceMax) force.z = forceMax;
            
        }
        
        // Decelerate
        if ( glm::length(force) >  0.0001) force -= (force * forceDecelerate);
        if (-glm::length(force) < -0.0001) force -= (force * forceDecelerate);
        
        glm::vec3 forceMul = force * forceAccelerate;
        
        // Minimum speed cut off
        if (glm::length(force) < 0.001f) 
            force = glm::vec3(0.0f);
        
        if (moving) 
            if (Input.CheckKeyCurrent(VK_CONTROL)) forceMul *= 2;
        
        // Max force
        if ( glm::length(forceMul) >  forceMax) forceMul = glm::vec3(forceMax);
        if (-glm::length(forceMul) < -forceMax) forceMul = glm::vec3(-forceMax);
        
        if (forceMul != glm::vec3(0)) 
            cameraController->AddForce(forceMul.x, forceMul.y, forceMul.z);
        
        
        
        //
        // Update camera height
        //
        
        RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
        rp3d::Transform bodyTransform = rigidBody->getTransform();
        
        rp3d::Vector3 position = bodyTransform.getPosition();
        
        //position.y = z + 1;
        
        bodyTransform.setPosition(position);
        
        rigidBody->setTransform( bodyTransform );
        
        
    }
    
    
    
    
    
    
    
    return;
}


//
// Called once every tick (20 frames)
//

void TickUpdate(void) {
    
    return;
}





void Shutdown(void) {
    
    
    
    return;
}




