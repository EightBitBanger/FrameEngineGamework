// User application code space
//

#include <GameEngineFramework/Engine/Engine.h>

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



#define NUMBER_OF_TEXT_ELEMENTS  80


// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

Text* text[NUMBER_OF_TEXT_ELEMENTS];

std::string ipAddress;
std::string port;




// Application entry point
//

void Start() {
    
    // Create the main world scene 
    Engine.sceneMain = Engine.Create<Scene>();
    
    // Add the scene to the render system
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);
    
    // Create a sky
    GameObject* skyObject = Engine.CreateSky("sky", Colors.black, Colors.black, 0);
    
    skyObject->GetComponent<MeshRenderer>()->material->EnableDepthTest();
    
    
    // Create a camera controller
    Vector3 position = Vector3(-100, 0, 0);
    Vector3 colliderScale = Vector3(1, 8, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    Engine.sceneMain->camera->DisableMouseLook();
    
    // Attach the sky object to the camera controller
    skyObject->parent = cameraController;
    cameraController->DisableGravity();
    
    
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
    Color fontColor = Colors.white;
    
    
    // Initiate text elements
    for (int i=0; i < NUMBER_OF_TEXT_ELEMENTS; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = (2 * i) + 1;
        
        continue;
    }
    
    Application.SetWindowCenterScale(0.6, 0.5);
    
    
    if (Network.ConnectToHost("127.0.0.1", 27000)) {
        
        // Client
        
        text[0]->text = "Joined host";
        
        
        
    } else {
        
        // Host
        
        if (Network.StartHost(27000)) {
            
            text[0]->text = "Server initiated";
            
            
            
        }
        
    }
    
    
    
    return;
}







//
// Application loop
//

unsigned int numberOfClients=0;

void Run() {
    
    if (!Network.GetConnectionState()) 
        return;
    
    if (Network.GetHostState()) {
        
        // Host
        
        text[2]->text = "Connections: " + Int.ToString( Network.mSockets.size() );
        
        for (int i=0; i < Network.mMessages.size(); i++) {
            
            std::string message = Network.mMessages[i];
            
            text[5 + i]->text = "Client: " + message;
            
            //
            // Send to all clients
            //
            
            if (numberOfClients != Network.mSockets.size()) {
                
                message = Int.ToString( Random.Range(0, 2) );
                send( Network.mSockets[i], (char*)message.c_str(), message.size(), 0 );
                
            }
            
        }
        numberOfClients = Network.mSockets.size();
        
        
    } else {
        
        // Client
        
        if (Random.Range(0, 100) > 10) 
            return;
        
        std::string message = Int.ToString( Random.Range(0, 2) );
        send( Network.mSocket, (char*)message.c_str(), message.size(), 0 );
        
        text[5]->text = "Server: " + Network.mMessages[0];
        
    }
    
    
    
    
    
    
    
    
    
    //text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    //text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    //text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
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



