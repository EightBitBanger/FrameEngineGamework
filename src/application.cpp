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

class ChunkManager {
    
public:
    
    std::vector<glm::vec2>   chunkList;
    std::vector<GameObject*> chunkObjects;
    
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
        generationDistance(500),
        destructionDistance(500),
        
        chunkSize(32),
        
        currentChunkX(0),
        currentChunkZ(0),
        
        chunkIndex(0),
        
        mMaterial(nullptr),
        mNumberOfChunksToUpdate(100)
    {}
    
    
    void SetMaterial(Material* material) {
        mMaterial = material;
        return;
    }
    
    void AddChunk(glm::vec2 position) {
        chunkList.push_back( position );
        return;
    }
    
    void RemoveChunk(unsigned int index) {
        chunkList.erase( (chunkList.begin() + index) );
        return;
    }
    
    int FindChunk(glm::vec2 position) {
        for (unsigned int i=0; i < chunkList.size(); i++) {
            if (chunkList[i] != position) 
                continue;
            return i;
        }
        return -1;
    }
    
    
    
    void GeneratePhysicsCollider(RigidBody* rigidBody, float* heightField, unsigned int chunkSize) {
        
        MeshCollider* collider = Physics.CreateHeightFieldMap(heightField, chunkSize, chunkSize);
        
        rp3d::Transform offsetTransform;
        //offsetTransform.setPosition(rp3d::Vector3((chunkSize - 1) * x, 0, (chunkSize - 1) * z));
        
        rp3d::Collider* colliderBody = rigidBody->addCollider( collider->heightFieldShape, offsetTransform );
        
        return;
    }
    
    
    GameObject* CreateBaseChunk(float x, float z, Material* material) {
        
        Mesh* chunkMesh = Engine.Create<Mesh>();
        
        GameObject* plainObject = Engine.Create<GameObject>();
        
        plainObject->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, mMaterial ) );
        //plainObject->AddComponent( Engine.CreateComponent<RigidBody>() );
        
        MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer, RENDER_QUEUE_BACKGROUND );
        plainRenderer->mesh->SetPrimitive( MESH_TRIANGLES );
        
        plainObject->SetAngularAxisLockFactor(0, 0, 0);
        plainObject->SetLinearAxisLockFactor(0, 0, 0);
        plainObject->SetStatic();
        
        plainObject->renderDistance = generationDistance * 1.2;
        
        plainObject->SetPosition(x, 0, z);
        
        return plainObject;
    }
    
    void GenerateChunk(float chunkX, float chunkZ, float* heightField, glm::vec3* colorField) {
        
        Engine.SetHeightFieldValues(heightField, chunkSize, chunkSize, 0);
        Engine.SetColorFieldValues(colorField, chunkSize, chunkSize, Colors.white);
        
        // Generate game object
        
        GameObject* plainObject = CreateBaseChunk(chunkX, chunkZ, mMaterial);
        
        chunkObjects.push_back(plainObject);
        
        MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
        
        plainRenderer->isActive = false;
        
        //RigidBody* plainBody = plainObject->GetComponent<RigidBody>();
        
        
        
        // Main noise channels
        
        Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.004, 0.004,  100, chunkX, chunkZ);
        
        Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize,  0.01, 0.01,     40, chunkX, chunkZ);
        
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
        
        // Generate a height field collider
        //GeneratePhysicsCollider( plainBody, heightField, chunkSize );
        
        // Add the chunk to the mesh
        
        Mesh* plainMesh = plainRenderer->mesh;
        
        Engine.AddHeightFieldToMesh(plainMesh, heightField, colorField, chunkSize, chunkSize, 0, 0);
        
        plainMesh->UploadToGPU();
        
        return;
    }
    
    void DestroyChunk(unsigned int index) {
        
        
        
        
        
    }
    
    void Update(void) {
        
        //
        // Chunk destruction
        //
        
        if (chunkObjects.size() > 0) {
            
            if (chunkIndex >= chunkObjects.size()) 
                chunkIndex = 0;
            
            GameObject* chunk = chunkObjects[chunkIndex];
            Transform* transform = chunk->GetComponent<Transform>();
            
            glm::vec3 chunkPosition  = transform->position;
            glm::vec3 playerPosition = Engine.sceneMain->camera->transform.position;
            
            // Ignore height
            chunkPosition.y = 0;
            playerPosition.y = 0;
            
            if (glm::distance(chunkPosition, playerPosition) > destructionDistance) {
                
                int index = FindChunk( glm::vec2(chunkPosition.x, chunkPosition.z) );
                
                if (index >= 0) {
                    
                    /*
                    
                    // Remove the chunk from the chunk index
                    RemoveChunk( index );
                    
                    // Remove the chunk object from the objects list
                    chunkObjects.erase( chunkObjects.begin() + chunkIndex );
                    
                    // Marked for destruction
                    chunk->isGarbage = true;
                    
                    MeshRenderer* meshRenderer = chunk->GetComponent<MeshRenderer>();
                    
                    Renderer.DestroyMesh( meshRenderer->mesh );
                    
                    //meshRenderer->mesh = nullptr;
                    
                    //meshRenderer->isActive = false;
                    
                    */
                    
                    
                }
                
            }
            
            chunkIndex++;
        }
        
        
        
        //
        // Attempt chunk generation
        //
        
        float playerChunkX = glm::round( Engine.sceneMain->camera->transform.position.x / (chunkSize - 1));
        float playerChunkZ = glm::round( Engine.sceneMain->camera->transform.position.z / (chunkSize - 1));
        
        // Decrease chunk update rate
        mNumberOfChunksToUpdate--;
        
        if (mNumberOfChunksToUpdate < 1) 
            mNumberOfChunksToUpdate = 1;
        
        float heightMap    [ chunkSize * chunkSize ];
        glm::vec3 colorMap [ chunkSize * chunkSize ];
        
        
        for (unsigned int i=0; i < mNumberOfChunksToUpdate; i++) {
            
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
            mNumberOfChunksToUpdate++;
            
            if (mNumberOfChunksToUpdate > 8) 
                mNumberOfChunksToUpdate = 8;
            
            AddChunk( chunkPosition );
            
            GenerateChunk(chunkX, chunkZ, heightMap, colorMap);
            
            continue;
        }
        
        return;
    }
    
private:
    
    // Number of chunks being updated per frame
    unsigned int mNumberOfChunksToUpdate;
    
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
    /*
    text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    text[4]->text = "Draw calls - " + Float.ToString( Renderer.GetNumberOfDrawCalls() );
    
    text[6]->text = "x - " + Int.ToString( cameraController->GetComponent<Transform>()->position.x );
    text[7]->text = "y - " + Int.ToString( cameraController->GetComponent<Transform>()->position.y );
    text[8]->text = "z - " + Int.ToString( cameraController->GetComponent<Transform>()->position.z );
    */
    
    
    
    //
    // Resources
    //
    
    if (Input.CheckKeyCurrent(VK_M)) {
        
        for (unsigned int i=0; i < 400; i++) {
            
            GameObject* gameObject = Engine.Create<GameObject>();
            
            gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            
            meshRenderer->mesh = Engine.Create<Mesh>();
            
            //gameObject->AddComponent( Engine.CreateComponent<RigidBody>() );
            
            
            Engine.Destroy<GameObject>( gameObject );
            
        }
    }
    
    
    if (!init) {
        
        text[1]->text = "MeshRenderer ---- " + Float.ToString( Renderer.GetNumberOfMeshRenderers() );
        text[2]->text = "Mesh ------------ " + Float.ToString( Renderer.GetNumberOfMeshes() );
        
        init = true;
    }
    
    text[3]->text = "MeshRenderer ---- " + Float.ToString( Renderer.GetNumberOfMeshRenderers() );
    text[4]->text = "Mesh ------------ " + Float.ToString( Renderer.GetNumberOfMeshes() );
    
    //text[6]->text = "Meshes --------- " + Float.ToString( Renderer.GetNumberOfMeshes() );
    //text[7]->text = "MeshRenderers -- " + Float.ToString( Renderer.GetNumberOfMeshRenderers() );
    
    //text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    //text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    
    
    
    
    
    
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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return;
    
    
    
    
    
    
    
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
        
        // No movement when puased
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




