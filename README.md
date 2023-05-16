## Introduction

This game engine framework aims to provide simpler interfaces with more complex systems. The application layer contains an entry point function and numerious APIs for rendering, physics, resource management, scene management, input, timing and random number generation.


<br><br/>
### Core Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Window input handling


<br><br/>
##  Building the project
First, download the repository with git. You can then use cmake to generate the project files of your choice.

```
git pull https://github.com/RetroBytes32/GameEngineFramework
```


## Dependencies
Building this repository will require the following libraries to be installed.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/Groovounet/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭

### Required exteral linkage.
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


> The source file "Application.cpp" is the application layer entry point and the starting point for your application code.

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
> The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
> The `Start()` function will be called once during application initiation.

```c++
// This example will load a model and a texture and render it with a camera controller.

GameObject* cameraController;

void Framework::Start() {
    
    // Load some external files
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Set the gravity for the simulation
    Physics.SetWorldGravity(0, -90, 0);
    
    // Create objects from resource tags
    Material* groundMaterial = Resources.CreateMaterialFromTag("mat_grassy");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.white);
    
    
    // Create a ground plain
    Mesh* groundMesh = Renderer.CreateMesh();
    groundMesh->AddPlainSubDivided(-50, 0, -50, 10, 10, Colors.white, 10, 10);
    
    GameObject* ground = Engine.CreateGameObject();
    ground->name = "world";
    
    // Add a render component
    Component* groundRenderer = Engine.CreateComponentEntityRenderer(groundMesh, groundMaterial);
    ground->AddComponent(groundRenderer);
    
    // Add a physics component
    Component* groundRigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
    ground->AddComponent(groundRigidBodyComponent);
    ground->SetRigidBodyStatic();
    
    // Lock the ground plain in place
    ground->SetLinearAxisLockFactor(0, 0, 0);
    ground->SetAngularAxisLockFactor(0, 0, 0);
    
    // Ground collider
    rp3d::BoxShape* groundCollider = Physics.CreateColliderBox(100, 100, 100);
    ground->AddColliderBox(groundCollider, 0, -100, 0);
    
    
    // Create a game object
    GameObject* barrel = Engine.CreateGameObject();
    
    // Add a render component
    Component* entityRenderer = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    barrel->AddComponent(entityRenderer);
    
    // Add a physics component
    Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
    barrel->AddComponent(rigidBodyComponent);
    
    
    // Create a camera controller
    cameraController = Engine.CreateCameraController(0, 30, 0);
    Component* componentPtr = cameraController->FindComponent(ComponentType::Script);
}
```

<br><br/>
### Main loop
> The `Run()` function will be called once per frame.

```c++
// This example will apply force to the camera`s rigid body on key presses.

void Framework::Run() {
    
    glm::vec3 force(0);
    
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed
    force *= 100;
    
    cameraController->AddForce(force.x, force.y, force.z);
}
```
