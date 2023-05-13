# Minimum cmake version
cmake_minimum_required(VERSION 3.8)

# Project configuration
project(GameEngineFramework VERSION 0.1.0 LANGUAGES CXX)

# In order to install libraries into correct locations on all platforms.
include(GNUInstallDirs)

# Set default build type
set(default_build_type "Release")

# CMake modules path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

SET(CMAKE_CXX_FLAGS "-std=c++11")

# Headers files
set (ENGINE_HEADERS
    
    "source/configuration.h"
    "source/main.h"
    "source/plugins.h"
    
    "source/Application/applicationlayer.h"
    "source/Application/winproc.h"
    
    "source/Audio/AudioSystem.h"
    
    "source/Engine/Engine.h"
    "source/Engine/components/component.h"
    "source/Engine/components/gameobject.h"
    
    "source/Input/InputSystem.h"
    "source/Input/keys.h"
    
    "source/Physics/PhysicsSystem.h"
    
    "source/Renderer/enumerators.h"
    "source/Renderer/pipeline.h"
    "source/Renderer/RenderSystem.h"
    "source/Renderer/components/camera.h"
    "source/Renderer/components/entity.h"
    "source/Renderer/components/material.h"
    "source/Renderer/components/mesh.h"
    "source/Renderer/components/scene.h"
    "source/Renderer/components/shader.h"
    "source/Renderer/components/sky.h"
    "source/Renderer/types/bufferlayout.h"
    "source/Renderer/types/color.h"
    "source/Renderer/types/transform.h"
    "source/Renderer/types/viewport.h"
    
    "source/Resources/plugins.h"
    "source/Resources/ResourceManager.h"
    "source/Resources/assets/colliderTag.h"
    "source/Resources/assets/meshTag.h"
    "source/Resources/assets/shaderTag.h"
    "source/Resources/assets/textureTag.h"
    
    "source/Scripting/ScriptSystem.h"
    "source/Scripting/components/script.h"
    
    "source/Vendor/Bly7/OBJ_Loader.h"
    "source/Vendor/my-std-lib/fileloader.h"
    "source/Vendor/my-std-lib/logging.h"
    "source/Vendor/my-std-lib/poolallocator.h"
    "source/Vendor/my-std-lib/random.h"
    "source/Vendor/my-std-lib/strings.h"
    "source/Vendor/my-std-lib/timer.h"
    "source/Vendor/my-std-lib/types.h"
    "source/Vendor/stb/stb_image.h"
    "source/Vendor/stb/stb_perlin.h"
    "source/Vendor/stb/stb_truetype.h"
    
    
)

# Source files
set (ENGINE_SOURCES
    
    "source/application.cpp"
    "source/main.cpp"
    
    "source/Application/ApplicationLayer.cpp"
    "source/Application/winproc.cpp"
    
    "source/Audio/AudioSystem.cpp"
    
    "source/Engine/Engine.cpp"
    "source/Engine/components/component.cpp"
    "source/Engine/components/gameobject.cpp"
    
    "source/Input/InputSystem.cpp"
    
    "source/Physics/PhysicsSystem.cpp"
    
    "source/Renderer/pipeline.cpp"
    "source/Renderer/RenderSystem.cpp"
    "source/Renderer/components/camera.cpp"
    "source/Renderer/components/entity.cpp"
    "source/Renderer/components/material.cpp"
    "source/Renderer/components/mesh.cpp"
    "source/Renderer/components/scene.cpp"
    "source/Renderer/components/shader.cpp"
    "source/Renderer/components/sky.cpp"
    "source/Renderer/types/bufferlayout.cpp"
    "source/Renderer/types/color.cpp"
    "source/Renderer/types/transform.cpp"
    "source/Renderer/types/viewport.cpp"
    
    "source/Resources/ResourceManager.cpp"
    "source/Resources/assets/colliderTag.cpp"
    "source/Resources/assets/meshTag.cpp"
    "source/Resources/assets/shaderTag.cpp"
    "source/Resources/assets/textureTag.cpp"
    
    "source/Scripting/ScriptSystem.cpp"
    "source/Scripting/components/script.cpp"
    
    "source/Vendor/my-std-lib/fileloader.cpp"
    "source/Vendor/my-std-lib/logging.cpp"
    "source/Vendor/my-std-lib/random.cpp"
    "source/Vendor/my-std-lib/strings.cpp"
    "source/Vendor/my-std-lib/timer.cpp"
    "source/Vendor/my-std-lib/types.cpp"
    
)

# Create the library
add_executable(Build ${ENGINE_HEADERS} ${ENGINE_SOURCES})

target_link_libraries(Build glew32s.lib)
target_link_libraries(Build libopengl32.a)
target_link_libraries(Build librp3d.a)




