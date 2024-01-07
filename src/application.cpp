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

GameObject* directionalLight;
Transform*  lightTransform;

Transform*  objectTransform;

Text* text[20];

std::vector<MeshRenderer*> shadowList;





// Application entry point
//

void Start() {
    
    // Create the main world scene 
    Engine.sceneMain   = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);
    
    
    //
    // Create a sky
    
    float skyBrightness = 0.87;
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.997);
    skyLow  += Colors.MakeGrayScale(0.799);
    
    skyHigh *= Colors.MakeGrayScale(0.961);
    
    skyHigh *= Colors.MakeGrayScale( skyBrightness );
    skyLow  *= Colors.MakeGrayScale( skyBrightness );
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer, RENDER_QUEUE_SKY );
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Color(0.087, 0.087, 0.087);
    
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(-13, 8, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    // Attach the sky object to the camera controller
    skyObject->GetComponent<Transform>()->parent = cameraController->GetComponent<Transform>();
    cameraController->DisableGravity();
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 3 );
    cameraController->SetMass( 10 );
    
    
    
    
    
    
    
    //
    // Shadow caster experimentation
    //
    
    Mesh* shadowMesh = Resources.CreateMeshFromTag("cube");
    
    for (int i=0; i < 4; i++) {
        
        GameObject* shadowObject = Engine.Create<GameObject>();
        
        objectTransform = shadowObject->GetComponent<Transform>();
        objectTransform->position.x = Random.Range(0, 80) - Random.Range(0, 80);
        objectTransform->position.y += 20;
        objectTransform->position.z = Random.Range(0, 80) - Random.Range(0, 80);
        
        if (objectTransform->position.x > 0) objectTransform->position.x += 1;
        if (objectTransform->position.z > 0) objectTransform->position.z += 1;
        if (objectTransform->position.x < 0) objectTransform->position.x -= 1;
        if (objectTransform->position.z < 0) objectTransform->position.z -= 1;
        
        
        shadowObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        MeshRenderer* objectRenderer = shadowObject->GetComponent<MeshRenderer>();
        objectRenderer->mesh = shadowMesh;
        
        objectRenderer->material = Engine.Create<Material>();
        objectRenderer->material->shader = Engine.shaders.color;
        
        objectRenderer->material->ambient = Color(0.01, 0.01, 0.01);
        objectRenderer->material->diffuse = Color(0.01, 0.01, 0.01);
        
        objectRenderer->material->EnableShadowPass();
        Engine.sceneMain->AddMeshRendererToSceneRoot( objectRenderer, RENDER_QUEUE_DEFAULT );
        
        shadowList.push_back( objectRenderer );
    }
    
    
    
    
    
    
    
    
    
    // Directional light
    directionalLight = Engine.Create<GameObject>();
    lightTransform = directionalLight->GetComponent<Transform>();
    lightTransform->RotateAxis(1, Vector3(0, -1, -0.3));
    
    directionalLight->AddComponent( Engine.CreateComponent<Light>() );
    Light* sunLight = directionalLight->GetComponent<Light>();
    
    Engine.sceneMain->AddLightToSceneRoot( sunLight );
    
    sunLight->intensity  = 0.5;
    sunLight->type       = LIGHT_TYPE_DIRECTIONAL;
    sunLight->color      = Colors.white;
    
    
    
    
    
    
    
    // Scene overlay
    sceneOverlay = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(sceneOverlay);                         // UI
    
    sceneOverlay->camera = Engine.Create<Camera>();
    sceneOverlay->camera->isOrthographic = true;
    sceneOverlay->camera->isFixedAspect  = true;
    
    sceneOverlay->camera->viewport.w = Renderer.displaySize.x;
    sceneOverlay->camera->viewport.h = Renderer.displaySize.y;
    
    sceneOverlay->camera->clipFar  =  100;
    sceneOverlay->camera->clipNear = -100;
    
    // Initiate text elements
    for (int i=0; i < 10; i++) {
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.white, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
    }
    
    
    
    
    
    
    
    //
    // Generate some ground chunks
    //
    
    // Base chunk mesh
    Mesh* plainBaseMesh = Resources.CreateMeshFromTag("chunk");
    SubMesh chunkSubMesh;
    plainBaseMesh->CopySubMesh(0, chunkSubMesh);
    
    // Chunk material
    Material* plainMaterial = Resources.CreateMaterialFromTag("grassy");
    plainMaterial->shader = Engine.shaders.texture;
    plainMaterial->diffuse = Colors.MakeGrayScale(0.001);
    plainMaterial->DisableShadowPass();
    
    // Chunk layout
    
    int chunkSize   = 50;
    
    int worldWidth  = 20;
    int worldHeight = 20;
    
    // Chunk mesh
    Mesh* chunkMesh = Engine.Create<Mesh>();
    
    // Chunk object
    GameObject* plainObject = Engine.Create<GameObject>();
    plainObject->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, plainMaterial ) );
    plainObject->AddComponent( Engine.CreateComponent<RigidBody>() );
    
    MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer, RENDER_QUEUE_BACKGROUND );
    
    
    plainObject->SetAngularAxisLockFactor(0, 0, 0);
    plainObject->SetLinearAxisLockFactor(0, 0, 0);
    plainObject->SetStatic();
    
    // Transform the chunk
    Transform* chunkTransform = plainObject->GetComponent<Transform>();
    chunkTransform->localScale = Vector3(chunkSize * 0.013, 1, chunkSize * 0.013);
    
    
    for (int z=0; z < worldHeight; z++) {
        
        float chunkZ = (z * chunkSize) - ((worldHeight / 2) * chunkSize);
        
        for (int x=0; x < worldWidth; x++) {
            
            float chunkX = (x * chunkSize) - ((worldWidth  / 2) * chunkSize);
            
            chunkMesh->AddSubMesh(chunkX, 0, chunkZ, chunkSubMesh, false);
            
            // Collider
            BoxShape* plainCollider = Physics.CreateColliderBox(chunkSize, 100, chunkSize);
            plainObject->AddColliderBox(plainCollider, chunkX, -100, chunkZ);
            
            continue;
        }
        
        continue;
    }
    
    chunkMesh->UploadToGPU();
    
    
    
    
    
    
    
    
    
    
    //
    // Generate AI actors
    //
    
    
    float spread = 50;
    
    for (int i=0; i < 8; i++) {
        
        Vector3 position;
        position.x = (Random.Range(0.0f, spread) * 0.1) - (Random.Range(0.0f, spread) * 0.1);
        position.y = 100;
        position.z = (Random.Range(0.0f, spread) * 0.1) - (Random.Range(0.0f, spread) * 0.1);
        
        GameObject* newActorObject = Engine.CreateAIActor( position );
        Transform* transform = newActorObject->GetComponent<Transform>();
        transform->localScale = Vector3(1, 1, 1);
        
        // Collision
        BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
        newActorObject->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Actor);
        
        // Actor
        Actor* actor = newActorObject->GetComponent<Actor>();
        actor->SetSpeed( 2.3 );
        
        float variantR = Random.Range(0, 10) * 0.001;
        float variantG = Random.Range(0, 10) * 0.001;
        float variantB = Random.Range(0, 10) * 0.001;
        
        Color baseColor = Colors.MakeGrayScale(0.001);
        
        variantG = variantR;
        variantB = variantR;
        
        // Body gene
        Gene geneBody;
        geneBody.offset    = BaseGene(0, 0, 0);
        geneBody.position  = BaseGene(0, 0.7, 0);
        geneBody.scale     = BaseGene(0.4, 0.4, 0.9);
        geneBody.color     = BaseGene(variantR, variantG, variantB);
        geneBody.color.x  *= baseColor.r;
        geneBody.color.y  *= baseColor.g;
        geneBody.color.z  *= baseColor.b;
        
        // Head gene
        Gene geneHead;
        geneHead.offset    = BaseGene(0, 1.02, 0.254);
        geneHead.position  = BaseGene(0, 0, 0.3);
        geneHead.scale     = BaseGene(0.415, 0.395, 0.415);
        geneHead.color     = BaseGene(0.4, 0.4, 0.4);
        geneHead.color.x  *= baseColor.r;
        geneHead.color.y  *= baseColor.g;
        geneHead.color.z  *= baseColor.b;
        
        // Limb FL gene
        Gene geneLimbFrontLeft;
        geneLimbFrontLeft.offset    = BaseGene(0.17, 0.75, 0.4);
        geneLimbFrontLeft.position  = BaseGene(0, -0.4, 0);
        geneLimbFrontLeft.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbFrontLeft.color     = BaseGene(0.4, 0.4, 0.4);
        geneLimbFrontLeft.color.x  *= baseColor.r;
        geneLimbFrontLeft.color.y  *= baseColor.g;
        geneLimbFrontLeft.color.z  *= baseColor.b;
        
        geneLimbFrontLeft.doAnimationCycle = true;
        geneLimbFrontLeft.animationAxis    = BaseGene(2, 0, 0);
        geneLimbFrontLeft.animationRange   = 15;
        
        // Limb FR gene
        Gene geneLimbFrontRight;
        geneLimbFrontRight.offset    = BaseGene(-0.17, 0.75, 0.4);
        geneLimbFrontRight.position  = BaseGene(0, -0.4, 0);
        geneLimbFrontRight.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbFrontRight.color     = BaseGene(0.4, 0.4, 0.4);
        geneLimbFrontRight.color.x  *= baseColor.r;
        geneLimbFrontRight.color.y  *= baseColor.g;
        geneLimbFrontRight.color.z  *= baseColor.b;
        
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
        geneLimbRearLeft.color.x  *= baseColor.r;
        geneLimbRearLeft.color.y  *= baseColor.g;
        geneLimbRearLeft.color.z  *= baseColor.b;
        
        geneLimbRearLeft.doAnimationCycle = true;
        geneLimbRearLeft.animationAxis    = BaseGene(2, 0, 0);
        geneLimbRearLeft.animationRange   = 15;
        
        // Limb RR gene
        Gene geneLimbReadRight;
        geneLimbReadRight.offset    = BaseGene(-0.17, 0.75, -0.4);
        geneLimbReadRight.position  = BaseGene(0, -0.4, 0);
        geneLimbReadRight.scale     = BaseGene(0.2, 0.65, 0.2);
        geneLimbReadRight.color     = BaseGene(0.4, 0.4, 0.4);
        geneLimbReadRight.color.x  *= baseColor.r;
        geneLimbReadRight.color.y  *= baseColor.g;
        geneLimbReadRight.color.z  *= baseColor.b;
        
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
glm::vec3 sunDir(0, 0, 1);
float sunStep = 0.5;
float sunRate = 1;

float shadowLength   = 10;
float shadowRayScale = 0.997;


void Run() {
    
    //lightTransform->RotateAxis(1, Vector3(1, 0, 0));
    
    objectTransform->RotateAxis(1, Vector3(0, 1, 0));
    
    //
    // Shadow experimentation
    //
    
    
    
    
    
    
    //
    // Profiling
    //
    
    text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    //text[5]->text = "x - " + Float.ToString( cameraController->GetComponent<Transform>()->position.x );
    //text[6]->text = "y - " + Float.ToString( cameraController->GetComponent<Transform>()->position.y );
    //text[7]->text = "z - " + Float.ToString( cameraController->GetComponent<Transform>()->position.z );
    
    
    
    
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    /*
    
    Transform* transform = directionalLight->GetComponent<Transform>();
    Light* light = directionalLight->GetComponent<Light>();
    
    //transform->RotateAxis(-10, Vector3(1, 0, 0));
    
    transform->RotateEuler(0.001, 0, 0);
    
    Vector3 direction = transform->EulerAngles();
    
    if ((direction.x > -90) & (direction.x < 90)) {
        
        light->isActive = true;
        
    } else {
        
        light->isActive = false;
        transform->SetIdentity();
        
        transform->RotateEuler(-90, 0, 0);
        
    }
    
    Vector3 direction = transform->EulerAngles();
    text[8]->text  = Float.ToString( direction.x );
    text[9]->text  = Float.ToString( direction.y );
    text[10]->text = Float.ToString( direction.z );
    
    */
    
    
    
    
    
    
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
        
        if (Input.CheckKeyCurrent(VK_CONTROL)) force *= 5;
        
        if (force != glm::vec3(0)) 
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



