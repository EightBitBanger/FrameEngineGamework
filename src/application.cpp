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

Text* text[80];

Actor* actor;





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
    Color skyLow(Colors.dkgray);
    
    skyHigh += Colors.MakeGrayScale(0.3);
    skyLow  += Colors.MakeGrayScale(0.01);
    
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
    
    // Initiate text elements
    for (int i=0; i < 80; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.white, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    
    Mesh* cubeMeshPtr = Resources.CreateMeshFromTag("cube");
    
    
    
    //
    // Generate some AI actors
    //
    
    float area = 0.3;
    float uniformScale = 0.1;
    unsigned int numberOfActors = 1;
    
    for (int i=0; i < numberOfActors; i++) {
        
        float xx = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float yy = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float zz = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        
        float scalexx = Random.Range(0, 10) * uniformScale;
        float scaleyy = Random.Range(0, 10) * uniformScale;
        float scalezz = Random.Range(0, 10) * uniformScale;
        
        GameObject* newActorObject = Engine.CreateAIActor(Vector3(xx, yy, zz), cubeMeshPtr);
        newActorObject->GetComponent<Transform>()->scale = Vector3(scalexx, scaleyy, scalezz);
        newActorObject->DisableGravity();
        
        MeshRenderer* actorMeshRenderer = newActorObject->GetComponent<MeshRenderer>();
        actorMeshRenderer->material->shader = Engine.shaders.colorUnlit;
        actorMeshRenderer->material->ambient  = Colors.black;
        actorMeshRenderer->material->diffuse  = Colors.MakeRandom();
        actorMeshRenderer->material->diffuse *= Colors.dkgray;
        
        actor = newActorObject->GetComponent<Actor>();
        
        // Initiate actor neural network layers
        for (int a=0; a < 25; a++) {
            
            WeightedLayer layer;
            
            // Random plasticity
            layer.plasticity = 0;
            
            if (Random.Range(0, 100) < 10) 
                layer.plasticity = 0.001;
            
            // Set random weights
            for (int z=0; z < NEURAL_LAYER_WIDTH; z++) 
                if (Random.Range(0, 100) < 90) 
                    layer.weight[z] = 1.0 - ((Random.Range(0, 10) * 0.0001) - ((Random.Range(0, 10) * 0.0001)));
            
            actor->AddWeightedLayer(layer);
        }
        
        continue;
    }
    
    return;
}







//
// Application loop
//

void Run() {
    
    WeightedLayer inputLayer = actor->GetWeightedLayer(0);
    
    std::string values;
    std::string inputValues;
    std::string outputValues;
    
    for (int i=0; i < NEURAL_LAYER_WIDTH; i++) 
        inputValues += Float.ToString( inputLayer.node[i] )  + " ";
    
    
    text[1]->text = "Inputs";
    text[2]->text = inputValues;
    
    
    // Display weighted layers
    
    
    for (int i=0; i < actor->GetNumberOfWeightedLayers(); i++) {
        
        WeightedLayer weightedLayer = actor->GetWeightedLayer(i);
        
        values = "";
        
        for (int a=0; a < NEURAL_LAYER_WIDTH; a++) 
            values += Float.ToString( weightedLayer.node[a] ) + " ";
        
        
        values += "  |  ";
        
        for (int a=0; a < NEURAL_LAYER_WIDTH; a++) 
            values += Float.ToString( weightedLayer.weight[a] ) + " ";
        
        
        values += "  |  ";
        
        values += Float.ToString( weightedLayer.plasticity );
        
        text[5 + i]->text = values;
        
        continue;
    }
    
    
    
    
    
    
    
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
    
    
    
    
    // Input layer
    if (Input.CheckKeyPressed(VK_RETURN)) {
        NeuralLayer inputLayer;
        
        inputLayer.node[0] = Random.Range(0, 100) * 0.01;
        inputLayer.node[1] = Random.Range(0, 100) * 0.01;
        inputLayer.node[2] = Random.Range(0, 100) * 0.01;
        
        actor->SetNeuralInputLayer(inputLayer);
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



