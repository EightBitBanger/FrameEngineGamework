<p align="center">
  <img src="https://github.com/RetroBytes32/FrameEngineGamework/blob/main/demo/bio.png"  width="100" height="100">
</p>


<br>

## Features

- Rendering
  - Simple textured mesh renderer
  - Point and directional lights
  - Text rendering via sprite sheet

- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
  - Rigid body simulation
  - Terrain height field mapping
  - <a href="https://github.com/DanielChappuis/reactphysics3d/blob/master/documentation/UserDocumentation.md">Much more...</a>⁭

- ActorAI
  - Psudo-genetics based actor generation
  - Neural network / state machine hybrid for actor personality
  - Updated on a separate thread

- Resource management
  - Resource files are loaded automatically at startup
  - Image: PNG BMP
  - Models: OBJ


<br><br/>


## Overview
 This framework is designed to provide the functionality of a game engine without a full editor. The primary use case would be for the creation of applications that require user interactivity and physical simulation. The core fuctionality of the framework covers rendering, physics, AI and resource management.


<br><br/>


## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows (x64)


<br><br/>


##  Building the repository
First, clone the repository with git. You can then use cmake to generate the project files of your choice. You have three primary build options in cmake. Each option will generate a project file which will compile the selected part of the framework. To use the framework you would want to select `BUILD_APPLICATION_LIBRARY`. However, modifying the core requires you to select `BUILD_CORE_ENGINE`. To build the runtime executable select `BUILD_RUNTIME_EXECUTABLE`.

```
git clone https://github.com/RetroBytes32/FrameEngineGamework
```


<br><br/>


## Dependencies
Building this repository will require the following libraries. These libraries and their associated licenses will be included in the "vendor" directory in this repository.
Links to the latest version of these libraries could be found below. Note: The latest version of these libraries may be incompatible with the current version of this framework.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/nothings/stb">STB-Perlin</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/icaven/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭



<br><br/>


## Architecture
 This engine has many sub systems which are split up into several dynamic libraries. The engine core and the user application code are located in separate dynamic libraries with a runtime executable to launch the application. The physics engine, ReactPhysics3D, is contained in it's own dynamic library. External resources are located in a directory named 'core' in the working directory of the runtime executable. These resources include models, textures and shaders that are loaded automatically by the engine`s resource manager.


<br><br/>


## Application entry point
The source file "Application.cpp" is the application layer entry point and the starting point for your application code.
The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
The `Start()` function will be called once during application initiation while `Run()` will be called once per frame.


<br><br/>


## Examples
Below are a few examples covering the use of objects and components in the framework

[Game objects](https://github.com/RetroBytes32/FrameEngineGamework/wiki/Game-objects)
<br>
[Working with components](https://github.com/RetroBytes32/FrameEngineGamework/wiki/Working-with-components)


<br><br/>


## Screenshots
You can find screenshots of development progress [here](https://github.com/RetroBytes32/GameEngineFramework/wiki/Screenshots).


<br><br/>

