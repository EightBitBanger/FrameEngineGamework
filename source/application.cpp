// User application code space
//

#include "Engine/Engine.h"

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern RandomGen            Random;
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
Scene* sceneMain;
Scene* sceneOverlay;

GameObject*  cameraController;
Camera*      mainCamera;

Material* skyMaterial;

GameObject* plain;
GameObject* overlayObject;

Vector3 overlayPosition;


// Application entry point
//

void Framework::Start() {
    
    sceneMain = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(sceneMain);
    
    // Create some objects from the loaded resources
    Shader* surfaceShader = Resources.CreateShaderFromTag("surface");
    Shader* textureShader = Resources.CreateShaderFromTag("texture");
    
    
    //
    // Create a sky
    
    Color skyLow(Colors.ltgray);
    Color skyHigh(Colors.blue);
    
    GameObject* skyObject = Engine.CreateSky("sky", "surface", skyLow, skyHigh, 0.127);
    sceneMain->AddMeshRendererToSceneRoot( skyObject->GetComponent<MeshRenderer>() );
    
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->material;
    skyMaterial->diffuse = Color(0.0, 0.0, 0.0);
    
    
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(Vector3(0, 50, 0), Vector3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    mainCamera = cameraController->GetComponent<Camera>();
    sceneMain->camera = mainCamera;
    
    // Attach sky object to the camera object
    skyObject->parent = cameraController;
    
    
    
    
    plain = Engine.Create<GameObject>();
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>(Resources.CreateMeshFromTag("plain"), Engine.Create<Material>()) );
    plain->transform.scale = glm::vec3(1, 1, 1);
    
    Mesh*     plainMesh     = plain->GetComponent<MeshRenderer>()->mesh;
    Material* plainMaterial = plain->GetComponent<MeshRenderer>()->material;
    plainMaterial->diffuse = Colors.white;
    
    sceneMain->AddMeshRendererToSceneRoot( plain->GetComponent<MeshRenderer>() );
    
    
    
    SubMesh meshPart;
    plainMesh->CopySubMesh(0, meshPart);
    
    int width  = 100;
    int height = 100;
    
    for (int z=0; z < height; z++) {
        for (int x=0; x < width; x++) {
            
            float CoordMul = 1;
            float CoordX   = x * 0.1;
            float CoordZ   = z * 0.1;
            
            float noiseTotal = Random.Perlin(CoordX, 0, CoordZ) * CoordMul;
            
            plainMesh->AddSubMesh((float)x, noiseTotal * 10, z, meshPart, false);
            
            if (noiseTotal < 0)   noiseTotal = 0;
            if (noiseTotal > 0.9) noiseTotal = 0.9;
            
            plainMesh->ChangeSubMeshColor(plainMesh->GetSubMeshCount() - 1, Colors.Make(noiseTotal, noiseTotal, noiseTotal));
            
        }
    }
    
    plainMesh->UpdateMesh();
    
    
    // Randomize colors
    for (int i=0; i < plainMesh->GetSubMeshCount(); i++) 
        plainMesh->ChangeSubMeshColor(i, Colors.MakeRandom());
    
    
    plainMaterial->shader = surfaceShader;
    
    plainMaterial->SetTextureFiltration(MATERIAL_FILTER_LINEAR);
    plainMaterial->DisableCulling();
    
    Physics.SetWorldGravity(0, 0, 0);
    
    
    
    
    
    // Overlay example
    sceneOverlay = Engine.Create<Scene>();
    sceneOverlay->camera = Engine.Create<Camera>();
    //sceneOverlay->camera->useMouseLook = true;
    
    overlayObject = Engine.Create<GameObject>();
    overlayObject->transform.RotateAxis(90, Vector3(0, 0, 1));
    overlayObject->transform.scale = Vector3(0.1, 0.1, 0.1);
    
    Mesh*     overlayMesh     = Engine.Create<Mesh>();
    Material* overlayMaterial = Engine.Create<Material>();
    overlayMaterial->shader = surfaceShader;
    overlayObject->AddComponent( Engine.CreateComponent<MeshRenderer>(overlayMesh, overlayMaterial) );
    
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject->GetComponent<MeshRenderer>() );
    
    overlayMesh->AddPlainSubDivided(0, 0, 0, 1, 1, Colors.blue, 1, 1);
    overlayMesh->UpdateMesh();
    
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    return;
}










// Day cycle
float dayNightMaxLight = 0.1f;
float dayNightRate     = 0.0001f;





// Camera movement force
float cameraSpeed     = 1.5f;



//
// Application loop
//

void Framework::Run() {
    
    if (Input.CheckKeyCurrent(VK_I)) {overlayPosition.y += 0.2;}
    if (Input.CheckKeyCurrent(VK_K)) {overlayPosition.y -= 0.2;}
    if (Input.CheckKeyCurrent(VK_J)) {overlayPosition.z += 0.2;}
    if (Input.CheckKeyCurrent(VK_L)) {overlayPosition.z -= 0.2;}
    
    overlayPosition.x = 1.001;
    
    overlayObject->transform.position = overlayPosition;
    
    
    
    
    //
    // AI system currently being implemented
    //
    
    /*
    if (Input.CheckMouseLeftPressed()) {
        Input.ClearMouseLeft();
        
        GameObject* actorObject = Engine.CreateAIActor( (Renderer.GetCamera()->transform.position + (Renderer.GetCamera()->forward * 10.0f)) );
        
        Material* actorMaterial = actorObject->GetComponent<MeshRenderer>()->material;
        actorMaterial->ambient = Color(0, 0, 0);
        rp3d::Vector3 position;
        
        rp3d::Vector3 force;
        Camera* currentCamera = Renderer.GetCamera();
        force.x = currentCamera->forward.x;
        force.y = currentCamera->forward.y;
        force.z = currentCamera->forward.z;
        
        actorObject->GetComponent<RigidBody>()->applyLocalForceAtCenterOfMass(force);
        
        actorObject->AddComponent( Engine.CreateComponent( Components.Light ) );
        
        Light* lightPtr = actorObject->GetComponent<Light>();
        lightPtr->intensity   = 100.0f;
        lightPtr->range       = 100.0f;
        lightPtr->attenuation = 2.0f;
        
        //Gene newGene;
        
    }
    */
    
    
    
    
    
    
    //
    // Day night cycle testing
    float skyMax = 0.8;
    
    if (skyMaterial != nullptr) {
        if (skyMaterial->diffuse < Color(skyMax, skyMax, skyMax)) {
            
            Color skyShift(dayNightRate, dayNightRate, dayNightRate);
            skyMaterial->diffuse += skyShift;
            
            if (skyMaterial->diffuse > Color(dayNightMaxLight, dayNightMaxLight, dayNightMaxLight)) 
                skyMaterial->diffuse = Color(dayNightMaxLight, dayNightMaxLight, dayNightMaxLight);
            
        }
    }
    
    
    
    
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
        
        //if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 2;
        
        if (cameraController != nullptr) {
            force *= cameraSpeed;
            glm::vec3 velocity = cameraController->transform.GetPosition();
            velocity += force;
            cameraController->transform.SetPosition(velocity);
            
            RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
            
            rp3d::Transform transform = rp3d::Transform::identity();
            transform.setPosition( rp3d::Vector3(velocity.x, velocity.y, velocity.z) );
            
            
            rigidBody->setTransform( transform );
        }
        
    }
    
    
    /*
    if (Renderer.GetCamera() != nullptr) {
        
        Vector3 forwardOffset = Renderer.GetCamera()->forward;
        forwardOffset *= 100;
        
        plain->transform.position = velocity + forwardOffset;
        
    }
    */
    
    
    
    
    
    
    
    
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            mainCamera->DisableMouseLook();
            Input.ClearKeys();
            
            Application.ShowMouseCursor();
            
        } else {
            mainCamera->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            mainCamera->EnableMouseLook();
            
            Application.HideMouseCursor();
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    return;
}


//
// Called once every tick
//

void Framework::TickUpdate(void) {
    
    
    
    
    
    
    
    
}





void Framework::Shutdown(void) {
    
    return;
}



