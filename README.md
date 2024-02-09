## Features
- Render system
  - Simple textured mesh renderer.
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ActorAI
  - Genetics based physical appearance system
  - Neural network / state machine hybrid
- Resource management
  - Image: PNG BMP
  - Models: OBJ
- Multithreading


<br><br/>


## Overview
 


## Architecture
 This engine has many sub systems which are split up into several dynamic libraries. The engine core and the game code are located in separate dynamic libraries with a runtime executable to launch the application. The physics engine, reactphysics3D, is contained in it's own dynamic library. External resources are located in a directory named 'data' in the working directory of the runtime executable. These resources include models, textures, shaders and are loaded automatically by the engine`s resource manager.


<br><br/>


## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows (x64)


<br><br/>


##  Building the repository
First, clone the repository with git. You can then use cmake to generate the project files of your choice. You have three build options in cmake. Each option will generate a project file which will compile the selected part of the framework. To use the framework you would want to select BUILD_APPLICATION_LIBRARY, however, modifying the core requires you to select BUILD_CORE_ENGINE. To build the runtime executable select BUILD_RUNTIME_EXECUTABLE.

```
git clone https://github.com/RetroBytes32/FrameEngineGamework
```


### Dependencies
Building this repository will require the following libraries.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/icaven/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭



## Screenshots
You can find screenshots of development progress [here](https://github.com/RetroBytes32/GameEngineFramework/wiki/Screenshots).


<br><br/>


## Application entry point
The source file "Application.cpp" is the application layer entry point and the starting point for your application code.
The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
The `Start()` function will be called once during application initiation while `Run()` will be called once per frame.

