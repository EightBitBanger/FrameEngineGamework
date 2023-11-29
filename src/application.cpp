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
    
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* cameraLight = cameraController->GetComponent<Light>();
    Engine.sceneMain->AddLightToSceneRoot(cameraLight);
    cameraLight->color = Colors.white;
    cameraLight->attenuation = 10;
    cameraLight->intensity   = 1000;
    cameraLight->range       = 400;
    
    
    
    GameObject* plain = Engine.Create<GameObject>();
    plain->transform.scale = Vector3(10, 10, 10);
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("cube"), Resources.CreateMaterialFromTag("barrel") ) );
    Engine.sceneMain->AddMeshRendererToSceneRoot( plain->GetComponent<MeshRenderer>() );
    
    Mesh* plainMesh = plain->GetComponent<MeshRenderer>()->mesh;
    plain->GetComponent<MeshRenderer>()->material->shader = Engine.shaders.color;
    plain->GetComponent<MeshRenderer>()->material->ambient = Colors.MakeGrayScale(0.1);
    plain->GetComponent<MeshRenderer>()->material->diffuse = Colors.MakeGrayScale(0);
    plain->GetComponent<MeshRenderer>()->material->DisableCulling();
    
    
    SubMesh meshPlainPart;
    
    plainMesh->CopySubMesh(0, meshPlainPart);
    
    Engine.Destroy<Mesh>( plainMesh );
    plainMesh = Engine.Create<Mesh>();
    
    
    
    
    
    
    
    
    
    //
    // Perlin noise example
    //
    
    
    float depthThreshold = -0.5;
    
    float xNoise = 0.1;
    float yNoise = 0.1;
    float zNoise = 0.1;
    
    int width  = 80;
    int depth  = 80;
    int height = 80;
    
    
    for (int z=0; z < height; z++) {
        
        for (int y=0; y < depth; y++) {
            
            for (int x=0; x < width; x++) {
                
                if (Random.Range(0, 4) > 1) 
                    continue;
                
                float CoordX = x * xNoise;
                float CoordY = y * yNoise;
                float CoordZ = z * zNoise;
                
                float noiseTotal = Random.Perlin(CoordX, CoordY, CoordZ);
                
                if (noiseTotal > depthThreshold) 
                    continue;
                
                // Create new color for the sub mesh
                Color color = Colors.MakeRandomGrayScale();
                color *= Color(0.2, 0.2, 0.2);
                color += Color(0.03, 0.03, 0.03);
                
                if (Random.Range(0, 10) > 70) color += Colors.green  * Color(0.1, 0.1, 0.1);
                if (Random.Range(0, 10) > 40) color += Colors.yellow * Color(0.1, 0.1, 0.1);
                if (Random.Range(0, 10) > 20) color += Colors.orange * Color(0.1, 0.1, 0.1);
                if (Random.Range(0, 10) > 10) color += Colors.blue   * Color(0.1, 0.1, 0.1);
                
                // Change sub mesh color
                for (unsigned int v=0; v < meshPlainPart.vertexCount; v++) {
                    
                    meshPlainPart.vertexBuffer[v].r = color.r;
                    meshPlainPart.vertexBuffer[v].g = color.g;
                    meshPlainPart.vertexBuffer[v].b = color.b;
                    
                }
                
                
                plainMesh->AddSubMesh((float)x - (width / 2), (float)y, (float)z - (height / 2), meshPlainPart, false);
                
            }
        }
    }
    plainMesh->UploadToGPU();
    
    
    
    
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
    
    
    
    
    //
    // Panel UI
    //
    
    
    
    //
    // Left panel
    //
    
    GameObject* panelLeft = Engine.CreateOverlayPanelRenderer(110, 10000, "panel_blue");
    //Panel* panelBaseOverlay = panelLeft->GetComponent<Panel>();
    
    //panelBaseOverlay->canvas.anchorCenterVert = true;
    //panelBaseOverlay->canvas.
    
    //
    // Top panel
    //
    
    GameObject* panelTitle = Engine.CreateOverlayPanelRenderer(10000, 59, "panel_gray");
    Panel* panelTitleOverlay = panelTitle->GetComponent<Panel>();
    
    panelTitleOverlay->canvas.anchorCenterHorz = true;
    panelTitleOverlay->canvas.anchorTop = true;
    
    //sceneOverlay->AddMeshRendererToSceneRoot( panelLeft->GetComponent<MeshRenderer>() );   // Left panel
    sceneOverlay->AddMeshRendererToSceneRoot( panelTitle->GetComponent<MeshRenderer>() );  // Title bar
    
    
    
    // Initiate left panel text elements
    for (int i=0; i < 20; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer("", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    
    
    // Title bar menu items
    GameObject* textObject = Engine.CreateOverlayTextRenderer("File  Edit  Project  Settings", fontSize, fontColor, "font");
    
    sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
    Text* titleText = textObject->GetComponent<Text>();
    titleText->canvas.anchorTop = true;
    titleText->canvas.x = 1;
    titleText->canvas.y = 1;
    
    
    
    
    //
    // Button callback test
    //
    
    Button* buttonExample = Engine.CreateButton(0, 0, 60, 50);
    buttonExample->triggerOnLeftButton = true;
    buttonExample->triggerOnPressed    = true;
    buttonExample->callback = callbackButtonA;
    
    
    return;
}







//
// Application loop
//

void Run() {
    
    if (Input.CheckKeyCurrent(VK_I)) Engine.sceneMain->camera->viewport.x += 10;
    if (Input.CheckKeyCurrent(VK_K)) Engine.sceneMain->camera->viewport.x -= 10;
    if (Input.CheckKeyCurrent(VK_J)) Engine.sceneMain->camera->viewport.y += 10;
    if (Input.CheckKeyCurrent(VK_L)) Engine.sceneMain->camera->viewport.y -= 10;
    
    
    float viewScale = 0;
    
    if (Input.CheckKeyCurrent(VK_R)) viewScale =  1;
    if (Input.CheckKeyCurrent(VK_F)) viewScale = -1;
    
    Engine.sceneMain->camera->viewport.w += viewScale;
    Engine.sceneMain->camera->viewport.h += viewScale;
    
    
    text[5]->text = "Renderer - " + FloatToString( Engine.profileRenderSystem );
    text[6]->text = "Physics  - " + FloatToString( Engine.profilePhysicsSystem );
    text[7]->text = "Engine   - " + FloatToString( Engine.profileGameEngineUpdate );
    text[8]->text = "ActorAI  - " + FloatToString( Engine.profileActorAI );
    
    
    
    
    
    
    
    
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



