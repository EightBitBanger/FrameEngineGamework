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



// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

Material* skyMaterial;

Text* text[20];

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
    Engine.sceneMain->camera->DisableMouseLook();
    
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
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    ipAddress = "192.168.1.153";
    port      = "80";
    
    // Client test
    text[0]->text = "Connecting to "+ipAddress+" : "+port;
    
    return;
}







//
// Application loop
//

unsigned int connectionCount = 0;
int initiate = 0;

void Run() {
    
    if (initiate < 10) {
        
        initiate++;
        
        if (initiate == 10) {
            if (Network.ConnectToServer(ipAddress, 27000) == 1) {
                
                text[0]->text = "Joined as client";
                
                // Client to server
                std::string message = "Hello, server.";
                Network.SendMessageToServer((char*)message.c_str(), message.size());
                
            } else {
                
                text[0]->text = "Server ready";
                
                // Start a server if not able to join
                Network.StartServer(27000);
                
            }
            
            initiate = 100;
        }
        
    }
    
    // Wait for connection
    if (!Network.GetConnectionStatus()) 
        return;
    
    
    // Are we the server or client?
    if (Network.GetServerStatus()) {
        
        
        //
        // Server side
        //
        
        
        int currentNumberOfConnections = Network.GetNumberOfConnections();
        
        std::string messageToClients = "New client joined!";
        
        // Number of connections
        if (initiate == 100) 
            text[3]->text = "Number of connctions: " + Int.ToString( currentNumberOfConnections );
        
        // Send data to clients
        if (0 != currentNumberOfConnections) {
            
            //connectionCount = currentNumberOfConnections;
            
            std::string message = "Welcome to the server!";
            
            for (int i=0; i < Network.GetNumberOfConnections(); i++) {
                
                Network.Send(i, (char*)message.c_str(), message.size());
                
                std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
                
            }
            
        }
        
        // Show incoming messages from connected clients
        
        for (int i=0; i < Network.GetNumberOfConnections(); i++) {
            
            std::string incomingString = Network.GetClientBufferByIndex( i );
            
            if (incomingString != "") {
                
                Network.ClearClientBufferByIndex(i);
                
                std::cout << incomingString << std::endl;
                
            }
            
        }
        
        
        
    } else {
        
        
        //
        // Client side
        //
        
        std::string buffer;
        buffer.resize(100);
        
        
        /*
        
        // Read data from server   attempt 1
        int numberOfBytes = recv(Network.mHost.mSocket, (char*)buffer.c_str(), 100, 0);
        
        if (numberOfBytes > 0) {
            buffer.resize(numberOfBytes);
            std::cout << buffer << std::endl;
        }
        */
        
        
        
        // Read data from server   attempt 2
        buffer.resize(100);
        int bufferSize = Network.GetMessageFromServer((char*)buffer.c_str(), 100);
        
        if (bufferSize > 0) {
            
            buffer.resize(bufferSize);
            
            if (bufferSize > 0) 
                std::cout << buffer << std::endl;
            
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



