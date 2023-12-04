// User application code space
//

#include <GameEngineFramework/Engine/Engine.h>

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern RandomGen            Random;
extern Logger               Log;
extern ProfilerTimer        Profiler;
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







Text* titleText;

GameObject* panelLeft;
Button* buttonExample;

unsigned int mouseOldX = 0;
unsigned int mouseOldY = 0;

bool isMouseDragging = false;


void callbackButtonA(void) {text[5]->text = "ButtonA";}
void callbackButtonB(void) {text[6]->text = "ButtonB";}
void callbackButtonC(void) {text[7]->text = "ButtonC";}
void callbackButtonD(void) {text[8]->text = "ButtonB";}



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
    // Panel testing
    //
    
    
    
    
    //
    // Top bar panel
    //
    
    GameObject* panelTitle = Engine.CreateOverlayPanelRenderer(-50, -1, 10000, 70, "panel_gray");
    Panel* panelTitleOverlay = panelTitle->GetComponent<Panel>();
    
    //panelTitleOverlay->canvas.anchorCenterHorz = true;
    panelTitleOverlay->canvas.anchorTop = true;
    
    
    // Title bar menu items
    GameObject* textObject = Engine.CreateOverlayTextRenderer(1, 1, "File  Edit  Project  Settings", fontSize, Colors.green, "font");
    
    titleText = textObject->GetComponent<Text>();
    titleText->canvas.anchorTop = true;
    
    
    
    
    //
    // Mouse drag test panel
    //
    
    int buttonX = 200;
    int buttonY = 200;
    
    int buttonW = 32;
    int buttonH = 32;
    
    panelLeft = Engine.CreateOverlayPanelRenderer(buttonX, buttonY, buttonW, buttonH, "panel_blue");
    Panel* panelBaseOverlay = panelLeft->GetComponent<Panel>();
    
    //panelBaseOverlay->canvas.anchorCenterVert = true;
    panelBaseOverlay->canvas.anchorTop = true;
    
    // Button callback test
    buttonExample = Engine.CreateOverlayButtonCallback(buttonX, 
                                                       buttonY, 
                                                       buttonW, 
                                                       buttonH, 
                                                       callbackButtonA);
    
    buttonExample->triggerOnLeftButton = true;
    buttonExample->triggerOnPressed    = true;
    
    
    
    
    sceneOverlay->AddMeshRendererToSceneRoot( panelLeft->GetComponent<MeshRenderer>() );   // Left panel
    sceneOverlay->AddMeshRendererToSceneRoot( panelTitle->GetComponent<MeshRenderer>() );  // Title bar
    sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );  // Title text
    
    return;
}










//
// Application loop
//

void Run() {
    
    if (Input.CheckKeyPressed(VK_I)) titleText->glyphWidth  += 0.01;
    if (Input.CheckKeyPressed(VK_K)) titleText->glyphWidth  -= 0.01;
    
    if (Input.CheckKeyPressed(VK_O)) titleText->glyphHeight += 0.01;
    if (Input.CheckKeyPressed(VK_L)) titleText->glyphHeight -= 0.01;
    
    text[15]->text = "text width  - " + FloatToString(titleText->glyphWidth);
    text[16]->text = "text height - " + FloatToString(titleText->glyphHeight);
    
    //
    // Mouse drag testing
    //
    
    
    // Check mouse down
    if (Input.CheckMouseLeftPressed() & (buttonExample->isHovering)) {
        
        // Set current position when mouse is clicked
        mouseOldX = Input.mouseX;
        mouseOldY = Input.mouseY;
        
        isMouseDragging = true;
    }
    
    // Check mouse up
    if (Input.CheckMouseLeftReleased()) {
        
        // Reset dragging state
        isMouseDragging = false;
    }
    
    // Drag the panel object
    if (isMouseDragging) {
        
        Panel* panelObject = panelLeft->GetComponent<Panel>();
        
        // Update panel renderer position
        panelObject->canvas.x -= mouseOldY - Input.mouseY;
        panelObject->canvas.y -= mouseOldX - Input.mouseX;
        
        // Update button callback position
        buttonExample->x -= mouseOldX - Input.mouseX;
        buttonExample->y -= mouseOldY - Input.mouseY;
        
        mouseOldX = Input.mouseX;
        mouseOldY = Input.mouseY;
        
    }
    
    
    
    
    // Initiate windows area
    
    text[2]->text = "window x - " + IntToString(Application.windowLeft);
    text[3]->text = "window y - " + IntToString(Application.windowTop);
    
    text[5]->text = "Renderer - " + FloatToString( Profiler.profileRenderSystem );
    text[6]->text = "Physics  - " + FloatToString( Profiler.profilePhysicsSystem );
    text[7]->text = "Engine   - " + FloatToString( Profiler.profileGameEngineUpdate );
    text[8]->text = "ActorAI  - " + FloatToString( Profiler.profileActorAI );
    
    text[10]->text = "mouse x - " + FloatToString( Application.windowLeft + Input.mouseX );
    text[11]->text = "mouse y - " + FloatToString( Application.windowTop + Input.mouseY );
    
    if (buttonExample->isHovering) {
        text[13]->text = "Hovering";
        text[13]->color = Colors.red;
        
        panelLeft->GetComponent<MeshRenderer>()->material->shader = Engine.shaders.texture;
        
    } else {
        text[13]->text = "Not hovering";
        text[13]->color = Colors.black;
        
        panelLeft->GetComponent<MeshRenderer>()->material->shader = Engine.shaders.color;
        
    }
    
    
    
    
    
    
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



