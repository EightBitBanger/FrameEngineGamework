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
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

Text* text[20];







// Application entry point
//

void Start() {
    
    // Create the main world scene 
    Engine.sceneMain = Engine.Create<Scene>();
    
    // Add the scene to the render system
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);
    
    // Create a sky
    float skyFadeBias = 0.0001;
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.2);
    skyLow  += Colors.MakeGrayScale(0.6);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, skyFadeBias);
    
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->material;
    skyMaterial->diffuse = Color(0.087, 0.087, 0.087);
    skyMaterial->EnableDepthTest();
    
    
    // Create a camera controller
    Vector3 position = Vector3(-100, 0, 0);
    Vector3 colliderScale = Vector3(1, 8, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    // Attach the sky object to the camera controller
    skyObject->parent = cameraController;
    cameraController->DisableGravity();
    
    // Camera light
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* light = cameraController->GetComponent<Light>();
    light->color = Colors.MakeRandom();
    light->attenuation = 4;
    light->range = 300;
    light->intensity = 1000;
    
    Engine.sceneMain->AddLightToSceneRoot(light);
    
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
    
    int fontSize = 9;
    Color fontColor = Colors.black;
    
    
    // Initiate text elements
    for (int i=0; i < 10; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer("", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    
    Mesh* cubeMeshPtr = Resources.CreateMeshFromTag("cube");
    
    
    
    //
    // AI actor testing
    //
    
    float spawnArea = 1;
    float uniformScale = 0.1;
    
    for (int i=0; i < 1000; i++) {
        
        float xx = (Random.Range(0, 10) - Random.Range(0, 10)) * spawnArea;
        float yy = (Random.Range(0, 10) - Random.Range(0, 10)) * spawnArea;
        float zz = (Random.Range(0, 10) - Random.Range(0, 10)) * spawnArea;
        
        float scalexx = Random.Range(0, 10) * uniformScale;
        float scaleyy = Random.Range(0, 10) * uniformScale;
        float scalezz = Random.Range(0, 10) * uniformScale;
        
        GameObject* newActorObject = Engine.CreateAIActor(Vector3(xx, yy, zz), cubeMeshPtr);
        newActorObject->transform.scale = Vector3(scalexx, scaleyy, scalezz);
        newActorObject->DisableGravity();
        
        MeshRenderer* actorMeshRenderer = newActorObject->GetComponent<MeshRenderer>();
        actorMeshRenderer->material->shader = Engine.shaders.colorUnlit;
        actorMeshRenderer->material->ambient  = Colors.black;
        actorMeshRenderer->material->diffuse  = Colors.MakeRandom();
        actorMeshRenderer->material->diffuse *= Colors.dkgray;
        
        Actor* actor = newActorObject->GetComponent<Actor>();
        actor->SetChanceToMove( Random.Range(0, 2) );
        
    }
    
    
    
    
    
    
    
    
    return;
}







//
// Application loop
//

void Run() {
    
    text[1]->text = "Renderer - " + FloatToString( Engine.profileRenderSystem );
    text[2]->text = "Physics  - " + FloatToString( Engine.profilePhysicsSystem );
    text[3]->text = "Engine   - " + FloatToString( Engine.profileGameEngineUpdate );
    //text[4]->text = "ActorAI  - " + FloatToString( Engine.profileActorAI );
    
    
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
        
        
            force *= 2;
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 5;
        
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



