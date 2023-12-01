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
    
    // Camera light
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* light = cameraController->GetComponent<Light>();
    light->color = Colors.MakeRandom();
    light->attenuation = 4;
    light->range = 300;
    light->intensity = 1000;
    
    Engine.sceneMain->AddLightToSceneRoot(light);
    
    
    
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
    
    
    // Initiate text elements
    for (int i=0; i < 10; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer("", fontSize, fontColor, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    
    
    
    
    
    GameObject* actorObject = Engine.CreateAIActor( Vector3(0, 0, 0) );
    
    float area = 10;
    float uniformScale = 0.1;
    
    for (int i=0; i < 100; i++) {
        
        float xx = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float yy = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float zz = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        
        float scalexx = Random.Range(0, 10) * uniformScale;
        float scaleyy = Random.Range(0, 10) * uniformScale;
        float scalezz = Random.Range(0, 10) * uniformScale;
        
        GameObject* newActorObject = Engine.CreateAIActor( Vector3(xx, yy, zz) );
        newActorObject->transform.scale = Vector3(scalexx, scaleyy, scalezz);
        newActorObject->DisableGravity();
        
        MeshRenderer* actorMeshRenderer = newActorObject->GetComponent<MeshRenderer>();
        actorMeshRenderer->material->shader = Engine.shaders.colorUnlit;
        actorMeshRenderer->material->ambient  = Colors.black;
        actorMeshRenderer->material->diffuse  = Colors.MakeRandom();
        actorMeshRenderer->material->diffuse *= Colors.dkgray;
        
        Actor* actor = newActorObject->GetComponent<Actor>();
        actor->SetChanceToMove( Random.Range(0, 2) );
        
        /*
        
        newActorObject->AddComponent( Engine.CreateComponent<Light>() );
        Light* light = newActorObject->GetComponent<Light>();
        light->color = Colors.MakeRandom();
        light->attenuation = 1;
        light->range = 1000;
        light->intensity = 1000;
        
        Engine.sceneMain->AddLightToSceneRoot(light);
        */
        
    }
    
    
    
    
    
    
    
    
    
    
    //
    // Perlin noise example
    //
    
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
    
    
    float depthThreshold = -0.4;
    
    float xNoise = 0.1;
    float yNoise = 0.1;
    float zNoise = 0.1;
    
    int width  = 200;
    int depth  = 50;
    int height = 200;
    
    
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



