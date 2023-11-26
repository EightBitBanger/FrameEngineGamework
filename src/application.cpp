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

GameObject* plain;
Mesh*       plainMesh;
SubMesh     meshPlainPart;

GameObject* overlayObject[20];

MeshRenderer* overlayRenderer;
Vector3 overlayPosition;

GameObject* actorObject;

GameObject* bannerObject;
Text*       bannerText;


// Application entry point
//

void Start() {
    
    // Create the main world scene and add it to the render queue
    Engine.sceneMain = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);
    
    // Create a sky
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.2);
    skyLow  += Colors.MakeGrayScale(0.6);
    
    float fadeBias = 0.0001;
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, fadeBias);
    skyMaterial = skyObject->GetComponent<MeshRenderer>()->material;
    skyMaterial->diffuse = Color(0.087, 0.087, 0.087);
    skyMaterial->EnableDepthTest();
    
    
    // Create a camera controller
    cameraController = Engine.CreateCameraController(Vector3(0, 0, 0), Vector3(1, 8, 1));
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    cameraController->DisableGravity();
    
    // Attach sky object to the camera object
    skyObject->parent = cameraController;
    
    
    cameraController->AddComponent( Engine.CreateComponent<Light>() );
    Light* cameraLight = cameraController->GetComponent<Light>();
    cameraLight->intensity   = 1000;
    cameraLight->range       = 100;
    cameraLight->attenuation = 2;
    cameraLight->color       = Colors.white;
    
    
    
    
    plain = Engine.Create<GameObject>();
    plain->transform.scale = Vector3(10, 10, 10);
    plain->AddComponent( Engine.CreateComponent<MeshRenderer>( Resources.CreateMeshFromTag("cube"), Resources.CreateMaterialFromTag("barrel") ) );
    plainMesh = plain->GetComponent<MeshRenderer>()->mesh;
    plain->GetComponent<MeshRenderer>()->material->shader = Engine.shaders.color;
    plain->GetComponent<MeshRenderer>()->material->ambient = Colors.MakeGrayScale(0.1);
    plain->GetComponent<MeshRenderer>()->material->diffuse = Colors.MakeGrayScale(0);
    plain->GetComponent<MeshRenderer>()->material->DisableCulling();
    
    
    
    
    plainMesh->CopySubMesh(0, meshPlainPart);
    
    Engine.Destroy<Mesh>( plainMesh );
    plainMesh = Engine.Create<Mesh>();
    
    
    
    
    /*

    for (int x=0; x < 1000000; x++) {
        
        float CoordX = Random.Range(0, 100) - Random.Range(0, 100);
        float CoordY = Random.Range(0, 100) - Random.Range(0, 100);
        float CoordZ = Random.Range(0, 100) - Random.Range(0, 100);
        
        // Create new color for the sub mesh
        Color color = Colors.MakeRandomGrayScale();
        color *= Color(0.2, 0.2, 0.2);
        color += Color(0.04, 0.04, 0.04);
        
        if (Random.Range(0, 10) > 8) 
            color += Colors.yellow;
        
        if (Random.Range(0, 10) > 4) 
            color += Colors.blue * Color(0.4, 0.4, 0.4);
        
        // Change sub mesh color
        for (int v=0; v < meshPlainPart.vertexCount; v++) {
            meshPlainPart.vertexBuffer[v].r = color.r;
            meshPlainPart.vertexBuffer[v].g = color.g;
            meshPlainPart.vertexBuffer[v].b = color.b;
        }
        
        
        if (!plainMesh->AddSubMesh(CoordX, CoordY, CoordZ, meshPlainPart, false)) 
            break;
        
        continue;
    }
    plainMesh->UploadToGPU();
    */
    
    
    
    
    
    
    /*
    float depthThreshold = -0.4;
    
    float xNoise = 0.1;
    float yNoise = 0.1;
    float zNoise = 0.1;
    
    int width  = 200;
    int depth  = 100;
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
    */
    
    
    actorObject = Engine.CreateAIActor( Vector3(0, 0, 0) );
    
    float area = 100;
    float uniformScale = 0.9;
    
    for (int i=0; i < 100; i++) {
        
        float xx = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float yy = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        float zz = (Random.Range(0, 10) - Random.Range(0, 10)) * area;
        
        float scalexx = Random.Range(0, 10) * uniformScale;
        float scaleyy = Random.Range(0, 10) * uniformScale;
        float scalezz = Random.Range(0, 10) * uniformScale;
        
        GameObject* newActorObject = Engine.CreateAIActor( Vector3(xx, yy, zz) );
        newActorObject->transform.scale = Vector3(scalexx, scaleyy, scalezz);
        
        MeshRenderer* actorMeshRenderer = newActorObject->GetComponent<MeshRenderer>();
        actorMeshRenderer->material->shader = Engine.shaders.colorUnlit;
        actorMeshRenderer->material->ambient  = Colors.black;
        actorMeshRenderer->material->diffuse  = Colors.MakeRandom();
        actorMeshRenderer->material->diffuse *= Colors.dkgray;
        
        Actor* actor = newActorObject->GetComponent<Actor>();
        actor->SetChanceToMove( Random.Range(0, 10) );
        
        
    }
    
    
    
    
    
    
    
    //
    // Overlay example
    
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
    
    
    
    
    
    // Upper left
    bannerObject = Engine.CreateOverlayTextRenderer("Upper left", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.anchorTop = true;
    bannerText->canvas.x += 1;
    bannerText->canvas.y += 2;
    
    // Upper right
    bannerObject = Engine.CreateOverlayTextRenderer("Upper right", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.anchorTop = true;
    bannerText->canvas.anchorRight = true;
    bannerText->canvas.x -= 3;
    bannerText->canvas.y += 2;
    
    
    // Lower left
    bannerObject = Engine.CreateOverlayTextRenderer("Lower left", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    
    // Lower right
    bannerObject = Engine.CreateOverlayTextRenderer("Lower right", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.anchorRight = true;
    bannerText->canvas.x -= 3;
    bannerText->canvas.y  = 0;
    
    
    
    
    
    // Center vertically
    bannerObject = Engine.CreateOverlayTextRenderer("Center vertically", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.anchorCenterVert = true;
    bannerText->canvas.y = -1;
    
    
    
    // Center horizontally
    bannerObject = Engine.CreateOverlayTextRenderer("Center horizontally", fontSize, fontColor, "font");
    sceneOverlay->AddMeshRendererToSceneRoot( bannerObject->GetComponent<MeshRenderer>() );
    bannerText = bannerObject->GetComponent<Text>();
    
    bannerText->canvas.anchorCenterHorz = true;
    bannerText->canvas.x = -11;
    
    
    
    
    
    
    
    /*
    Text* text[10];
    for (int i=0; i < 10; i++) {
        
        overlayObject[i] = Engine.CreateOverlayTextRenderer("", 10, Colors.black, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( overlayObject[i]->GetComponent<MeshRenderer>() );
        
        text[i] = overlayObject[i]->GetComponent<Text>();
        
        text[i]->canvas.position.x =  1;
        text[i]->canvas.position.y =  5 + i;
        
        //text[i]->color = Colors.black;
        
    }
    
    text[0]->color = Colors.red;
    text[0]->text = "Render window";
    text[0]->canvas.position.x =  1;
    text[0]->canvas.position.y =  3;
    overlayObject[0]->transform.scale = glm::vec3(0.01, 0.01, 0.01);
    
    //text[4]->canvas.anchorRight = true;
    */
    
    
    
    // World physics
    
    Physics.SetWorldGravity(0, 0, 0);
    
    
    return;
}




// Camera movement force
float cameraSpeed   = 3;



//
// Application loop
//

void Run() {
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    
    if (Input.CheckKeyCurrent(VK_I)) {bannerText->scaleWidth  -= 1.0;}
    if (Input.CheckKeyCurrent(VK_K)) {bannerText->scaleWidth  += 1.0;}
    if (Input.CheckKeyCurrent(VK_J)) {bannerText->scaleHeight -= 1.0;}
    if (Input.CheckKeyCurrent(VK_L)) {bannerText->scaleHeight += 1.0;}
    
    
    
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
        
        if (cameraController != nullptr) 
            force *= cameraSpeed;
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 3;
        
        cameraController->AddForce(force.x, force.y, force.z);
        
    }
    
    
    
    
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
// Called once every tick
//


void TickUpdate(void) {
    
    
    
    return;
}





void Shutdown(void) {
    
    
    
    return;
}



