## Introduction

This game engine framework aims to provide simpler interfaces with more complex systems. The application layer contains an entry point function and numerous APIs for rendering, physics, resource management, scene management, input, timing and random number generation.


<br><br/>
### Core Features
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Render%E2%80%89System">Render system</a>⁭
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Resource%E2%80%89Management">Resource management</a>⁭
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Input%E2%80%89System">Input handling</a>⁭

<br><br/>
## Screenshots
You can find more screenshots [here](https://github.com/RetroBytes32/GameEngineFramework/wiki/Screenshots).

### Vertex lighting demo
<img src="https://github.com/RetroBytes32/GameEngineFramework/blob/main/demo/vertex_lighting_demo_2.jpg"  width="800" height="600">


<br><br/>
## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows


<br><br/>
##  Building the project
First, clone the repository with git. You can then use cmake to generate the project files of your choice.

```
git clone https://github.com/RetroBytes32/GameEngineFramework
```


### Dependencies
Building this repository will require the following libraries to be installed.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/Groovounet/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭

### Required exteral linkage
```
             Windows                    OpenGL           Physics
 _______________________________    _______________    ____________
gdi32  user32  kernel32  comctl32  glew32s  opengl32  reactphysics3d
```


<br><br/>
## Documentation
You can find the API documentation <a href="https://github.com/RetroBytes32/GameEngine-Framework/wiki">here</a>⁭.


<br><br/>
## Singleton access pointers
The framework contains many sub systems which can be accessed though the following singletons.

```c++
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
```


<br><br/>
## Application entry point
The source file "Application.cpp" is the application layer entry point and the starting point for your application code.
The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
The `Start()` function will be called once during application initiation.

```c++
// This example will load required resources and initiate the scene

// Global resources that 'run()' must know about
Mesh*           projectileMesh;
Material*       barrelMaterial;
GameObject*     cameraController;
GameObject*     skyObject;
rp3d::BoxShape* projectileCollider;

void Framework::Start() {
    
    // Load some external resources
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -9.81, 0);
    
    // Create objects from resource tags
    Material* groundMaterial = Resources.CreateMaterialFromTag("mat_grassy");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    barrelMaterial->diffuse = Color(0.01, 0.01, 0.01);
    groundMaterial->diffuse = Color(0.01, 0.01, 0.01);
    
    Mesh* barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.white);
    
    projectileMesh = Resources.CreateMeshFromTag("barrel");
    projectileMesh->ChangeSubMeshColor(0, Colors.white);
    
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
    
    skyObject->transform.scale = glm::vec3(10000,10000,10000);
    
    //
    // Create a ground plain
    Mesh* groundMesh = Renderer.CreateMesh();
    groundMesh->AddPlainSubDivided(-500, 0, -500, 10, 10, Colors.white, 100, 100);
    
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
    rp3d::BoxShape* groundCollider = Physics.CreateColliderBox(100000, 10, 100000);
    ground->AddColliderBox(groundCollider, -10000, -10, -10000);
    
    //
    // Create a camera controller
    cameraController = Engine.CreateCameraController(-50, 20, 0);
    cameraController->SetLinearDamping(3);
    cameraController->SetMass(100);
    
    cameraController->CalculatePhysics();
    cameraController->EnableGravity(false);
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 8, 1);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    cameraController->CalculatePhysics();
    
    //
    // Create the projectile collider
    projectileCollider = Physics.CreateColliderBox(2, 4, 2);
    
}
```

<br><br/>
### Main loop
The `Run()` function will be called once per frame.

```c++
// This example will apply force to the camera`s rigid body on key presses
// and allow you to throw objects along the direction angle of the camera.

void Framework::Run() {
    
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
    force *= 100000;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    // Update the sky object
    skyObject->transform.position = cameraController->transform.position;
    
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
            fwd *= 13;
            
            glm::vec3 pos(0);
            pos = Renderer.cameraMain->transform.position;
            pos += fwd;
            
            // Total forward force + camera offset distance
            fwd *= 100000;
            
            float startx = pos.x + offsetx;
            float starty = pos.y + offsety - 5;
            float startz = pos.z + offsetz;
            
            
            
            GameObject* projectile = Engine.CreateGameObject();
            projectile->name = "projectile";
            
            
            
            // Add a render component
            Component* entityRenderer = Engine.CreateComponent(ComponentType::Renderer);
            projectile->AddComponent(entityRenderer);
            // Set the render component to a loaded resource
            Entity* entity = (Entity*)entityRenderer->GetComponent();
            entity->AttachMesh(projectileMesh);
            entity->AttachMaterial(barrelMaterial);
            
            
            // Light component test
            Component* lightComponent = Engine.CreateComponent(ComponentType::Light);
            projectile->AddComponent(lightComponent);
            Light* lightPtr = (Light*)lightComponent->GetComponent();
            lightPtr->color = Colors.MakeRandom();
            lightPtr->intensity    = 40.0;
            lightPtr->range        = 100.0;
            lightPtr->attenuation  = 0.002;
            
            
            // Add a physics component
            Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
            projectile->AddComponent(rigidBodyComponent);
            rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
            
            // Projectile collider
            projectile->AddColliderBox(projectileCollider, 0, 0, 0);
            
            projectile->SetMass(0.001);
            projectile->SetLinearDamping(0.001);
            projectile->SetAngularDamping(0.003);
            projectile->CalculatePhysics();
            
            
            // Transform the rigid body and apply force
            rp3d::Transform newTransform;
            newTransform.setPosition(rp3d::Vector3(startx, starty, startz));
            
            rp3d::Quaternion quat;
            quat.setAllValues(fwdAngle.x + offsetx, fwdAngle.y + offsety, fwdAngle.z + offsetz, 0);
            
            newTransform.setOrientation(quat);
            
            body->setTransform(newTransform);
            projectile->AddForce(fwd.x, fwd.y, fwd.z);
            
            
            
            projectile->SetPosition(startx, starty, startz);
            
            continue;
        }
        
        
    }
    
    // Purge extra objects
    unsigned int index=0;
    while (Engine.GetGameObjectCount() > RENDER_NUMBER_OF_LIGHTS) {
        
        GameObject* gameObject = Engine.GetGameObject(index);
        index++;
        
        // Ignore cameras and world objects
        if ((gameObject->name == "world") | (gameObject->name == "camera") | (gameObject->name == "sky")) 
            continue;
        
        Engine.DestroyGameObject(gameObject);
        index = 0;
    }
    
}
```
