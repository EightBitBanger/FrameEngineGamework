// User application code space
//

#include <GameEngineFramework/Engine/Engine.h>


// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

Text* text[20];







Text* titleText;

Button* buttonExample;

void callbackButtonA(void) {text[15]->text = "ButtonA";}
void callbackButtonB(void) {text[16]->text = "ButtonB";}
void callbackButtonC(void) {text[17]->text = "ButtonC";}
void callbackButtonD(void) {text[18]->text = "ButtonB";}



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
    Engine.sceneMain->camera->DisableMouseLook();
    
    
    
    
    //
    // Initiate overlay scene
    //
    
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
    
    
    
    
    // Initiate left panel text elements
    for (int i=0; i < 20; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 2 * i + 4, "", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
    }
    
    
    
    
    
    //
    // Top bar panel
    //
    
    GameObject* panelTitle = Engine.CreateOverlayPanelRenderer(-50, -1, 10000, 110, "panel_gray");
    Panel* panelTitleOverlay = panelTitle->GetComponent<Panel>();
    
    panelTitleOverlay->canvas.anchorTop = true;
    
    
    // Title bar menu
    GameObject* textObject = Engine.CreateOverlayTextRenderer(1, 1, "File  Edit Project", fontSize, Colors.white, "font");
    
    titleText = textObject->GetComponent<Text>();
    titleText->canvas.anchorTop = true;
    
    
    
    
    
    //
    // Mouse drag test panel
    //
    
    buttonExample = Engine.CreateOverlayButtonCallback(15, 34, 45, 17, callbackButtonA);
    
    buttonExample->triggerOnLeftButton = true;
    buttonExample->triggerOnPressed    = true;
    
    
    
    sceneOverlay->AddMeshRendererToSceneRoot( panelTitle->GetComponent<MeshRenderer>() );  // Title bar
    sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );  // Title text
    
    return;
}










//
// Application loop
//

void Run() {
    
    
    // Initiate windows area
    
    text[2]->text = "window x - " + IntToString(Application.windowLeft);
    text[3]->text = "window y - " + IntToString(Application.windowTop);
    
    text[5]->text = "Renderer - " + FloatToString( Profiler.profileRenderSystem );
    text[6]->text = "Physics  - " + FloatToString( Profiler.profilePhysicsSystem );
    text[7]->text = "Engine   - " + FloatToString( Profiler.profileGameEngineUpdate );
    text[8]->text = "ActorAI  - " + FloatToString( Profiler.profileActorAI );
    
    text[10]->text = "mouse x - " + FloatToString( Input.mouseX - Application.windowLeft );
    text[11]->text = "mouse y - " + FloatToString( Input.mouseY - Application.windowTop );
    
    text[15]->text = "";
    text[16]->text = "";
    text[17]->text = "";
    text[18]->text = "";
    
    
    
    //
    // Camera controller input handling
    
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



