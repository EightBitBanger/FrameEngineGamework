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

MeshRenderer* overlayRenderer;
Vector3 overlayPosition;


// Application entry point
//

void Framework::Start() {
    
    sceneMain = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(sceneMain);
    
    // Create some objects from the loaded resources
    Shader* colorShader   = Resources.CreateShaderFromTag("color");
    Shader* textureShader = Resources.CreateShaderFromTag("texture");
    Shader* textureUnlit  = Resources.CreateShaderFromTag("textureUnlit");
    Shader* UIShader      = Resources.CreateShaderFromTag("UI");
    
    
    //
    // Create a sky
    
    Color skyLow(Colors.black);
    Color skyHigh(Colors.blue);
    
    GameObject* skyObject = Engine.CreateSky("sky", "color", skyLow, skyHigh, 0.0001);
    sceneMain->AddMeshRendererToSceneRoot( skyObject->GetComponent<MeshRenderer>() );
    
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->material;
    skyMaterial->diffuse = Color(0.087, 0.087, 0.087);
    skyMaterial->EnableDepthTest();
    
    
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(Vector3(0, 50, 0), Vector3(1, 8, 1));
    cameraController->transform.position = glm::vec3(0);
    mainCamera = cameraController->GetComponent<Camera>();
    sceneMain->camera = mainCamera;
    cameraController->DisableGravity();
    
    // Attach sky object to the camera object
    skyObject->parent = cameraController;
    
    
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* cameraLight = cameraController->GetComponent<Light>();
    sceneMain->AddLightToSceneRoot(cameraLight);
    cameraLight->intensity   = 10000;
    cameraLight->range       = 80;
    cameraLight->attenuation = 10;
    cameraLight->color       = Colors.red;
    
    
    
    
    GameObject* plain = Engine.Create<GameObject>();
    plain->transform.scale = Vector3(10, 10, 10);
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("plain"), Resources.CreateMaterialFromTag("grassy") ) );
    Mesh* plainMesh = plain->GetComponent<MeshRenderer>()->mesh;
    plain->GetComponent<MeshRenderer>()->material->shader = textureShader;
    plain->GetComponent<MeshRenderer>()->material->ambient = Colors.black;
    plain->GetComponent<MeshRenderer>()->material->DisableCulling();
    
    sceneMain->AddMeshRendererToSceneRoot( plain->GetComponent<MeshRenderer>() );
    
    
    
    
    SubMesh meshPart;
    plainMesh->CopySubMesh(0, meshPart);
    
    Engine.Destroy<Mesh>( plainMesh );
    plainMesh = Engine.Create<Mesh>();
    
    float noiseMul = 3;
    float xNoise = 0.1;
    float zNoise = 0.1;
    
    int width  = 100;
    int height = 100;
    
    for (int z=0; z < height; z++) {
        for (int x=0; x < width; x++) {
            
            float CoordX = x * xNoise;
            float CoordZ = z * zNoise;
            
            float noiseTotal = Random.Perlin(CoordX, 0, CoordZ) * noiseMul;
            
            plainMesh->AddSubMesh((float)x, noiseTotal, z, meshPart, false);
            
            if (noiseTotal < 0)   noiseTotal = 0;
            if (noiseTotal > 0.9) noiseTotal = 0.9;
            
            noiseTotal *= 0.3;
            
            plainMesh->ChangeSubMeshColor(plainMesh->GetSubMeshCount() - 1, Colors.Make(noiseTotal, noiseTotal, noiseTotal));
            
        }
    }
    
    plainMesh->UploadToGPU();
    
    
    
    
    
    
    
    // Overlay example
    sceneOverlay = Engine.Create<Scene>();
    sceneOverlay->camera = Engine.Create<Camera>();
    sceneOverlay->camera->isFixedAspect = true;
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    
    overlayObject = Engine.CreateOverlayRenderer();
    overlayObject->AddComponent( Engine.CreateComponent<Text>() );
    
    float textSize = 1.5;
    overlayObject->transform.position += Vector3(0, -0.5, -0.7);
    overlayObject->transform.scale = Vector3(0.01 * textSize, 1, 0.011 * textSize);
    
    overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot( overlayRenderer );
    
    Engine.Destroy<Material>( overlayRenderer->material );
    overlayRenderer->material = Resources.CreateMaterialFromTag("font");
    overlayRenderer->material->shader   = UIShader;
    overlayRenderer->material->ambient  = Colors.green;
    
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayRenderer->material->SetTextureFiltration(MATERIAL_FILTER_NONE);
    
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
    
    // Update player position on screen
    
    overlayObject->GetComponent<Text>()->text = "some string";
    
    
    
    
    
    if (Input.CheckKeyCurrent(VK_I)) {overlayObject->transform.position.y += 0.1;}
    if (Input.CheckKeyCurrent(VK_K)) {overlayObject->transform.position.y -= 0.1;}
    
    std::cout << FloatToString(overlayObject->transform.position.y) << std::endl;
    
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



