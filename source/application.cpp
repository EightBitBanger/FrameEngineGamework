#include "Engine/Engine.h"


extern RandomGen            Random;
extern ColorPreset          Colors;
extern Timer                Time;
extern Timer                PhysicsTime;
extern Logger               Log;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern Serialization        Serializer;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;


// Global resource pointers
Mesh*           projectileMesh;

Material*       projectionMaterial;

GameObject*     cameraController;
GameObject*     skyObject;

rp3d::SphereShape* projectileCollider;

GameObject* objectA;
GameObject* objectB;
GameObject* objectC;
GameObject* objectD;



//
// Application entry point
//



void Framework::Start() {
    
    // Load some external resources
    Resources.LoadWaveFront("data/chunk.obj", "chunk");
    Resources.LoadTexture("data/chunk.png", "mat_chunk");
    
    Resources.LoadWaveFront("data/projectile/projectile.obj", "bullet");
    Resources.LoadTexture("data/projectile/projectile.png", "mat_bullet");
    
    
    
    
    
    
    
    
    
    //
    // Projectile object
    
    projectileMesh = Resources.CreateMeshFromTag("bullet");
    projectileMesh->ChangeSubMeshColor(0, Colors.yellow);
    
    projectionMaterial = Resources.CreateMaterialFromTag("mat_bullet");
    projectionMaterial->diffuse = Color(0.02f, 0.02f, 0.02f);
    
    projectileCollider = Physics.CreateColliderSphere(1);
    
    
    
    
    //
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0.0f, -9.81f * 2 * 2, 0);
    
    
    
    //
    // Create a sky object
    Resources.LoadWaveFront("data/sky/sky.obj", "skyBox");
    Resources.LoadTexture("data/sky/sky.png",   "skyMaterial");
    
    Mesh* skyMesh = Resources.CreateMeshFromTag("skyBox");
    Material* skyMaterial = Resources.CreateMaterialFromTag("skyMaterial");
    
    skyObject = Engine.CreateGameObject();
    skyObject->name = "sky";
    Component* skyComponent = Engine.CreateComponentMeshRenderer(skyMesh, skyMaterial);
    skyObject->AddComponent(skyComponent);
    
    skyObject->transform.SetScale(10000,10000,10000);
    
    
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(glm::vec3(0, 10, 0));
    cameraController->SetLinearDamping(1);
    cameraController->SetMass(40);
    
    
    // Camera physics
    BoxShape* boxShape = Physics.CreateColliderBox(1, 8, 1);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    cameraController->CalculatePhysics();
    
    cameraController->EnableGravity(false);
    cameraController->SetLinearDamping(3);
    
    // Sky will follow the camera parent object
    skyObject->parent = cameraController;
    
    
    float chunkSz = 100;
    
    
    
    
    
    
    
    
    
    
    
    
    float noiseWidth  = 0.1f;
    float noiseHeight = 0.1f;
    float noiseMul    = 2.0;
    
    int areaWidth  = 50;
    int areaHeight = 50;
    
    
    for (int z=0; z < areaHeight; z++) {
        
        for (int x=0; x < areaWidth; x++) {
            
            Mesh* chunkMesh = Resources.CreateMeshFromTag("chunk");
            Material* chunkMaterial = Resources.CreateMaterialFromTag("mat_chunk");
            
            float noiseX = x * noiseWidth;
            float noiseZ = z * noiseHeight;
            
            
            
            float newColor = Random.Perlin(x * noiseWidth, 0, z * noiseHeight) * noiseMul;
            
            
            
            chunkMaterial->diffuse = Color(newColor, newColor, newColor);
            
            GameObject* newChunk = Engine.CreateGameObject();
            newChunk->AddComponent( Engine.CreateComponentMeshRenderer(chunkMesh, chunkMaterial) );
            
            newChunk->isActive = true;
            
            // Add a physics component
            newChunk->AddComponent( Engine.CreateComponent(ComponentType::RigidBody) );
            
            // Lock in place
            newChunk->SetStatic();
            newChunk->SetLinearAxisLockFactor(0, 0, 0);
            newChunk->SetAngularAxisLockFactor(0, 0, 0);
            
            // Collider
            BoxShape* chunkCollider = Physics.CreateColliderBox(chunkSz, 10, chunkSz);
            newChunk->AddColliderBox(chunkCollider, 0, -10, 0);
            
            
            newChunk->SetPosition((x * chunkSz) - ((areaWidth/2) * chunkSz), 0.0f, (z * chunkSz) - ((areaHeight/2) * chunkSz));
            newChunk->transform.SetScale(1, 1, 1);
            
            
            //const int nbVertices = 8;
            //const int nbTriangles = 12;
            //float vertices [3 * nbVertices ] = ...;
            //int indices [3 * nbTriangles ] = ...;
            //TriangleVertexArray * triangleArray =
            //new TriangleVertexArray ( nbVertices , vertices , 3 * sizeof (
            //float ) , nbTriangles ,
            //indices , 3 * sizeof ( int ) ,
            //TriangleVertexArray :: VertexDataType :: VERTEX_FLOAT_TYPE ,
            //TriangleVertexArray :: IndexDataType :: INDEX_INTEGER_TYPE ) ;
            
        }
    }
    
    
    
    
    return;
}










// Projectile shoot force
float projectileSpeed = 700;

// Camera movement force
float cameraSpeed     = 80000;

float forceBuild    = 0;
float forceBuildMax = 1000;



//
// Application loop
//

void Framework::Run() {
    
    // Keyboard movement, WASD keys
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Space and shift for elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed multiplier
    force *= cameraSpeed;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    
    
    //
    // Shoot object from camera
    //
    
    if (Input.CheckMouseLeftPressed()) {
        if (forceBuild < forceBuildMax) {
            forceBuild += 0.87;
        } else {
            forceBuild = forceBuildMax;
        }
    }
    
    if (Input.CheckMouseLeftReleased()) {
        Input.SetMouseLeftReleased(false);
        
        // Spread offset effect on projectile angle
        float spreadMul = 0.0001f;
        
        // Apply some random physical forces
        float offsetx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float offsety = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float offsetz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        
        
        //
        // Calculate projectile force from camera forward angle
        
        glm::vec3 fwd = Renderer.cameraMain->forward;
        glm::vec3 fwdAngle = Renderer.cameraMain->forward;
        
        // Offset starting distance from camera
        fwd *= 9;
        
        glm::vec3 pos(0);
        pos = Renderer.cameraMain->transform.GetPosition();
        pos += fwd;
        
        // Total forward force
        fwd *= projectileSpeed * forceBuild;
        
        // Camera height offset
        float fireFromHeightOffset = -2;
        
        float startx = pos.x + offsetx;
        float starty = pos.y + offsety + fireFromHeightOffset;
        float startz = pos.z + offsetz;
        
        
        GameObject* projectile = Engine.CreateGameObject();
        projectile->name = "projectile";
        projectile->transform.scale = glm::vec3(3, 3, 3);
        
        // Add a render component
        Component* meshRenderer = Engine.CreateComponent(ComponentType::Renderer);
        projectile->AddComponent(meshRenderer);
        
        // Set the render component to a loaded resource
        Entity* entity = (Entity*)meshRenderer->GetComponent();
        entity->AttachMesh(projectileMesh);
        entity->AttachMaterial(projectionMaterial);
        
        // Light component
        Component* lightComponent = Engine.CreateComponent(ComponentType::Light);
        projectile->AddComponent(lightComponent);
        Light* lightPtr = (Light*)lightComponent->GetComponent();
        lightPtr->color = Colors.green + Colors.Make(0.0f, 0.3f, 0.0f);
        lightPtr->intensity    = 70.0f;
        lightPtr->range        = 100.0f;
        lightPtr->attenuation  = 0.01f;
        
        // Add a physics component
        Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
        projectile->AddComponent(rigidBodyComponent);
        RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
        
        // Projectile collider
        projectile->AddColliderSphere(projectileCollider, 0, 0, 0);
        
        projectile->SetMass(10);
        projectile->SetLinearDamping(0.001);
        projectile->SetAngularDamping(2);
        projectile->CalculatePhysics();
        
        
        // Transform the rigid body and apply force
        rp3d::Transform newTransform;
        newTransform.setPosition(rp3d::Vector3(startx, starty, startz));
        
        rp3d::Quaternion quat;
        quat.setAllValues(fwdAngle.x + offsetx, fwdAngle.y + offsety, fwdAngle.z + offsetz, -0.5f);
        
        newTransform.setOrientation(quat);
        
        body->setTransform(newTransform);
        projectile->AddForce(fwd.x, fwd.y, fwd.z);
        
        projectile->SetPosition(startx, starty, startz);
        
        
        // Reset built up force
        forceBuild = 0;
    }
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            Renderer.cameraMain->DisableMouseLook();
            ShowCursor(true);
            
            Input.ClearKeys();
        } else {
            
            Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            Renderer.cameraMain->EnableMouseLook();
            ShowCursor(false);
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    return;
}




void Framework::Shutdown(void) {
    
    return;
}



