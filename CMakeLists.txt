# Minimum cmake version
cmake_minimum_required(VERSION 3.8)

# Project configuration
project(GameEngineFramework VERSION 0.1.0 LANGUAGES CXX)

# In order to install libraries into correct locations on all platforms.
include(GNUInstallDirs)

# CMake modules path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)




# Headers files
set (ENGINE_HEADERS
    
    "source/configuration.h"
    "source/plugins.h"
    
    "source/Application/applicationlayer.h"
    "source/Application/winproc.h"
    "source/Application/main.h"
    
    "source/Audio/AudioSystem.h"
    
    "source/AI/ActorSystem.h"
    "source/AI/genetics.h"
    "source/AI/components/actor.h"
    
    "source/Serialization/Serialization.h"
    
    "source/Math/Math.cpp"
    
    "source/Engine/Engine.h"
    "source/Engine/components/component.h"
    "source/Engine/components/gameobject.h"
    
    "source/Engine/types/bufferlayout.h"
    "source/Engine/types/color.h"
    "source/Engine/types/transform.h"
    "source/Engine/types/viewport.h"
    
    "source/Engine/UI/text.h"
    
    "source/Input/InputSystem.h"
    "source/Input/keys.h"
    
    "source/Physics/PhysicsSystem.h"
    "source/Physics/components/meshcollider.h"
    
    "source/Renderer/enumerators.h"
    "source/Renderer/RenderSystem.h"
    "source/Renderer/components/camera.h"
    "source/Renderer/components/meshrenderer.h"
    "source/Renderer/components/material.h"
    "source/Renderer/components/mesh.h"
    "source/Renderer/components/scene.h"
    "source/Renderer/components/light.h"
    "source/Renderer/components/shader.h"
    
    "source/Resources/plugins.h"
    "source/Resources/ResourceManager.h"
    "source/Resources/assets/colliderTag.h"
    "source/Resources/assets/meshTag.h"
    "source/Resources/assets/shaderTag.h"
    "source/Resources/assets/textureTag.h"
    
    "source/Scripting/ScriptSystem.h"
    "source/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
    "vendor/CodeBaseLibrary/directorysearch.h"
    "vendor/CodeBaseLibrary/fileloader.h"
    "vendor/CodeBaseLibrary/logging.h"
    "vendor/CodeBaseLibrary/poolallocator.h"
    "vendor/CodeBaseLibrary/random.h"
    "vendor/CodeBaseLibrary/strings.h"
    "vendor/CodeBaseLibrary/timer.h"
    "vendor/CodeBaseLibrary/types.h"
    
    "vendor/stb/stb_image.h"
    "vendor/stb/stb_perlin.h"
    "vendor/stb/stb_truetype.h"
    
)

# Source files
set (ENGINE_SOURCES
    
    "source/Application/properties.rc"
    "source/Application/ApplicationLayer.cpp"
    "source/Application/winproc.cpp"
    "source/Application/main.cpp"
    
    "source/application.cpp"
    
    "source/Audio/AudioSystem.cpp"
    
    "source/Engine/Engine.cpp"
    "source/Engine/components/component.cpp"
    "source/Engine/components/gameobject.cpp"
    
    "source/Engine/UI/text.cpp"
    
    "source/AI/ActorSystem.cpp"
    "source/AI/genetics.cpp"
    "source/AI/components/actor.cpp"
    
    "source/Serialization/Serialization.cpp"
    
    "source/Math/Math.cpp"
    
    "source/Engine/types/bufferlayout.cpp"
    "source/Engine/types/color.cpp"
    "source/Engine/types/transform.cpp"
    "source/Engine/types/viewport.cpp"
    
    "source/Input/InputSystem.cpp"
    
    "source/Physics/PhysicsSystem.cpp"
    "source/Physics/components/meshcollider.cpp"
    
    "source/Renderer/RenderSystem.cpp"
    "source/Renderer/components/camera.cpp"
    "source/Renderer/components/meshrenderer.cpp"
    "source/Renderer/components/material.cpp"
    "source/Renderer/components/mesh.cpp"
    "source/Renderer/components/scene.cpp"
    "source/Renderer/components/light.cpp"
    "source/Renderer/components/shader.cpp"
    
    "source/Resources/ResourceManager.cpp"
    "source/Resources/assets/colliderTag.cpp"
    "source/Resources/assets/meshTag.cpp"
    "source/Resources/assets/shaderTag.cpp"
    "source/Resources/assets/textureTag.cpp"
    
    "source/Scripting/ScriptSystem.cpp"
    "source/Scripting/components/script.cpp"
    
    "tests/framework.cpp"
    "tests/units/testComponents.cpp"
    "tests/units/testEngineFunctionality.cpp"
    "tests/units/testGameObjects.cpp"
    "tests/units/testRenderSystem.cpp"
    "tests/units/testScriptSystem.cpp"
    "tests/units/testPhysicsSystem.cpp"
    "tests/units/testSerializer.cpp"
    "tests/units/testTransform.cpp"
    
    "vendor/CodeBaseLibrary/directorysearch.cpp"
    "vendor/CodeBaseLibrary/fileloader.cpp"
    "vendor/CodeBaseLibrary/logging.cpp"
    "vendor/CodeBaseLibrary/random.cpp"
    "vendor/CodeBaseLibrary/strings.cpp"
    "vendor/CodeBaseLibrary/timer.cpp"
    "vendor/CodeBaseLibrary/types.cpp"
    
)


#
# Compile as an executable
#

add_executable(release ${ENGINE_HEADERS} ${ENGINE_SOURCES})

set(default_build_type "release")

target_compile_features(release PUBLIC cxx_std_11)

set_target_properties(release PROPERTIES CXX_EXTENSIONS OFF)


target_include_directories(release PUBLIC
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

target_link_libraries(release glew32s.lib)
target_link_libraries(release libopengl32.a)
target_link_libraries(release ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)

