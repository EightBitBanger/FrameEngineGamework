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
extern PlatformLayer        Platform;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern MathCore             Math;
extern ActorSystem          AI;



// User functions
void RenderPhysicsDebugger(void);
void InitiatePhysicsDebugger(void);




// User globals
Scene* sceneOverlay;

GameObject*  cameraController;

GameObject* directionalLight;
Transform*  lightTransform;

Text* text[20];

Transform* bendJoint = nullptr;


Actor* testActor;




MeshRenderer* combineRenderer;
SubMesh submesh;





// Day night cycle

bool cycleDirection = false;

float ambientLight = 0.3;

Material* skyMaterial;
Material* plainMaterial;






// Application entry point
//

void Start() {
    
    //Engine.EnablePhysicsDebugRenderer();
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    
    Transform* skyTransform = skyObject->GetComponent<Transform>();
    
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer, RENDER_QUEUE_SKY );
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(-400, 100, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
    
    // Attach the sky object to the camera controller
    Transform* cameraTransform = cameraController->GetComponent<Transform>();
    skyTransform->parent = cameraTransform;
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 1 );
    cameraController->SetMass( 10 );
    cameraController->DisableGravity();
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    
    
    //
    // Directional light
    directionalLight = Engine.Create<GameObject>();
    lightTransform = directionalLight->GetComponent<Transform>();
    lightTransform->RotateAxis(1, Vector3(0.8, -0.7, 0.3));
    
    directionalLight->AddComponent( Engine.CreateComponent<Light>() );
    Light* sunLight = directionalLight->GetComponent<Light>();
    
    Engine.sceneMain->AddLightToSceneRoot( sunLight );
    sunLight->intensity  = 1;
    sunLight->type       = LIGHT_TYPE_DIRECTIONAL;
    sunLight->color      = Colors.white;
    
    
    
    //
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
    for (int i=0; i < 20; i++) {
        GameObject* textObject = Engine.CreateOverlayTextRenderer(0, 0, "", 9, Colors.white, "font");
        
        sceneOverlay->AddMeshRendererToSceneRoot( textObject->GetComponent<MeshRenderer>() );
        
        text[i] = textObject->GetComponent<Text>();
        text[i]->canvas.anchorTop = true;
        
        text[i]->canvas.x = 0;
        text[i]->canvas.y = 2 * i + 4;
    }
    
    
    
    //
    // Profiler overlay renderer
    //
    
    /*
    
    GameObject* panelObject = Engine.CreateOverlayPanelRenderer(100, 100, 100, 80, "panel_blue");
    MeshRenderer* panelRenderer = panelObject->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot( panelRenderer, RENDER_QUEUE_OVERLAY );
    
    float alphaBlend = 0.5;
    panelRenderer->material->ambient.g = alphaBlend;
    
    Panel* panel = panelObject->GetComponent<Panel>();
    */
    
    
    
    
    
    
    
    
    //
    // Generate some ground chunks
    //
    
    // Chunk material
    
    plainMaterial = Engine.Create<Material>();
    
    TextureTag* plainTexture = Resources.FindTextureTag("grassy");
    plainTexture->Load();
    
    plainMaterial->texture.UploadTextureToGPU( plainTexture->buffer, plainTexture->width, plainTexture->height, MATERIAL_FILTER_ANISOTROPIC );
    
    plainMaterial->shader = Engine.shaders.color;
    
    plainMaterial->DisableCulling();
    
    
    
    
    // Chunk mesh
    
    Mesh* chunkMesh = Engine.Create<Mesh>();
    
    GameObject* plainObject = Engine.Create<GameObject>();
    plainObject->AddComponent( Engine.CreateComponentMeshRenderer( chunkMesh, plainMaterial ) );
    plainObject->AddComponent( Engine.CreateComponent<RigidBody>() );
    
    MeshRenderer* plainRenderer = plainObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( plainRenderer, RENDER_QUEUE_BACKGROUND );
    plainRenderer->mesh->SetPrimitive(MESH_TRIANGLES);
    
    plainObject->SetAngularAxisLockFactor(0, 0, 0);
    plainObject->SetLinearAxisLockFactor(0, 0, 0);
    plainObject->SetStatic();
    
    
    
    
    //
    // Generate height field map
    //
    
    // World
    int worldSize   = 30;
    
    // Chunk size in multiples of eight
    int chunkSize  = 1;
    
    
    
    
    RigidBody* heightFieldBody = plainObject->GetComponent<RigidBody>();
    
    // Chunk size must be a multiple of eight
    chunkSize *= 8;
    
    // Maps
    float     heightMap[chunkSize * chunkSize];
    glm::vec3 colorMap[chunkSize * chunkSize];
    
    
    for (int x = -worldSize; x <= worldSize; x++) {
        
        for (int z = -worldSize; z <= worldSize; z++) {
            
            float chunkOffsetX = (chunkSize - 1) * x;
            float chunkOffsetZ = (chunkSize - 1) * z;
            
            // Base height field map values
            Engine.SetHeightFieldValues(heightMap, chunkSize, chunkSize, 0);
            
            Engine.SetColorFieldValues(colorMap, chunkSize, chunkSize, Colors.white);
            
            // Apply perlin noise
            
            Engine.GenerateHeightFieldByPerlinNoise(heightMap, chunkSize, chunkSize, 
                                                    0.007, 0.007, 200, 
                                                    chunkOffsetX, chunkOffsetZ);
            
            Engine.GenerateHeightFieldByPerlinNoise(heightMap, chunkSize, chunkSize, 
                                                    0.05, 0.05, 20, 
                                                    chunkOffsetX, chunkOffsetZ);
            
            Engine.GenerateHeightFieldByPerlinNoise(heightMap, chunkSize, chunkSize, 
                                                    0.09, 0.09, 5, 
                                                    chunkOffsetX, chunkOffsetZ);
            
            
            
            // Apply terrain color by height value
            Color colorLow = Colors.green;
            colorLow *= Colors.MakeGrayScale(0.1);
            
            Color colorHigh = Colors.brown;
            
            //Engine.GenerateColorFieldFromHeightField(colorMap, heightMap, chunkSize, chunkSize, colorLow, colorHigh, 0.9f);
            
            
            float bias = 0.01;
            
            for (int x=0; x < chunkSize; x ++) {
                
                for (int z=0; z < chunkSize; z ++) {
                    
                    unsigned int index = z * chunkSize + x;
                    
                    if (heightMap[index] < 0) heightMap[index] *= 0.3;
                    
                    float heightPoint = heightMap[index] * bias;
                    
                    Color color = colorLow;
                    
                    if (heightPoint < 0) 
                        heightPoint = 0;
                    
                    if (heightPoint > 1) 
                        heightPoint = 1;
                    
                    color = Colors.Lerp(colorLow, colorHigh, heightPoint);
                    
                    colorMap[index] = glm::vec3( color.r, color.g, color.b );
                    
                    continue;
                }
                
                continue;
            }
            
            
            
            
            
            
            
            
            // Generate a height field collider
            
            //MeshCollider* collider = Physics.CreateHeightFieldMap(heightMap, chunkSize, chunkSize);
            
            //rp3d::Transform offsetTransform;
            //offsetTransform.setPosition(rp3d::Vector3((chunkSize - 1) * x, 0, (chunkSize - 1) * z));
            
            //rp3d::Collider* colliderBody = heightFieldBody->addCollider( collider->heightFieldShape, offsetTransform );
            
            
            // Add the chunk to the mesh
            Engine.AddHeightFieldToMesh(chunkMesh, heightMap, colorMap, chunkSize, chunkSize, chunkOffsetX, chunkOffsetZ);
            
            continue;
        }
        
        continue;
    }
    
    chunkMesh->UploadToGPU();
    
    
    
    
    
    return;
    
    
    
    //
    // Generate AI actors
    //
    
    unsigned int spread = 40;
    
    
    for (unsigned int i=0; i < 500; i++) {
        
        float xx = (Random.Range(0, spread) * 0.5) - (Random.Range(0, spread) * 0.5);
        float yy = 10;
        float zz = (Random.Range(0, spread) * 0.5) - (Random.Range(0, spread) * 0.5);
        
        GameObject* actorObject = Engine.CreateAIActor( glm::vec3(xx, yy, zz) );
        
        actorObject->renderDistance = 300;
        
        // Collision
        BoxShape* boxShape = Physics.CreateColliderBox(0.3, 0.3, 0.3);
        actorObject->AddColliderBox(boxShape, 0, 0.3, 0, LayerMask::Actor);
        
        
        // Actor
        Actor* actor = actorObject->GetComponent<Actor>();
        
        // Use sheep actor preset
        AI.genomes.SheepGene( actor );
        
        //actor->SetChanceToWalk(0);
        //actor->SetChanceToChangeDirection(0);
        //actor->SetChanceToStopWalking(0);
        //actor->SetChanceToFocusOnActor(0);
        
        actor->SetActive(false);
        
    }
    
    
    
    return;
}







//
// Application loop
//

glm::vec3 force(0);

void Run() {
    
    //
    // Profiling
    //
    
    text[1]->text = "Renderer - " + Float.ToString( Profiler.profileRenderSystem );
    text[2]->text = "Physics  - " + Float.ToString( Profiler.profilePhysicsSystem );
    text[3]->text = "Engine   - " + Float.ToString( Profiler.profileGameEngineUpdate );
    
    text[4]->text = "Draw calls - " + Float.ToString( Renderer.GetNumberOfDrawCalls() );
    
    text[6]->text = "x - " + Float.ToString( cameraController->GetComponent<Transform>()->position.x );
    text[7]->text = "y - " + Float.ToString( cameraController->GetComponent<Transform>()->position.y );
    text[8]->text = "z - " + Float.ToString( cameraController->GetComponent<Transform>()->position.z );
    
    
    
    
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    float lightingMax       = 0.87;
    float lightingMin       = 0.087;
    
    float skyLightingMax    = 0.87;
    float skyLightingMin    = 0.0034;
    
    float worldLightingMax  = 0.87;
    float worldLightingMin  = 0.34;
    
    
    
    bool adjustCycle = false;
    if (Input.CheckKeyCurrent(VK_I)) {cycleDirection = true;  adjustCycle = true;}
    if (Input.CheckKeyCurrent(VK_K)) {cycleDirection = false; adjustCycle = true;}
    
    if (adjustCycle) {
        if (cycleDirection) {
            ambientLight += 0.01f;
        } else {
            ambientLight -= 0.01f;
        }
    }
    
    
    // Ambient limits
    if (ambientLight > 0.87f) ambientLight = 0.87f;
    if (ambientLight < 0.0f)  ambientLight = 0.0f;
    
    // Sky brightness
    skyMaterial->ambient = Math.Lerp(skyLightingMin, skyLightingMax, ambientLight);
    
    // World brightness
    plainMaterial->diffuse = Math.Lerp(worldLightingMin, worldLightingMax, ambientLight);
    
    
    Light* sunLight = directionalLight->GetComponent<Light>();
    sunLight->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);;
    
    // Light direction
    lightTransform = directionalLight->GetComponent<Transform>();
    
    
    
    //
    // DEBUG - Manually adjust light direction
    //
    
    if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
    if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
    
    
    
    
    
    
    
    
    
    
    
    if (cameraController == nullptr) 
        return;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    float forceAccelerate = 0.03;
    float forceDecelerate = 0.02;
    float forceMax        = 10;
    
    if (mainCamera != nullptr) {
        
        // Directional movement
        bool moving = false;
        if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward; moving = true;}
        if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward; moving = true;}
        if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right; moving = true;}
        if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right; moving = true;}
        
        // Elevation
        if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up; moving = true;}
        if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up; moving = true;}
        
        // Accelerate
        if (glm::length(force) < forceMax) force += (force * forceAccelerate) * glm::vec3(0.1);
        
        // Decelerate
        if ( glm::length(force) >  0.0001) force -= (force * forceDecelerate);
        if (-glm::length(force) < -0.0001) force -= (force * forceDecelerate);
        
        glm::vec3 forceMul = force * forceAccelerate;
        
        // Minimum speed cut off
        if (glm::length(force) < 0.001f) 
            force = glm::vec3(0.0f);
        
        if (moving) 
            if (Input.CheckKeyCurrent(VK_CONTROL)) forceMul *= 2;
        
        // Max force
        if ( glm::length(forceMul) >  forceMax) forceMul = glm::vec3(forceMax);
        if (-glm::length(forceMul) < -forceMax) forceMul = glm::vec3(-forceMax);
        
        if (forceMul != glm::vec3(0)) 
            cameraController->AddForce(forceMul.x, forceMul.y, forceMul.z);
        
        
        
        //
        // Update camera height
        //
        
        RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
        rp3d::Transform bodyTransform = rigidBody->getTransform();
        
        rp3d::Vector3 position = bodyTransform.getPosition();
        
        //position.y = z + 1;
        
        bodyTransform.setPosition(position);
        
        rigidBody->setTransform( bodyTransform );
        
        
    }
    
    
    
    
    
    
    //
    // Pausing
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Platform.Pause();
        
        if (Platform.isPaused) {
            
            mainCamera->DisableMouseLook();
            
            Input.ClearKeys();
            
            Platform.ShowMouseCursor();
            
        } else {
            
            SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
            mainCamera->EnableMouseLook();
            
            Platform.HideMouseCursor();
            
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




