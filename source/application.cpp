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
    skyMaterial->diffuse = Color(0.1, 0.1, 0.1);
    skyMaterial->EnableDepthTest();
    
    
    
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(Vector3(0, 50, 0), Vector3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    mainCamera = cameraController->GetComponent<Camera>();
    sceneMain->camera = mainCamera;
    
    // Attach sky object to the camera object
    //skyObject->parent = cameraController;
    
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* cameraLight = cameraController->GetComponent<Light>();
    cameraLight->intensity = 100;
    
    
    
    GameObject* plain = Engine.Create<GameObject>();
    plain->transform.scale = Vector3(100, 0, 100);
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("plain"), Resources.CreateMaterialFromTag("grassy") ) );
    sceneMain->AddMeshRendererToSceneRoot( plain->GetComponent<MeshRenderer>() );
    
    plain->GetComponent<MeshRenderer>()->material->shader = textureShader;
    
    
    
    
    
    
    
    
    
    
    
    GameObject* barrel = Engine.Create<GameObject>();
    barrel->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("barrel"), Resources.CreateMaterialFromTag("barrel") ) );
    barrel->GetComponent<MeshRenderer>()->material->shader = textureShader;
    barrel->GetComponent<MeshRenderer>()->material->ambient = Colors.black;
    barrel->GetComponent<MeshRenderer>()->material->diffuse = Colors.black;
    
    barrel->GetComponent<MeshRenderer>()->mesh->ChangeSubMeshColor(0, Colors.MakeGrayScale(0.1));
    
    sceneMain->AddMeshRendererToSceneRoot( barrel->GetComponent<MeshRenderer>() );
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*
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
            
            // Causes invalid OpenGL command
            //plainMesh->ChangeSubMeshColor(plainMesh->GetSubMeshCount() - 1, Colors.Make(noiseTotal, noiseTotal, noiseTotal));
            
        }
    }
    
    plainMesh->UploadToGPU();
    
    
    */
    
    
    
    
    
    
    
    
    // Overlay example
    sceneOverlay = Engine.Create<Scene>();
    sceneOverlay->camera = Engine.Create<Camera>();
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    overlayObject = Engine.CreateOverlayRenderer();
    overlayObject->transform.scale = Vector3(0.1, 0.1, 0.1);
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject->GetComponent<MeshRenderer>() );
    
    overlayRenderer->mesh->AddPlain(0, 0, 0, 1, 1, Colors.white);
    overlayRenderer->mesh->UploadToGPU();
    
    Engine.Destroy<Mesh>( overlayRenderer->mesh );
    Engine.Destroy<Material>( overlayRenderer->material );
    
    overlayRenderer->mesh = Resources.CreateMeshFromTag("plain");
    overlayRenderer->material = Resources.CreateMaterialFromTag("cross");
    
    //overlayRenderer->material->diffuse = Colors.white;
    //overlayRenderer->material->ambient = Colors.white;
    overlayRenderer->material->shader = textureShader;
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayRenderer->material->DisableCulling();
    
    
    
    
    
    
    // World physics
    
    Physics.SetWorldGravity(0, 0, 0);
    
    
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



