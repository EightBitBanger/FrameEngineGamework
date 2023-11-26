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

Text* positionX;
Text* positionY;
Text* positionZ;
Text* positionW;


GameObject* panelObject;
Text*       textObject;



// Application entry point
//

void Start() {
    
    // Create the main world scene and add it to the render queue
    Engine.sceneMain = Engine.Create<Scene>();
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
    Color fontColor = Colors.MakeGrayScale(0.09);
    
    
    
    //
    // Panel UI
    //
    
    
    panelObject = Engine.CreateOverlayPanelRenderer(10, 10, "panel");
    textObject  = Engine.CreateOverlayTextRenderer("", 9, Colors.red, "panel");
    
    sceneOverlay->AddMeshRendererToSceneRoot( panelObject->GetComponent<MeshRenderer>() );
    sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
    
    
    
    panelOverlay  = panelObject->parent->GetComponent<Panel>();
    
    panelOverlay->canvas.anchorCenterHorz = true;
    panelOverlay->canvas.anchorCenterVert = true;
    
    
    
    
    textOverlay   = panelObject->GetComponent<Text>();
    
    textOverlay->canvas.anchorCenterHorz = true;
    textOverlay->canvas.anchorCenterVert = true;
    
    
    
    
    
    
    
    
    
    
    
    // Window banner
    
    Color bannerColor = Colors.blue;
    GameObject* bannerObject = Engine.CreateOverlayTextRenderer("Window renderer", 9, bannerColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    
    Text* bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.x = 10;
    bannerText->canvas.y = 10;
    bannerText->canvas.anchorTop = true;
    
    
    
    
    
    
    
    // Outputs
    
    GameObject* textObjectA = Engine.CreateOverlayTextRenderer("", fontSize, Colors.black, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( textObjectA->GetComponent<MeshRenderer>() );
    positionX = textObjectA->GetComponent<Text>();
    positionX->canvas.anchorTop = true;
    positionX->canvas.y = 5;
    
    GameObject* textObjectB = Engine.CreateOverlayTextRenderer("", fontSize, Colors.black, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( textObjectB->GetComponent<MeshRenderer>() );
    positionY = textObjectB->GetComponent<Text>();
    positionY->canvas.anchorTop = true;
    positionY->canvas.y = 6;
    
    GameObject* textObjectC = Engine.CreateOverlayTextRenderer("ass", fontSize, Colors.black, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( textObjectC->GetComponent<MeshRenderer>() );
    positionZ = textObjectC->GetComponent<Text>();
    positionZ->canvas.anchorTop = true;
    positionZ->canvas.y = 7;
    
    GameObject* textObjectD = Engine.CreateOverlayTextRenderer("twat", fontSize, Colors.black, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( textObjectD->GetComponent<MeshRenderer>() );
    positionW = textObjectD->GetComponent<Text>();
    positionW->canvas.anchorTop = true;
    positionW->canvas.y = 8;
    
    
    
    
    
    
    return;
}






//
// Application loop
//

unsigned long long int numberWang = 0;

void Run() {
    
    numberWang++;
    positionX->text = "Numberwang " + IntToString( numberWang );
    
    
    
    
    //
    // Mouse checking
    //
    
    int buttonX = 30;
    int buttonY = 30;
    
    int buttonW = 50;
    int buttonH = 50;
    
    int windowMouseX = Input.mouseX - Renderer.viewport.x;
    int windowMouseY = Input.mouseY - Renderer.viewport.y;
    
    positionZ->text = "";
    
    if ((windowMouseX > buttonX) & (windowMouseX < buttonX + buttonW) & 
        (windowMouseY > buttonY) & (windowMouseY < buttonY + buttonH)) 
        positionZ->text = "hovered";
    
    
    
    
    MeshRenderer* panelRenderer = panelObject->GetComponent<MeshRenderer>();
    
    if (Input.CheckKeyCurrent(VK_I)) panelOverlay->canvas.x -= amount;
    if (Input.CheckKeyCurrent(VK_K)) panelOverlay->canvas.x += amount;
    if (Input.CheckKeyCurrent(VK_J)) panelOverlay->canvas.y -= amount;
    if (Input.CheckKeyCurrent(VK_L)) panelOverlay->canvas.y += amount;
    
    // Testing ambient alpha channel hack
    if (Input.CheckKeyCurrent(VK_R)) panelRenderer->material->ambient += Colors.MakeGrayScale(0.01);
    if (Input.CheckKeyCurrent(VK_F)) panelRenderer->material->ambient -= Colors.MakeGrayScale(0.01);
    
    if (Input.CheckKeyCurrent(VK_T)) panelObject->transform.scale += Vector3(1);
    if (Input.CheckKeyCurrent(VK_G)) panelObject->transform.scale -= Vector3(1);
    
    
    
    
    
    
    
    
    
    
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



