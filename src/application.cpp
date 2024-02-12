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

// User plug-ins

#include <GameEngineFramework/Plugins/ChunkManager.h>
ChunkManager chunkManager;


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












// Application entry point
//

void Start() {
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    
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
    skyObject->GetComponent<Transform>()->parent = cameraTransform;
    
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
    
    chunkManager.generationDistance  = 400;
    chunkManager.destructionDistance = 400;
    
    chunkManager.doUpdateWithPlayerPosition = false;
    
    chunkManager.chunkSize = 128;
    
    
    
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
    
    text[11]->text = "Garbage game objects - " + Int.ToString( Engine.mGarbageObjects.size() );
    text[12]->text = "Garbage rigid bodies - " + Int.ToString( Engine.mGarbageRigidBodies.size() );
    
    
    
    
    
    
    
    
    
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




