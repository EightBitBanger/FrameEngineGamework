// User application code space
//

#include <GameEngineFramework/Engine/EngineSystems.h>


// User plug-ins

#include <GameEngineFramework/Plugins/ChunkManager.h>
ChunkManager chunkManager;


// User globals
GameObject*  cameraController;

Text* text[20];







// Day night cycle

bool cycleDirection = false;

float ambientLight = 0.3;

Material* skyMaterial;

Material* plainMaterial;










// Application entry point
//

void Start() {
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
    //Engine.EnableConsole();
    Engine.EnableConsoleBackPanel();
    Engine.EnableConsoleCloseOnReturn();
    
    
    
    //
    // Create a camera controller
    //
    
    // The position of the player in the world.
    Vector3 playerPosition = Vector3(0, 0, 0);
    
    // Create a new camera controller object
    cameraController = Engine.CreateCameraController(playerPosition);
    
    // Assign the camera controller's camera for rendering scene main.
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    // Use the mouse to look around.
    Engine.sceneMain->camera->EnableMouseLook();
    
    // Create a box collider for the player.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    // Add the collider to the camera controller game object.
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    
    
    //
    // Directional light
    //
    
    // Create a new game object.
    GameObject* lightObject = Engine.Create<GameObject>();
    
    // Add a new light component to the game object.
    lightObject->AddComponent( Engine.CreateComponent<Light>() );
    
    // Point the game object downward. This will cause 
    // the light component to point toward the ground.
    lightObject->GetComponent<Transform>()->RotateAxis(1, Vector3(0.0, -1.0, 0.0));
    
    // Get a pointer to the newly created light component.
    Light* directionalLight = lightObject->GetComponent<Light>();
    
    // Setup the parameters for a directional light
    directionalLight->type       = LIGHT_TYPE_DIRECTIONAL;
    directionalLight->intensity  = 0.3;
    directionalLight->color      = Colors.white;
    
    // Add the light to the main scene.
    Engine.sceneMain->AddLightToSceneRoot( directionalLight );
    
    
    //
    // Create a sky
    //
    
    // Sky colors from hight to low.
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    // Ammount of fade bias from the color "skyHigh" to "skyLow".
    float colorBias = 1.0f;
    
    // Sky mesh resource name.
    // Note: this mesh is loaded by the resource manager.
    std::string skyResourceName = "sky";
    
    // Generate the sky which will be returned as a game object.
    // This game object will contain a mesh renderer to draw the sky.
    GameObject* skyObject = Engine.CreateSky(skyResourceName, Colors.blue, Colors.blue, colorBias);
    
    // Add the sky's mesh renderer to the main scene.
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY );
    
    // Attach the sky object to the camera controller to prevent 
    // the player from moving outside of the sky.
    skyObject->GetComponent<Transform>()->parent = cameraController->GetComponent<Transform>();
    
    
    
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    // Initiate text elements
    for (int i=0; i < 20; i++) {
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.white, "font");
        
        Engine.sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
    }
    
    
    
    
    
    // Setup chunk generation
    chunkManager.generationDistance  = 800;
    chunkManager.destructionDistance = 800;
    
    chunkManager.renderDistance = 0.7;
    
    chunkManager.doUpdateWithPlayerPosition = false;
    
    chunkManager.chunkSize = 64;
    
    chunkManager.actorsPerChunk = 0;
    
    // Chunk material
    Material* chunkMaterial = Engine.Create<Material>();
    
    chunkMaterial->shader = Engine.shaders.color;
    chunkMaterial->isShared = true;
    
    chunkManager.SetMaterial( chunkMaterial );
    
    
    
    
    plainMaterial = chunkMaterial;
    
    //GameObject* panelObject = Engine.CreateOverlayPanelRenderer(300, 250, 100, 100, "panel_blue");
    //MeshRenderer* panelRenderer = panelObject->GetComponent<MeshRenderer>();
    //Engine.sceneOverlay->AddMeshRendererToSceneRoot( panelRenderer );
    
    
    
    
    //
    // Random objects in the sky
    //
    
    Material* newMaterial = Engine.Create<Material>();
    newMaterial->isShared = true;
    
    newMaterial->shader = Engine.shaders.color;
    newMaterial->EnableShadowVolumePass();
    
    Color shadowColor = Colors.yellow;
    shadowColor *= Colors.white;
    shadowColor *= Colors.white;
    shadowColor *= Colors.white;
    
    newMaterial->SetShadowVolumeColor( shadowColor );
    
    newMaterial->SetShadowVolumeColorIntensity( 4 );
    
    newMaterial->SetShadowVolumeIntensityHigh( 0.7 );
    newMaterial->SetShadowVolumeIntensityLow( 0.1 );
    newMaterial->SetShadowVolumeLength(24);
    newMaterial->SetShadowVolumeAngleOfView(24);
    
    
    for (unsigned int i=0; i < 1000; i++) {
        
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * 2;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * 2;
        
        float sxx = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.1;
        float szz = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.1;
        
        GameObject* newObject = Engine.Create<GameObject>();
        Transform* transform = newObject->GetComponent<Transform>();
        
        transform->position.x = xx;
        transform->position.y = 80 + Random.Range(0, 10);
        transform->position.z = zz;
        
        //transform->scale.x = sxx;
        //transform->scale.z = szz;
        
        newObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        MeshRenderer* meshRenderer = newObject->GetComponent<MeshRenderer>();
        
        meshRenderer->mesh     = Engine.meshes.cube;
        meshRenderer->material = newMaterial;
        
        Engine.sceneMain->AddMeshRendererToSceneRoot( meshRenderer );
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

GameObject* actorObject;

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
    
    //text[11]->text = "Garbage game objects - " + Int.ToString( Engine.mGarbageGameObjects.size() );
    //text[12]->text = "Garbage rigid bodies - " + Int.ToString( Engine.mGarbageRigidBodies.size() );
    //text[13]->text = "Clean rigid bodies --- " + Int.ToString( Engine.mFreeRigidBodies.size() );
    
    
    
    
    
    
    
    
    //if (actorObject != nullptr) 
    //    Engine.Destroy<GameObject>( actorObject );
    
    //actorObject = Engine.CreateAIActor( Vector3(0, 0, 0) );
    
    
    
    
    
    
    
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
    
    
    //Light* sunLight = directionalLight->GetComponent<Light>();
    //sunLight->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);;
    
    // Light direction
    //lightTransform = directionalLight->GetComponent<Transform>();
    
    
    
    //
    // DEBUG - Manually adjust light direction
    //
    
    //if (!Platform.isPaused) {
    //    if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
    //    if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
    //}
    
    
    
    
    
    
    
    
    
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




