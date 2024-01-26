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




// Height field testing

float* heightField;




// Application entry point
//

void Start() {
    
    // Create the main world scene 
    Engine.sceneMain   = Engine.Create<Scene>();
    Renderer.AddSceneToRenderQueue(Engine.sceneMain);
    
    //
    // Create a sky
    
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
    skyHigh += Colors.MakeGrayScale(0.997);
    skyLow  += Colors.MakeGrayScale(0.799);
    
    skyHigh *= Colors.MakeGrayScale(0.961);
    
    skyHigh *= Colors.MakeGrayScale(0.1);
    skyLow  *= Colors.MakeGrayScale(0.1);
    
    GameObject* skyObject = Engine.CreateSky("sky", skyLow, skyHigh, 1);
    
    Transform* skyTransform = skyObject->GetComponent<Transform>();
    
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyRenderer, RENDER_QUEUE_SKY );
    
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    
    //
    // Create a camera controller
    
    Vector3 position = Vector3(0, 10, 0);
    Vector3 colliderScale = Vector3(1, 1, 1);
    
    cameraController = Engine.CreateCameraController(position, colliderScale);
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    // Attach the sky object to the camera controller
    Transform* cameraTransform = cameraController->GetComponent<Transform>();
    skyTransform->parent = cameraTransform;
    
    cameraController->SetAngularDamping( 1 );
    cameraController->SetLinearDamping( 3 );
    cameraController->SetMass( 10 );
    cameraController->DisableGravity();
    
    
    
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
    Mesh* plainBaseMesh = Resources.CreateMeshFromTag("plain");
    
    SubMesh chunkSubMesh;
    plainBaseMesh->CopySubMesh(0, chunkSubMesh);
    
    // Chunk material
    plainMaterial = Engine.Create<Material>();
    
    TextureTag* plainTexture = Resources.FindTextureTag("grassy");
    plainTexture->Load();
    
    plainMaterial->texture.UploadTextureToGPU( plainTexture->buffer, plainTexture->width, plainTexture->height, MATERIAL_FILTER_ANISOTROPIC );
    
    plainMaterial->shader = Engine.shaders.texture;
    plainMaterial->diffuse = Colors.white;
    plainMaterial->ambient = Colors.white;
    
    plainMaterial->DisableShadowVolumePass();
    
    
    
    
    // Chunk object
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
    
    
    
    
    // Area of chunks
    unsigned int worldWidth  = 100;
    unsigned int worldHeight = 100;
    
    // Points in the mesh
    unsigned int chunkWidth  = 4;
    unsigned int chunkHeight = 4;
    
    // Noise scaling
    float noiseWidth  = 0.7;
    float noiseHeight = 0.7;
    
    float heightMul = 3;
    
    
    // Transform the chunk
    Transform* chunkTransform = plainObject->GetComponent<Transform>();
    chunkTransform->scale = Vector3(2, 2, 2);
    
    
    
    
    
    
    //
    // Generate chunk
    //
    
    
    // Plain collider
    BoxShape* plainCollider = Physics.CreateColliderBox(10000, 10, 10000);
    
    plainObject->AddColliderBox(plainCollider, 0, -10, 0);
    
    
    for (unsigned int x=0; x < worldWidth; x++) {
        
        for (unsigned int z=0; z < worldHeight; z++) {
            
            float chunkX = (float)x - (worldWidth  / 2.0f);
            float chunkZ = (float)z - (worldHeight / 2.0f);
            
            chunkMesh->AddSubMesh(chunkX, 0, chunkZ, chunkSubMesh, false);
            
            
            continue;
            
            
            // Generate perlin
            
            for (unsigned int index=0; index < chunkMesh->GetNumberOfVertices(); index++) {
                
                Vertex vert = chunkMesh->GetVertex(index);
                
                float xCoord = vert.x * noiseWidth;
                float zCoord = vert.z * noiseHeight;
                
                float height = Random.Perlin(xCoord, 0, zCoord) * heightMul;
                
                // Height step effect
                //float heightMul = 1;
                //height = glm::round( height * heightMul ) / heightMul;
                
                if (height < 0) 
                    height = 0;
                
                vert.y = height;
                
                chunkMesh->SetVertex(index, vert);
                
                continue;
            }
            
            
            continue;
        }
        
        continue;
    }
    
    //chunkMesh->CalculateNormals();
    
    chunkMesh->UploadToGPU();
    
    
    
    
    
    //
    // Generate height field map collider
    //
    /*
    
    heightField = new float[ chunkWidth * chunkHeight ];
    unsigned int heightFieldIndex = 0;
    
    
    for (unsigned int x=0; x < worldWidth; x++) {
        
        for (unsigned int z=0; z < worldHeight; z++) {
            
            float chunkX = (x * 1.0f) - ((worldWidth  / 2) * 1.0f);
            float chunkZ = (z * 1.0f) - ((worldHeight / 2) * 1.0f);
            
            chunkMesh->AddSubMesh(chunkX, 0, chunkZ, chunkSubMesh, false);
            
        }
        
    }
    
    */
    
    /*
    
    for (unsigned int i=0; i < chunkMesh->GetNumberOfIndices(); i += 3) {
        
        unsigned int indexA = chunkMesh->GetIndex(i)  .index;
        unsigned int indexB = chunkMesh->GetIndex(i+1).index;
        unsigned int indexC = chunkMesh->GetIndex(i+2).index;
        
        Vertex vertA = chunkMesh->GetVertex(indexA);
        Vertex vertB = chunkMesh->GetVertex(indexB);
        Vertex vertC = chunkMesh->GetVertex(indexC);
        
        heightField[heightFieldIndex] = vertA.y;
        
        heightFieldIndex++;
        continue;
    }
    
    */
    
    
    
    //
    // Generate a physics height field map
    //
    
    /*
    
    rp3d::HeightFieldShape* heightFieldShape = Physics.common.createHeightFieldShape(chunkWidth, chunkHeight, 
                                                                                    -1000, 1000, 
                                                                                     heightField, 
                                                                                     rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
    
    
    
    
    heightFieldShape->setScale(rp3d::Vector3(3, 8, 3));
    
    RigidBody* rigidBody = plainObject->GetComponent<RigidBody>();
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(0, 0, 0));
    
    rp3d::Collider* collider = rigidBody->addCollider( heightFieldShape, offsetTransform );
    
    */
    
    
    
    
    
    
    
    
    
    
    
    //
    // Generate plant life
    //
    
    Mesh* batchRenderer = Engine.Create<Mesh>();
    
    Mesh* crossBaseMesh = Resources.CreateMeshFromTag("treebase");
    Mesh* logMesh       = Resources.CreateMeshFromTag("log");
    
    Material* treeBaseMaterial = Engine.Create<Material>();
    
    
    SubMesh logSubMesh;
    crossBaseMesh->CopySubMesh(0, logSubMesh);
    crossBaseMesh->CopySubMesh(1, logSubMesh);
    
    SubMesh baseSubMesh;
    crossBaseMesh->CopySubMesh(0, baseSubMesh);
    crossBaseMesh->CopySubMesh(1, baseSubMesh);
    
    
    treeBaseMaterial->shader = Engine.shaders.color;
    treeBaseMaterial->diffuse = Colors.white;
    treeBaseMaterial->ambient = Colors.white;
    
    treeBaseMaterial->DisableCulling();
    
    
    GameObject* treeBase = Engine.Create<GameObject>();
    
    treeBase->AddComponent( Engine.CreateComponentMeshRenderer(batchRenderer, treeBaseMaterial) );
    MeshRenderer* treeRenderer = treeBase->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(treeRenderer);
    
    
    unsigned int numberOfLeaves = 5;
    unsigned int treeSpread     = 800;
    
    float leafHeightOffset  = 3;
    float treeBaseHeightMin = 3;
    float treeBaseHeightMax = 8;
    float leafOffset        = 0.01;
    
    Color treeBaseColor;
    Color leafColor;
    treeBaseColor = Colors.red * Colors.green;
    leafColor     = Colors.green * Colors.dkgray;
    
    for (unsigned int i=0; i < 1000; i++) {
        
        float xx = ((Random.Range(0, treeSpread) * 0.1) - (Random.Range(0, treeSpread) * 0.1));
        float yy = 0;
        float zz = ((Random.Range(0, treeSpread) * 0.1) - (Random.Range(0, treeSpread) * 0.1));
        
        unsigned int index = batchRenderer->AddSubMesh(xx, yy, zz, baseSubMesh, false);
        
        for (unsigned int a=0; a < 2; a++) {
            
            unsigned indexOffset = index + (a * 4);
            
            Vertex vertA = batchRenderer->GetVertex(indexOffset);
            Vertex vertB = batchRenderer->GetVertex(indexOffset+1);
            Vertex vertC = batchRenderer->GetVertex(indexOffset+2);
            Vertex vertD = batchRenderer->GetVertex(indexOffset+3);
            
            if (vertA.y > 0) vertA.y += Random.Range(treeBaseHeightMin, treeBaseHeightMax);
            if (vertB.y > 0) vertB.y += Random.Range(treeBaseHeightMin, treeBaseHeightMax);
            if (vertC.y > 0) vertC.y += Random.Range(treeBaseHeightMin, treeBaseHeightMax);
            if (vertD.y > 0) vertD.y += Random.Range(treeBaseHeightMin, treeBaseHeightMax);
            
            if (vertA.r > 0) vertA.r = treeBaseColor.r;
            if (vertB.r > 0) vertB.r = treeBaseColor.r;
            if (vertC.r > 0) vertC.r = treeBaseColor.r;
            if (vertD.r > 0) vertD.r = treeBaseColor.r;
            
            if (vertA.g > 0) vertA.g = treeBaseColor.g;
            if (vertB.g > 0) vertB.g = treeBaseColor.g;
            if (vertC.g > 0) vertC.g = treeBaseColor.g;
            if (vertD.g > 0) vertD.g = treeBaseColor.g;
            
            if (vertA.b > 0) vertA.b = treeBaseColor.b;
            if (vertB.b > 0) vertB.b = treeBaseColor.b;
            if (vertC.b > 0) vertC.b = treeBaseColor.b;
            if (vertD.b > 0) vertD.b = treeBaseColor.b;
            
            batchRenderer->SetVertex(indexOffset,   vertA);
            batchRenderer->SetVertex(indexOffset+1, vertB);
            batchRenderer->SetVertex(indexOffset+2, vertC);
            batchRenderer->SetVertex(indexOffset+3, vertD);
            
            continue;
        }
        
        
        continue;
        
        
        //
        // Generate leaves
        
        for (unsigned int a=0; a < numberOfLeaves; a++) {
            
            float xl = xx + ((Random.Range(0, treeSpread) * 0.1) - (Random.Range(0, treeSpread) * 0.1)) * leafOffset;
            float yl = yy + ((Random.Range(0, treeSpread) * 0.1) - (Random.Range(0, treeSpread) * 0.1)) * leafOffset;
            float zl = zz + ((Random.Range(0, treeSpread) * 0.1) - (Random.Range(0, treeSpread) * 0.1)) * leafOffset;
            
            unsigned int index = batchRenderer->AddSubMesh(xl, yl + leafHeightOffset, zl, baseSubMesh, false);
            
            for (unsigned int b=0; b < 2; b++) {
                
                unsigned indexOffset = index + (b * 4);
                
                Vertex vertA = batchRenderer->GetVertex(indexOffset);
                Vertex vertB = batchRenderer->GetVertex(indexOffset+1);
                Vertex vertC = batchRenderer->GetVertex(indexOffset+2);
                Vertex vertD = batchRenderer->GetVertex(indexOffset+3);
                
                if (vertA.r > 0) vertA.r = leafColor.r;
                if (vertB.r > 0) vertB.r = leafColor.r;
                if (vertC.r > 0) vertC.r = leafColor.r;
                if (vertD.r > 0) vertD.r = leafColor.r;
                
                if (vertA.g > 0) vertA.g = leafColor.g;
                if (vertB.g > 0) vertB.g = leafColor.g;
                if (vertC.g > 0) vertC.g = leafColor.g;
                if (vertD.g > 0) vertD.g = leafColor.g;
                
                if (vertA.b > 0) vertA.b = leafColor.b;
                if (vertB.b > 0) vertB.b = leafColor.b;
                if (vertC.b > 0) vertC.b = leafColor.b;
                if (vertD.b > 0) vertD.b = leafColor.b;
                
                batchRenderer->SetVertex(indexOffset,   vertA);
                batchRenderer->SetVertex(indexOffset+1, vertB);
                batchRenderer->SetVertex(indexOffset+2, vertC);
                batchRenderer->SetVertex(indexOffset+3, vertD);
                
            }
            
            continue;
        }
        
        
        
        
        
        
        
        /*
        
        
        
        
        
        
        
        float leafNoise   = 0.008;
        float leafHeight  = 0.07;
        
        //Vertex vertA = batchRenderer->GetVertex(index);
        //Vertex vertB = batchRenderer->GetVertex(index+1);
        //Vertex vertC = batchRenderer->GetVertex(index+2);
        //Vertex vertD = batchRenderer->GetVertex(index+3);
        
        
        float randomHeight = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        float leafX  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        float leafZ  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        
        vertA.x += leafX;
        vertA.y += randomHeight;
        vertA.z += leafZ;
        
        
        randomHeight = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafX  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafZ  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        
        vertB.x += leafX;
        vertB.y += randomHeight;
        vertB.z += leafZ;
        
        
        randomHeight = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafX  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafZ  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        
        vertC.x += leafX;
        vertC.y += randomHeight;
        vertC.z += leafZ;
        
        
        randomHeight = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafX  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        leafZ  = (Random.Range(0.0f, 10.0f) - Random.Range(0.0f, 10.0f)) * leafHeight;
        
        vertD.x += leafX;
        vertD.y += randomHeight;
        vertD.z += leafZ;
        
        batchRenderer->SetVertex(index,   vertA);
        batchRenderer->SetVertex(index+1, vertB);
        batchRenderer->SetVertex(index+2, vertC);
        batchRenderer->SetVertex(index+3, vertD);
        */
        
    }
    
    //batchRenderer->CalculateNormals();
    
    batchRenderer->UploadToGPU();
    
    
    
    
    
    
    
    
    
    //
    // Generate AI actors
    //
    
    unsigned int spread = 100;
    
    
    for (unsigned int i=0; i < 100; i++) {
        
        float xx = Random.Range(0, spread) - Random.Range(0, spread);
        float yy = 10;
        float zz = Random.Range(0, spread) - Random.Range(0, spread);
        
        GameObject* actorObject = Engine.CreateAIActor( glm::vec3(xx, yy, zz) );
        
        actorObject->renderDistance = 300;
        
        // Collision
        BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
        actorObject->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Actor);
        
        // Actor
        Actor* actor = actorObject->GetComponent<Actor>();
        
        // Use sheep actor preset
        AI.genomes.SheepGene( actor );
        
        actor->SetActive(false);
        
    }
    
    
    
    return;
}

















//
// Application loop
//

glm::vec3 sunDir(0, 0, 1);
float sunStep = 0.5;
float sunRate = 1;


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
    
    bool adjustCycle = false;
    
    if (Input.CheckKeyCurrent(VK_I)) {cycleDirection = true;  adjustCycle = true;}
    if (Input.CheckKeyCurrent(VK_K)) {cycleDirection = false; adjustCycle = true;}
    
    
    //ambientLight += 0.01f;
    
    if (adjustCycle) {
        
        if (cycleDirection) {
            
            ambientLight += 0.03f;
            
        } else {
            
            ambientLight -= 0.03f;
            
        }
        
    }
    
    // Ambient limits
    if (ambientLight > 1.1)   ambientLight = 1.1;
    if (ambientLight < 0.0087) ambientLight = 0.0087;
    
    skyMaterial->ambient = ambientLight;
    
    Light* sunLight = directionalLight->GetComponent<Light>();
    sunLight->intensity = ambientLight * 0.87;
    
    // Light direction
    lightTransform = directionalLight->GetComponent<Transform>();
    
    if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
    if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
    
    
    
    
    
    
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
    //text[8]->text  = Float.ToString( direction.x );
    //text[9]->text  = Float.ToString( direction.y );
    //text[10]->text = Float.ToString( direction.z );
    
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













