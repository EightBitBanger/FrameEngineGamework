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
void RenderPhysicsDebugger(void);
void InitiatePhysicsDebugger(void);




// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

GameObject* directionalLight;
Transform*  lightTransform;

Text* text[20];

Transform* bendJoint = nullptr;


Actor* testActor;




MeshRenderer* combineRenderer;
SubMesh submesh;





// Day night cycle

bool cycleDirection = false;

float ambientLight = 0.3;

Material* skyMaterial;
Material* plainMaterial;






// Application entry point
//

void Start() {
    
    Engine.EnablePhysicsDebugRenderer();
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.997);
    skyLow  += Colors.MakeGrayScale(0.799);
    
    skyHigh *= Colors.MakeGrayScale(0.961);
    
    skyHigh *= Colors.MakeGrayScale(0.1);
    skyLow  *= Colors.MakeGrayScale(0.1);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    
    Transform* skyTransform = skyObject->GetComponent<Transform>();
    
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer, RENDER_QUEUE_SKY );
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(0, 3, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
    
    // Attach the sky object to the camera controller
    Transform* cameraTransform = cameraController->GetComponent<Transform>();
    skyTransform->parent = cameraTransform;
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 3 );
    cameraController->SetMass( 10 );
    cameraController->DisableGravity();
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    
    
    //
    // Directional light
    directionalLight = Engine.Create<GameObject>();
    lightTransform = directionalLight->GetComponent<Transform>();
    lightTransform->RotateAxis(1, Vector3(0.8, -0.7, 0.3));
    
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
    // Generate some ground chunks
    //
    
    // Chunk material
    
    plainMaterial = Engine.Create<Material>();
    
    TextureTag* plainTexture = Resources.FindTextureTag("grassy");
    plainTexture->Load();
    
    plainMaterial->texture.UploadTextureToGPU( plainTexture->buffer, plainTexture->width, plainTexture->height, MATERIAL_FILTER_ANISOTROPIC );
    
    plainMaterial->shader = Engine.shaders.texture;
    
    plainMaterial->diffuse = Colors.MakeGrayScale(0.87);
    plainMaterial->ambient = Colors.MakeGrayScale(0.87);
    
    plainMaterial->DisableCulling();
    
    
    
    
    // Chunk mesh
    
    Mesh* chunkMesh = Engine.Create<Mesh>();
    
    GameObject* plainObject = Engine.Create<GameObject>();
    plainObject->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, plainMaterial ) );
    plainObject->AddComponent( Engine.CreateComponent<RigidBody>() );
    
    MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer, RENDER_QUEUE_BACKGROUND );
    plainRenderer->mesh->SetPrimitive(MESH_TRIANGLES);
    
    plainObject->SetAngularAxisLockFactor(0, 0, 0);
    plainObject->SetLinearAxisLockFactor(0, 0, 0);
    plainObject->SetStatic();
    
    
    
    
    //
    // Generate height field map
    //
    
    // World
    int worldSize   = 20;
    
    // Chunk
    int chunkSize  = 8;
    
    // Noise
    float noiseWidth  = 0.1;
    float noiseHeight = 0.1;
    
    float heightMul = 8;
    
    
    
    
    RigidBody* heightFieldBody = plainObject->GetComponent<RigidBody>();
    
    float heightMap[chunkSize * chunkSize];
    
    for (int x = -worldSize; x <= worldSize; x++) {
        
        for (int z = -worldSize; z <= worldSize; z++) {
            
            Engine.GenerateHeightFieldMap(heightMap, chunkSize, chunkSize, 
                                          noiseWidth, noiseHeight, heightMul, 
                                         (chunkSize - 1) * x, (chunkSize - 1) * z);
            
            
            MeshCollider* collider = Physics.CreateHeightFieldMap(heightMap, chunkSize, chunkSize);
            
            rp3d::Transform offsetTransform;
            offsetTransform.setPosition(rp3d::Vector3((chunkSize - 1) * x, 0, (chunkSize - 1) * z));
            
            rp3d::Collider* colliderBody = heightFieldBody->addCollider( collider->heightFieldShape, offsetTransform );
            
            
            
            Engine.GenerateHeightFieldMesh(chunkMesh, heightMap, chunkSize, chunkSize, (chunkSize - 1) * x, (chunkSize - 1) * z);
            
        }
        
    }
    
    chunkMesh->UploadToGPU();
    
    
    
    
    
    return;
    
    
    
    //
    // Generate AI actors
    //
    
    unsigned int spread = 40;
    
    
    for (unsigned int i=0; i < 500; i++) {
        
        float xx = (Random.Range(0, spread) * 0.5) - (Random.Range(0, spread) * 0.5);
        float yy = 10;
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




glm::vec3 sunDir(0, 0, 1);
float sunStep = 0.5;
float sunRate = 1;


void Run() {
    
    //
    // Profiling
    //
    
    text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    text[4]->text = "Draw calls - " + Float.ToString( Renderer.GetNumberOfDrawCalls() );
    
    text[6]->text = "x - " + Float.ToString( cameraController->GetComponent<Transform>()->position.x );
    text[7]->text = "y - " + Float.ToString( cameraController->GetComponent<Transform>()->position.y );
    text[8]->text = "z - " + Float.ToString( cameraController->GetComponent<Transform>()->position.z );
    
    
    
    
    
    
    
    
    
    
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    bool adjustCycle = false;
    
    if (Input.CheckKeyCurrent(VK_I)) {cycleDirection = true;  adjustCycle = true;}
    if (Input.CheckKeyCurrent(VK_K)) {cycleDirection = false; adjustCycle = true;}
    
    
    //ambientLight += 0.01f;
    
    if (adjustCycle) {
        
        if (cycleDirection) {
            
            ambientLight += 0.03f;
            
        } else {
            
            ambientLight -= 0.03f;
            
        }
        
    }
    
    // Ambient limits
    if (ambientLight > 1.1)   ambientLight = 1.1;
    if (ambientLight < 0.0087) ambientLight = 0.0087;
    
    skyMaterial->ambient = ambientLight;
    
    Light* sunLight = directionalLight->GetComponent<Light>();
    sunLight->intensity = ambientLight * 0.87;
    
    // Light direction
    lightTransform = directionalLight->GetComponent<Transform>();
    
    if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
    if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
    
    
    
    
    
    
    
    
    
    
    
    if (cameraController == nullptr) 
        return;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    glm::vec3 force(0);
    if (mainCamera != nullptr) {
        
        // Directional movement
        if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward;}
        if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward;}
        if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right;}
        if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right;}
        
        // Elevation
        if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up;}
        if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up;}
        
        
        force *= 0.2;
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 3;
        
        if (force != glm::vec3(0)) 
            cameraController->AddForce(force.x, force.y, force.z);
        
        
        
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
    
    
    
    
    
    
    //
    // Pausing
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Platform.Pause();
        
        if (Platform.isPaused) {
            
            mainCamera->DisableMouseLook();
            
            Input.ClearKeys();
            
            Platform.ShowMouseCursor();
            
        } else {
            
            SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
            mainCamera->EnableMouseLook();
            
            Platform.HideMouseCursor();
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    
    
    return;
}


//
// Called once every tick (every 20 frames)
//

void TickUpdate(void) {
    
    
    
    return;
}





void Shutdown(void) {
    
    
    
    return;
}




