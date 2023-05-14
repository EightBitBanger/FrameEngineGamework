## Introduction

This game engine framework aims to provides a simple interface with more complex systems. The application layer provides APIs for rendering, physics, resource management, scene management, input, timing and random number generation. 


<br><br/>
### Core Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Window input handling


<br><br/>
##  Building the project

> Downloading the repository
```
git pull https://github.com/RetroBytes32/GameEngineFramework
mkdir build
cd build
cmake ../
```

### Dependencies
> Building this repository will require the following libraries to be installed.
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
> The start function will be called once during application initiation.

```c++
// This example will load a model and a texture and display it centered on screen.

GameObject* cameraController;

void Framework::Start() {
    
    // Load some external files
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -0, 0);
    
    // Create objects from resource tags
    Material* barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    Mesh*     barrelMesh     = Resources.CreateMeshFromTag("barrel");
    
    
    // Create a game object
    GameObject* barrel = Engine.CreateGameObject();
    
    // Add a render component
    Component* entityRenderer = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    barrel->AddComponent(entityRenderer);
    
    // Add a physics component
    Component* rigidBodyComponent = Engine.CreateComponent(COMPONENT_TYPE_RIGIDBODY);
    barrel->AddComponent(rigidBodyComponent);
    
    
    // Create a camera controller
    cameraController = Engine.CreateCameraController(-50, 0, 0);
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
