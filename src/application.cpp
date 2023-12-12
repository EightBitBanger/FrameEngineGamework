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



#define NUMBER_ON_TEXT_ELEMENTS  80


// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

Text* text[NUMBER_ON_TEXT_ELEMENTS];

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
    for (int i=0; i < NUMBER_ON_TEXT_ELEMENTS; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = (2 * i) + 1;
        
        continue;
    }
    
    Application.SetWindowCenterScale(0.6, 0.5);
    
    ipAddress = "192.168.1.153";
    port      = "80";
    
    
    if (Network.ConnectToServer( ipAddress, 80 )) {
        
        text[0]->text = "Joined as client";
        
        // Client to server
        std::string message = "Message from client";
        Network.SendMessageToServer((char*)message.c_str(), message.size());
        
    } else {
        
        text[0]->text = "Server ready";
        
        // Start a server if not able to join
        Network.StartServer( 80 );
        
    }
    
    return;
}







//
// Application loop
//
unsigned int connectionCount = 0;

void Run() {
    
    if (!Network.GetConnectionStatus()) 
        return;
    
    // Are we the server or client?
    if (Network.GetServerStatus()) {
        
        //
        // Server side
        //
        
        // Number of connections
        text[2]->text = "Number of connections: " + Int.ToString( Network.GetNumberOfConnections() );
        
        
        int currentNumberOfConnections = Network.GetNumberOfConnections();
        
        std::string messageToClients = "Welcome to the server!";
        
        // Send data to clients
        if (connectionCount != currentNumberOfConnections) {
            
            connectionCount = currentNumberOfConnections;
            
            for (int i=0; i < Network.GetNumberOfConnections(); i++) {
                
                Network.Send(i, (char*)messageToClients.c_str(), messageToClients.size());
                
            }
            
            //std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
            
        }
        
        // Show incoming messages from connected clients
        int charWidthMax = 10;
        
        for (int i=0; i < Network.GetNumberOfConnections(); i++) {
            
            std::string incomingString = Network.GetClientBufferByIndex( i );
            
            if (incomingString != "") {
                
                Network.ClearClientBufferByIndex(i);
                
                //int numberOfSegments = incomingString.size() / charWidthMax;
                
                //for (int a=0; a < numberOfSegments; a++) {
                    
                    //std::string segment = incomingString.substr(a * charWidthMax, charWidthMax);
                    std::string segment = incomingString;
                    
                    // Shift text elements down by one
                    for (int i=20; i > 4; i--) 
                        text[i+1]->text = text[i]->text;
                    
                    text[5]->text = segment;
                    
                //}
                
            }
            
        }
        
        
        
    } else {
        
        //
        // Client side
        //
        
        // Read data from server   attempt 2
        std::string buffer;
        buffer.resize(10);
        int bufferSize = Network.GetMessageFromServer((char*)buffer.c_str(), 10);
        
        if (bufferSize > 0) {
            
            buffer.resize(bufferSize);
            
            if (bufferSize > 0) 
                text[5]->text = buffer;
            
        }
        
        
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



