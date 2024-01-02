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
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;



// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

GameObject* directionalLight;

Text* text[20];






// Application entry point
//

void Start() {
    
    // Create the main world scene 
    Engine.sceneMain   = Engine.Create<Scene>();
    
    
    //
    // Create a sky
    float skyBrightness = 1.3;
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.997);
    skyLow  += Colors.MakeGrayScale(0.799);
    
    skyHigh *= Colors.MakeGrayScale(0.961);
    
    skyHigh *= Colors.MakeGrayScale( skyBrightness );
    skyLow  *= Colors.MakeGrayScale( skyBrightness );
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Color(0.087, 0.087, 0.087);
    skyMaterial->EnableDepthTest();
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(-13, 8, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    // Attach the sky object to the camera controller
    skyObject->GetComponent<Transform>()->parent = cameraController->GetComponent<Transform>();
    cameraController->DisableGravity();
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 3 );
    cameraController->SetMass( 10 );
    
    
    
    
    
    
    
    //
    // Shadow caster experimentation
    //
    
    
    // Base object
    GameObject* shadowObject = Engine.Create<GameObject>();
    Transform* transformObject = shadowObject->GetComponent<Transform>();
    transformObject->position.y += 5;
    
    shadowObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* objectRenderer = shadowObject->GetComponent<MeshRenderer>();
    
    objectRenderer->mesh     = Resources.CreateMeshFromTag("cube");
    objectRenderer->material = Engine.Create<Material>();
    objectRenderer->material->shader = Engine.shaders.color;
    objectRenderer->material->ambient = Color(0.01, 0.01, 0.01);
    objectRenderer->material->diffuse = Color(0.01, 0.01, 0.01);
    
    
    
    
    // Shadow caster object
    
    GameObject* shadowCaster = Engine.Create<GameObject>();
    Transform* transform = shadowCaster->GetComponent<Transform>();
    transform->position.y += 5;
    
    shadowCaster->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* shadowCasterRenderer = shadowCaster->GetComponent<MeshRenderer>();
    
    shadowCasterRenderer->mesh     = Resources.CreateMeshFromTag("cube");
    shadowCasterRenderer->material = Engine.Create<Material>();
    shadowCasterRenderer->material->ambient = Color(0.01, 0.01, 0.01);
    shadowCasterRenderer->material->diffuse = Color(0.01, 0.01, 0.01);
    
    shadowCasterRenderer->material->EnableBlending();
    shadowCasterRenderer->material->SetBlendingAlpha(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    
    shadowCasterRenderer->material->shader = Engine.shaders.shadowCaster;
    
    
    
    
    
    // Directional light
    directionalLight = Engine.Create<GameObject>();
    Transform* lightTransform = directionalLight->GetComponent<Transform>();
    lightTransform->RotateAxis(1, Vector3(0, -1, 0));
    
    directionalLight->AddComponent( Engine.CreateComponent<Light>() );
    Light* sunLight = directionalLight->GetComponent<Light>();
    
    Engine.sceneMain->AddLightToSceneRoot( sunLight );
    
    sunLight->intensity  = 0.87;
    sunLight->type       = LIGHT_TYPE_DIRECTIONAL;
    sunLight->color      = Colors.white;
    
    
    
    
    
    
    
    
    
    // Scene overlay
    sceneOverlay = Engine.Create<Scene>();
    
    sceneOverlay->camera = Engine.Create<Camera>();
    sceneOverlay->camera->isOrthographic = true;
    sceneOverlay->camera->isFixedAspect  = true;
    
    sceneOverlay->camera->viewport.w = Renderer.displaySize.x;
    sceneOverlay->camera->viewport.h = Renderer.displaySize.y;
    
    sceneOverlay->camera->clipFar  =  100;
    sceneOverlay->camera->clipNear = -100;
    
    // Initiate text elements
    for (int i=0; i < 10; i++) {
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
    
    // Base chunk mesh
    Mesh* plainBaseMesh = Resources.CreateMeshFromTag("chunk");
    SubMesh chunkSubMesh;
    plainBaseMesh->CopySubMesh(0, chunkSubMesh);
    
    // Chunk material
    Material* plainMaterial = Resources.CreateMaterialFromTag("grassy");
    plainMaterial->shader = Engine.shaders.texture;
    plainMaterial->diffuse = Colors.MakeGrayScale(0.001);
    
    // Chunk layout
    
    int chunkSize   = 50;
    
    int worldWidth  = 20;
    int worldHeight = 20;
    
    // Chunk mesh
    Mesh* chunkMesh = Engine.Create<Mesh>();
    
    // Chunk object
    GameObject* plainObject = Engine.Create<GameObject>();
    plainObject->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, plainMaterial ) );
    plainObject->AddComponent( Engine.CreateComponent<RigidBody>() );
    
    MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
    
    plainObject->SetAngularAxisLockFactor(0, 0, 0);
    plainObject->SetLinearAxisLockFactor(0, 0, 0);
    plainObject->SetStatic();
    
    // Transform the chunk
    Transform* chunkTransform = plainObject->GetComponent<Transform>();
    chunkTransform->localScale = Vector3(chunkSize * 0.013, 1, chunkSize * 0.013);
    
    
    for (int z=0; z < worldHeight; z++) {
        
        float chunkZ = (z * chunkSize) - ((worldHeight / 2) * chunkSize);
        
        for (int x=0; x < worldWidth; x++) {
            
            float chunkX = (x * chunkSize) - ((worldWidth  / 2) * chunkSize);
            
            chunkMesh->AddSubMesh(chunkX, 0, chunkZ, chunkSubMesh, false);
            
            // Collider
            BoxShape* plainCollider = Physics.CreateColliderBox(chunkSize, 100, chunkSize);
            plainObject->AddColliderBox(plainCollider, chunkX, -100, chunkZ);
            
            continue;
        }
        
        continue;
    }
    
    chunkMesh->UploadToGPU();
    
    
    
    
    
    
    // Add scenes to root node
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( shadowCasterRenderer );
    Engine.sceneMain->AddMeshRendererToSceneRoot( objectRenderer );
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer );
    Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer );
    
    
    Renderer.AddSceneToRenderQueue(sceneOverlay);                         // UI
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);                     // Main scene
    
    return;
}







//
// Application loop
//
glm::vec3 sunDir(0, 0, 1);
float sunStep = 0;
float sunRate = 1;

float shadowLength   = 10;
float shadowRayScale = 0.997;


void Run() {
    
    
    
    
    //
    // Shadow experimentation
    //
    
    
    
    Transform* lightTransform = directionalLight->GetComponent<Transform>();
    
    lightTransform->SetIdentity();
    //lightTransform->RotateAxis( 90, glm::vec3(0, 1, 0) );
    lightTransform->RotateAxis(1, Vector3(0, -1, 0));
    
    //lightTransform->RotateAxis(45, sunDir);
    
    lightTransform->RotateAxis(20, sunDir);
    
    sunStep = 45;
    
    //if (sunStep > 360) sunStep = 0;
    
    //text[1]->text = Int.ToString( sunStep );
    
    
    
    
    
    Renderer.mShadowTransform.SetIdentity();
    
    // Correct the sun cycle rotation
    Renderer.mShadowTransform.RotateAxis( 90, glm::vec3(0, 1, 0) );
    
    // Rotate by the inverse sun angle
    //Renderer.mShadowTransform.RotateAxis( -sunStep, sunDir );
    Renderer.mShadowTransform.RotateWorldAxis( -sunStep, sunDir, glm::vec3(0, -0.2, 0) );
    
    // Scale the length of the shadow
    Renderer.mShadowTransform.Scale( glm::vec3(shadowRayScale, shadowLength * 2, shadowRayScale) );
    
    // Offset by half the distance
    Renderer.mShadowTransform.Translate( glm::vec3(0, -1, 0) * 0.5f );
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    //text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    //text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    //text[5]->text = "x - " + Float.ToString( cameraController->GetComponent<Transform>()->position.x );
    //text[6]->text = "y - " + Float.ToString( cameraController->GetComponent<Transform>()->position.y );
    //text[7]->text = "z - " + Float.ToString( cameraController->GetComponent<Transform>()->position.z );
    
    
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    /*
    
    Transform* transform = directionalLight->GetComponent<Transform>();
    Light* light = directionalLight->GetComponent<Light>();
    
    //transform->RotateAxis(-10, Vector3(1, 0, 0));
    
    transform->RotateEuler(0.001, 0, 0);
    
    Vector3 direction = transform->EulerAngles();
    
    if ((direction.x > -90) & (direction.x < 90)) {
        
        light->isActive = true;
        
    } else {
        
        light->isActive = false;
        transform->SetIdentity();
        
        transform->RotateEuler(-90, 0, 0);
        
    }
    
    Vector3 direction = transform->EulerAngles();
    text[8]->text  = Float.ToString( direction.x );
    text[9]->text  = Float.ToString( direction.y );
    text[10]->text = Float.ToString( direction.z );
    
    */
    
    
    
    
    
    
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
        
        
        force *= 0.3;
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 5;
        
        if (force != glm::vec3(0)) 
            cameraController->AddForce(force.x, force.y, force.z);
        
    }
    
    
    
    
    
    
    //
    // Pausing
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            mainCamera->DisableMouseLook();
            
            Input.ClearKeys();
            
            Application.ShowMouseCursor();
            
        } else {
            
            SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
            mainCamera->EnableMouseLook();
            
            Application.HideMouseCursor();
            
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



