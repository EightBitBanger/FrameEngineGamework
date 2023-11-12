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
GameObject* overlayObject[4];

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
    
    overlayObject[0] = Engine.CreateOverlayTextRenderer("", 17, "font");
    overlayObject[1] = Engine.CreateOverlayTextRenderer("", 17, "font");
    overlayObject[2] = Engine.CreateOverlayTextRenderer("", 17, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[0]->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[1]->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[2]->GetComponent<MeshRenderer>() );
    overlayObject[0]->GetComponent<MeshRenderer>()->material->shader = UIShader;
    overlayObject[1]->GetComponent<MeshRenderer>()->material->shader = UIShader;
    overlayObject[2]->GetComponent<MeshRenderer>()->material->shader = UIShader;
    
    overlayObject[0]->transform.position += Vector3(0, -0.64,  -0.74);
    overlayObject[1]->transform.position += Vector3(0, -0.67,  -0.74);
    overlayObject[2]->transform.position += Vector3(0, -0.7,   -0.74);
    
    
    
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
    
    overlayObject[0]->GetComponent<Text>()->text = "x " + FloatToString(mainCamera->transform.position.x);
    overlayObject[1]->GetComponent<Text>()->text = "y " + FloatToString(mainCamera->transform.position.y);
    overlayObject[2]->GetComponent<Text>()->text = "z " + FloatToString(mainCamera->transform.position.z);
    
    
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



