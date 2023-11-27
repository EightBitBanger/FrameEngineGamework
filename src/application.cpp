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
    
    // Create the main world scene and add it to the render queue
    Engine.sceneMain = Engine.Create<Scene>();
    
    // Add the scene to the renderer
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
    cameraController = Engine.CreateCameraController(Vector3(0, 0, 0), Vector3(1, 8, 1));
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    Engine.sceneMain->camera->DisableMouseLook();
    Engine.sceneMain->camera->isFixedAspect = true;
    
    Engine.sceneMain->camera->viewport.w = Renderer.viewport.w;
    Engine.sceneMain->camera->viewport.h = Renderer.viewport.h;
    
    cameraController->DisableGravity();
    
    // Attach sky object to the camera object
    skyObject->parent = cameraController;
    
    
    
    
    
    
    
    
    
    
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
    Color fontColor = Colors.MakeGrayScale(0.9);
    
    
    
    
    //
    // Panel UI
    //
    
    //
    // Left panel
    //
    
    GameObject* panelBase = Engine.CreateOverlayPanelRenderer(110, 10000, "panel_blue");
    Panel* panelBaseOverlay = panelBase->GetComponent<Panel>();
    
    panelBaseOverlay->canvas.anchorCenterVert = true;
    
    
    //
    // Top panel
    //
    
    GameObject* panelTitleBase = Engine.CreateOverlayPanelRenderer(10000, 80, "panel_gray");
    Panel* panelTitleOverlay = panelTitleBase->GetComponent<Panel>();
    
    panelTitleOverlay->canvas.anchorCenterHorz = true;
    panelTitleOverlay->canvas.anchorTop = true;
    
    
    // Panel layer
    sceneOverlay->AddMeshRendererToSceneRoot( panelTitleBase->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( panelBase->GetComponent<MeshRenderer>() );
    
    
    // Output text elements
    for (int i=0; i < 20; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer("", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        text[i]->canvas.y = 2 * i + 2;
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return;
}






//
// Application loop
//

void Run() {
    
    text[0]->text = "Mouse X " + IntToString( Input.mouseX );
    text[1]->text = "Mouse Y " + IntToString( Input.mouseY );
    
    if (Input.CheckKeyCurrent(VK_I)) Engine.sceneMain->camera->viewport.x += 10;
    if (Input.CheckKeyCurrent(VK_K)) Engine.sceneMain->camera->viewport.x -= 10;
    if (Input.CheckKeyCurrent(VK_J)) Engine.sceneMain->camera->viewport.y += 10;
    if (Input.CheckKeyCurrent(VK_L)) Engine.sceneMain->camera->viewport.y -= 10;
    
    
    float viewScale = 0;
    if (Input.CheckKeyCurrent(VK_R)) viewScale =  1;
    if (Input.CheckKeyCurrent(VK_F)) viewScale = -1;
    
    Engine.sceneMain->camera->viewport.w += viewScale;
    Engine.sceneMain->camera->viewport.h += viewScale;
    
    //
    // Mouse checking
    //
    
    int buttonX = 0;
    int buttonY = 0;
    
    int buttonW = 100;
    int buttonH = 50;
    
    int windowMouseX = Input.mouseX - Renderer.viewport.x;
    int windowMouseY = Input.mouseY - Renderer.viewport.y;
    
    text[2]->text = "";
    
    bool isClicked = false;
    bool isHovered = false;
    
    // Check hovered
    if ((windowMouseX > buttonX) & (windowMouseX < buttonX + buttonW) & 
        (windowMouseY > buttonY) & (windowMouseY < buttonY + buttonH)) {
        isHovered = true;
        
        // Check clicked
        if (Input.CheckMouseLeftPressed()) 
            isClicked = true;
    }
    
    
    
    if (isClicked) 
        text[2]->text = "Clicked";
    
    
    
    
    
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



