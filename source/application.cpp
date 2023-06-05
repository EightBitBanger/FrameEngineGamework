#include "Engine/Engine.h"

extern RandomGen            Random;
extern ColorPreset          Colors;
extern Timer                Time;
extern Timer                PhysicsTime;
extern Logger               Log;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;






// Global resource pointers
Mesh*           projectileMesh;
Material*       projectionMaterial;

Material*       barrelMaterial;
GameObject*     cameraController;
GameObject*     skyObject;

rp3d::BoxShape* projectileCollider;
GameObject* objectA;
GameObject* objectB;
GameObject* objectC;
GameObject* objectD;


//
// Application entry point
//

void Framework::Start() {
    
    // Load some external resources
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    Resources.LoadTexture("data/skull.png",  "mat_skull");
    
    Resources.LoadWaveFront("data/marine.obj", "marine");
    Resources.LoadWaveFront("data/projectile/projectile.obj", "bullet");
    Resources.LoadWaveFront("data/skull.obj", "skull");
    
    Resources.LoadTexture("data/projectile/projectile.png", "mat_bullet");
    
    //
    // Create objects from resource tags
    Material* groundMaterial = Resources.CreateMaterialFromTag("mat_grassy");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    barrelMaterial->diffuse = Color(0.02, 0.02, 0.02);
    groundMaterial->diffuse = Color(0.02, 0.02, 0.02);
    
    Mesh* barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.white);
    
    //
    // Projectile object
    
    projectileMesh = Resources.CreateMeshFromTag("skull");
    projectileMesh->ChangeSubMeshColor(0, Colors.yellow);
    
    projectionMaterial = Resources.CreateMaterialFromTag("mat_skull");
    projectionMaterial->diffuse = Color(0.02, 0.02, 0.02);
    projectileCollider = Physics.CreateColliderBox(2, 2, 2);
    
    
    //
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -9.81 * 2 * 2, 0);
    
    //
    // Create a sky object
    Resources.LoadWaveFront("data/sky/sky.obj", "skyBox");
    Resources.LoadTexture(  "data/sky/sky.png", "skyMaterial");
    
    Mesh* skyMesh = Resources.CreateMeshFromTag("skyBox");
    Material* skyMaterial = Resources.CreateMaterialFromTag("skyMaterial");
    
    skyObject = Engine.CreateGameObject();
    skyObject->name = "sky";
    Component* skyComponent = Engine.CreateComponentEntityRenderer(skyMesh, skyMaterial);
    skyObject->AddComponent(skyComponent);
    
    skyObject->transform.SetScale(10000,10000,10000);
    
    //
    // Create a ground plain
    Mesh* groundMesh = Renderer.CreateMesh();
    groundMesh->AddPlainSubDivided(-100, 0, -100, 10, 10, Colors.white, 100, 100);
    
    GameObject* ground = Engine.CreateGameObject();
    ground->name = "world";
    
    // Add a render component
    Component* groundRenderer = Engine.CreateComponentEntityRenderer(groundMesh, groundMaterial);
    ground->AddComponent(groundRenderer);
    
    // Add a physics component
    Component* groundRigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
    ground->AddComponent(groundRigidBodyComponent);
    ground->SetStatic();
    
    // Lock the ground plain in place
    ground->SetLinearAxisLockFactor(0, 0, 0);
    ground->SetAngularAxisLockFactor(0, 0, 0);
    
    // Ground collider
    rp3d::Vector3 groundScale = rp3d::Vector3(100000, 10, 100000);
    rp3d::BoxShape* groundCollider = Physics.CreateColliderBox(groundScale.x, groundScale.y, groundScale.z);
    ground->AddColliderBox(groundCollider, -(groundScale.x / 2), -10, -(groundScale.z / 2));
    
    
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(glm::vec3(-50, 20, 0));
    cameraController->SetLinearDamping(1);
    cameraController->SetMass(40);
    
    // Camera physics
    cameraController->CalculatePhysics();
    cameraController->EnableGravity(false);
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 8, 1);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    cameraController->CalculatePhysics();
    cameraController->SetLinearDamping(3);
    
    // Sky will follow the camera parent object
    skyObject->parent = cameraController;
    
    
    
    //
    // Testing transformation matrix parent chain
    //
    
    
    // Object A
    objectA = Engine.CreateGameObject();
    objectA->name = "testobject";
    Component* entityRendererA = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    objectA->AddComponent(entityRendererA);
    
    // Object B
    objectB = Engine.CreateGameObject();
    objectB->name = "testobject";
    Component* entityRendererB = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    objectB->AddComponent(entityRendererB);
    
    // Object C
    objectC = Engine.CreateGameObject();
    objectC->name = "testobject";
    Component* entityRendererC = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    objectC->AddComponent(entityRendererC);
    
    // Object D
    objectD = Engine.CreateGameObject();
    objectD->name = "testobject";
    Component* entityRendererD = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    objectD->AddComponent(entityRendererD);
    
    Component* lightComponent = Engine.CreateComponent(ComponentType::Light);
    objectD->AddComponent(lightComponent);
    Light* lightPtr = (Light*)lightComponent->GetComponent();
    lightPtr->color = Colors.white;
    lightPtr->intensity    = 300.0;
    lightPtr->range        = 10000.0;
    lightPtr->attenuation  = 0.001;
    
    
    
    
    objectB->parent = objectA;
    objectC->parent = objectB;
    objectD->parent = objectC;
    
    objectA->transform.position = glm::vec3(0, 10, 0);
    objectB->transform.position = glm::vec3(10, 10, 0);
    objectC->transform.position = glm::vec3(10, 10, 0);
    objectD->transform.position = glm::vec3(10, 10, 0);
    
    return;
}







//
// Application main loop
//

float rotate = 0;

void Framework::Run() {
    
    rotate += 1;
    
    //objectA->transform.RotateEuler(  rotate, 0, 0);
    //objectB->transform.RotateEuler(  rotate, 0, 0);
    //objectD->transform.RotateEuler(  rotate*2, 0, 0);
    
    
    
    
    glm::vec3 force(0);
    
    // Keyboard movement, WASD keys
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Space and shift for elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed multiplier
    force *= 8000;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    // Shoot object from camera
    
    if (Input.CheckMouseLeftPressed()) {
        //Input.SetMouseLeftPressed(false);
        
        // Spread offset effect on projectile angle
        float spreadMul = 0.0001;
        
        for (int i=0; i < 1; i++) {
            
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
            
            // Total forward force + camera offset distance
            fwd *= 1500;
            
            // Camera height offset
            float fireFromHeightOffset = -2;
            
            float startx = pos.x + offsetx;
            float starty = pos.y + offsety + fireFromHeightOffset;
            float startz = pos.z + offsetz;
            
            
            GameObject* projectile = Engine.CreateGameObject();
            projectile->name = "projectile";
            projectile->transform.scale = glm::vec3(3, 3, 3);
            
            // Add a render component
            Component* entityRenderer = Engine.CreateComponent(ComponentType::Renderer);
            projectile->AddComponent(entityRenderer);
            // Set the render component to a loaded resource
            Entity* entity = (Entity*)entityRenderer->GetComponent();
            entity->AttachMesh(projectileMesh);
            entity->AttachMaterial(projectionMaterial);
            
            
            // Light component
            if (Random.Range(0, 10) > 3) {
                Component* lightComponent = Engine.CreateComponent(ComponentType::Light);
                projectile->AddComponent(lightComponent);
                Light* lightPtr = (Light*)lightComponent->GetComponent();
                lightPtr->color = Colors.black;
                lightPtr->intensity    = 10.0;
                lightPtr->range        = 40.0;
                lightPtr->attenuation  = 0.01;
            }
            
            // Add a physics component
            Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
            projectile->AddComponent(rigidBodyComponent);
            rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
            
            // Projectile collider
            projectile->AddColliderBox(projectileCollider, 0, 0, 0);
            
            projectile->SetMass(10);
            projectile->SetLinearDamping(0.001);
            projectile->SetAngularDamping(2);
            projectile->CalculatePhysics();
            
            
            // Transform the rigid body and apply force
            rp3d::Transform newTransform;
            newTransform.setPosition(rp3d::Vector3(startx, starty, startz));
            
            rp3d::Quaternion quat;
            quat.setAllValues(fwdAngle.x + offsetx, fwdAngle.y + offsety, fwdAngle.z + offsetz, -0.5);
            
            newTransform.setOrientation(quat);
            
            body->setTransform(newTransform);
            projectile->AddForce(fwd.x, fwd.y, fwd.z);
            
            projectile->SetPosition(startx, starty, startz);
            
            continue;
        }
        
        
    }
    
    // Purge extra objects
    unsigned int index=0;
    while (Engine.GetGameObjectCount() > 300) {
        
        GameObject* gameObject = Engine.GetGameObject(index);
        index++;
        
        // Cull projectile objects
        if (gameObject->name != "projectile") 
            continue;
        
        Engine.DestroyGameObject(gameObject);
        index = 0;
    }
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        // Uncomment to make the escape key close the application
        //Application.isActive = false;
        //return;
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            if (Renderer.cameraMain != nullptr) 
                Renderer.cameraMain->DisableMouseLook();
            
            Input.ClearKeys();
        } else {
            
            if (Renderer.cameraMain != nullptr) {
                // Reset the camera`s mouse reset position
                Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
                Renderer.cameraMain->EnableMouseLook();
            }
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    return;
}




void Framework::Shutdown(void) {
    
    return;
}





