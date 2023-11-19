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
    
    
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    skyHigh += Colors.MakeGrayScale(0.2);
    
    Color skyLow(Colors.blue);
    skyLow  += Colors.MakeGrayScale(0.6);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 0.0001);
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
    cameraLight->range       = 120;
    cameraLight->attenuation = 10;
    cameraLight->color       = Colors.white;
    
    
    
    
    GameObject* plain = Engine.Create<GameObject>();
    plain->transform.scale = Vector3(10, 10, 10);
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("plain"), Resources.CreateMaterialFromTag("grassy") ) );
    Mesh* plainMesh = plain->GetComponent<MeshRenderer>()->mesh;
    plain->GetComponent<MeshRenderer>()->material->shader = Engine.shaders.texture;
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
    //sceneOverlay->camera->isFixedAspect = true;
    sceneOverlay->camera->isOrthographic = true;
    //sceneOverlay->camera->transform.scale = Vector3(0.7, 1, 0.7);
    
    sceneOverlay->camera->transform.position = Vector3(0, 0, 0);
    
    sceneOverlay->camera->clipFar  =  100;
    sceneOverlay->camera->clipNear = -100;
    
    
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    // Create some text renderers
    overlayObject[0] = Engine.CreateOverlayTextRenderer("Render window", 15, Colors.black, "font");
    overlayObject[1] = Engine.CreateOverlayTextRenderer("", 12, Colors.black, "font");
    overlayObject[2] = Engine.CreateOverlayTextRenderer("", 12, Colors.black, "font");
    overlayObject[3] = Engine.CreateOverlayTextRenderer("", 12, Colors.green, "font");
    
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[0]->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[1]->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[2]->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[3]->GetComponent<MeshRenderer>() );
    
    Text* text[4];
    for (int i=0; i < 4; i++) 
        text[i] = overlayObject[i]->GetComponent<Text>();
    
    text[0]->color = Colors.red;
    text[1]->color = Colors.black;
    text[2]->color = Colors.black;
    text[3]->color = Colors.black;
    
    text[0]->sprite.canvas.position.x =  5;
    text[0]->sprite.canvas.position.y =  3;
    
    text[1]->sprite.canvas.position.x =  80;
    text[1]->sprite.canvas.position.y =  17;
    
    text[2]->sprite.canvas.position.x =  80;
    text[2]->sprite.canvas.position.y =  18;
    
    text[3]->sprite.canvas.position.x =  80;
    text[3]->sprite.canvas.position.y =  19;
    
    text[1]->sprite.canvas.anchorBottom = true;
    text[1]->sprite.canvas.anchorRight = true;
    text[2]->sprite.canvas.anchorBottom = true;
    text[2]->sprite.canvas.anchorRight = true;
    text[3]->sprite.canvas.anchorRight = true;
    text[3]->sprite.canvas.anchorBottom = true;
    
    
    
    // World physics
    
    Physics.SetWorldGravity(0, 0, 0);
    
    
    return;
}




// Camera movement force
float cameraSpeed     = 1.5f;



//
// Application loop
//

GameObject* test;

void Framework::Run() {
    
    test = Engine.Create<GameObject>();
    Engine.Destroy<GameObject>( test );
    
    // Update player position on screen
    overlayObject[1]->GetComponent<Text>()->text = "x " + FloatToString( mainCamera->transform.position.x );
    overlayObject[2]->GetComponent<Text>()->text = "y " + FloatToString( mainCamera->transform.position.y );
    overlayObject[3]->GetComponent<Text>()->text = "z " + FloatToString( mainCamera->transform.position.z );
    
    if (Input.CheckKeyCurrent(VK_I)) sceneOverlay->camera->transform.scale += 0.01;
    if (Input.CheckKeyCurrent(VK_K)) sceneOverlay->camera->transform.scale -= 0.01;
    
    
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
// Called once every tick
//

void Framework::TickUpdate(void) {
    
    
    
    
    
    
    
    
}





void Framework::Shutdown(void) {
    
    return;
}



