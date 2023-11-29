## Features
- Render system
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Not yet added in full)
- Resource management
- Input handling
- Serialization
- Multithreading


<br><br/>


## Overview
This engine is split up into three parts. The engine core and the game code are located in separate dynamic libraries with a runtime executable to bring it together.


<br><br/>


## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows


<br><br/>


##  Building the project
First, clone the repository with git. You can then use cmake to generate the project files of your choice. You have three build options in cmake. Each option will generate a project file which will compile the selected part of the framework. To use the framework you would want to select BUILD_APPLICATION_LIBRARY, however, modifying the core requires you to select BUILD_CORE_ENGINE.

```
git clone https://github.com/RetroBytes32/FrameEngineGamework
```


### Dependencies
Building this repository will require the following libraries to be installed.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/icaven/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭



## Screenshots
You can find screenshots of development progress [here](https://github.com/RetroBytes32/GameEngineFramework/wiki/Screenshots).


<br><br/>


<br><br/>
## Access pointers
The framework contains many sub systems which can be accessed though the following "singletons".

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
The `Start()` function will be called once during application initiation while `Run()` will be called once per frame.

