// User application code space
//

#include <GameEngineFramework/Engine/Engine.h>

extern EngineComponents     Components;
extern ColorPreset          Colors;
extern NumberGeneration     Random;
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
    Vector3 position = Vector3(0, 120, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
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
    for (int i=0; i < 10; i++) {
        
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.black, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
        
    }
    
    
    
    //
    // Generate some chunks
    //
    
    Material* plainMaterial = Resources.CreateMaterialFromTag("chunk");
    plainMaterial->shader = Engine.shaders.texture;
    
    int chunkSize   = 100;
    int chunkWidth  = 16;
    int chunkHeight = 16;
    
    for (int z=0; z <= chunkHeight; z++) {
        
        for (int x=0; x <= chunkWidth; x++) {
            
            float xx = (x * chunkSize) - (chunkSize * (chunkWidth / 2));
            float zz = (z * chunkSize) - (chunkSize * (chunkWidth / 2));
            
            GameObject* plainObject = Engine.Create<GameObject>();
            plainObject->AddComponent( Engine.CreateComponentMeshRenderer( Engine.meshes.plain, plainMaterial ) );
            
            Transform* transform  = plainObject->GetComponent<Transform>();
            transform->position = Vector3(xx, 0, zz);
            transform->scale    = Vector3(chunkSize, 1, chunkSize);
            
            //MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
            //plainRenderer->mesh = Engine.meshes.plain;
            //plainRenderer->material = Resources.CreateMaterialFromTag("grassy");
            //plainRenderer->material->shader = Engine.shaders.texture;
            
            //Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer );
            
            continue;
        }
        
        continue;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    //
    // Generate AI actor
    //
    float spread = 80;
    
    for (int i=0; i < 100; i++) {
        
        Vector3 position;
        position.x = Random.Range(0.0f, spread) - Random.Range(0.0f, spread);
        position.z = Random.Range(0.0f, spread) - Random.Range(0.0f, spread);
        
        GameObject* newActorObject = Engine.CreateAIActor( position );
        newActorObject->GetComponent<Transform>()->scale = Vector3(1, 1, 1);
        newActorObject->DisableGravity();
        
        Actor* actor = newActorObject->GetComponent<Actor>();
        actor->SetSpeed( 2.4 );
        
        float variantR = Random.Range(0, 10) * 0.1;
        float variantG = Random.Range(0, 10) * 0.1;
        float variantB = Random.Range(0, 10) * 0.1;
        
        // Body gene
        Gene geneBody;
        geneBody.offset    = BaseGene(0, 0, 0);
        geneBody.position  = BaseGene(0, 0.7, 0);
        geneBody.scale     = BaseGene(0.4, 0.4, 0.9);
        geneBody.color     = BaseGene(variantR, variantG, variantB);
        
        // Head gene
        Gene geneHead;
        geneHead.offset    = BaseGene(0, 1.02, 0.254);
        geneHead.position  = BaseGene(0, 0, 0.3);
        geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
        geneHead.color     = BaseGene(0.4, 0.4, 0.4);
        
        // Limb FL gene
        Gene geneLimbFrontLeft;
        geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
        geneLimbFrontLeft.position  = BaseGene(0, -0.4, 0);
        geneLimbFrontLeft.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbFrontLeft.color     = BaseGene(0.4, 0.4, 0.4);
        
        geneLimbFrontLeft.doAnimationCycle = true;
        geneLimbFrontLeft.animationAxis    = BaseGene(2, 0, 0);
        geneLimbFrontLeft.animationRange   = 15;
        
        // Limb FR gene
        Gene geneLimbFrontRight;
        geneLimbFrontRight.offset    = BaseGene(-0.17, 0.75, 0.4);
        geneLimbFrontRight.position  = BaseGene(0, -0.4, 0);
        geneLimbFrontRight.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbFrontRight.color     = BaseGene(0.4, 0.4, 0.4);
        
        geneLimbFrontRight.doAnimationCycle   = true;
        geneLimbFrontRight.doInverseAnimation = true;
        geneLimbFrontRight.animationAxis      = BaseGene(2, 0, 0);
        geneLimbFrontRight.animationRange     = 15;
        
        // Limb RL gene
        Gene geneLimbRearLeft;
        geneLimbRearLeft.offset    = BaseGene(0.17, 0.75, -0.4);
        geneLimbRearLeft.position  = BaseGene(0, -0.4, 0);
        geneLimbRearLeft.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbRearLeft.color     = BaseGene(0.4, 0.4, 0.4);
        
        geneLimbRearLeft.doAnimationCycle = true;
        geneLimbRearLeft.animationAxis    = BaseGene(2, 0, 0);
        geneLimbRearLeft.animationRange   = 15;
        
        // Limb RR gene
        Gene geneLimbReadRight;
        geneLimbReadRight.offset    = BaseGene(-0.17, 0.75, -0.4);
        geneLimbReadRight.position  = BaseGene(0, -0.4, 0);
        geneLimbReadRight.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbReadRight.color     = BaseGene(0.4, 0.4, 0.4);
        
        geneLimbReadRight.doAnimationCycle   = true;
        geneLimbReadRight.doInverseAnimation = true;
        geneLimbReadRight.animationAxis      = BaseGene(2, 0, 0);
        geneLimbReadRight.animationRange     = 15;
        
        
        // Apply genes to the actor
        actor->AddGene(geneBody);
        actor->AddGene(geneHead);
        
        actor->AddGene(geneLimbFrontLeft);
        actor->AddGene(geneLimbFrontRight);
        actor->AddGene(geneLimbRearLeft);
        actor->AddGene(geneLimbReadRight);
        
        continue;
    }
    
    return;
}







//
// Application loop
//

void Run() {
    
    text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
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
        
        
        force *= 0.3;
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 10;
        
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



