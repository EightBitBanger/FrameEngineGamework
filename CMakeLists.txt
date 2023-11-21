cmake_minimum_required(VERSION 3.8)

project(framework VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

set(CMAKE_SHARED_LIBRARY_PREFIX "")






set (RUNTIME_HEADERS
    
    "source/configuration.h"
    
    "source/Application/applicationlayer.h"
    "source/Application/winproc.h"
    "source/Application/main.h"
    
    "source/Audio/AudioSystem.h"
    
    "source/AI/ActorSystem.h"
    "source/AI/genetics.h"
    "source/AI/components/actor.h"
    
    "source/Serialization/Serialization.h"
    
    "source/Math/Math.h"
    
    "source/Engine/Engine.h"
    "source/Engine/components/component.h"
    "source/Engine/components/gameobject.h"
    
    "source/Engine/types/bufferlayout.h"
    "source/Engine/types/color.h"
    "source/Engine/types/transform.h"
    "source/Engine/types/viewport.h"
    
    "source/Engine/UI/text.h"
    "source/Engine/UI/sprite.h"
    "source/Engine/UI/canvas.h"
    
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
    
#    "vendor/CodeBaseLibrary/directorysearch.h"
#    "vendor/CodeBaseLibrary/fileloader.h"
#    "vendor/CodeBaseLibrary/logging.h"
#    "vendor/CodeBaseLibrary/poolallocator.h"
#    "vendor/CodeBaseLibrary/random.h"
#    "vendor/CodeBaseLibrary/strings.h"
#    "vendor/CodeBaseLibrary/timer.h"
#    "vendor/CodeBaseLibrary/types.h"
    
#    "vendor/stb/stb_image.h"
#    "vendor/stb/stb_perlin.h"
#    "vendor/stb/stb_truetype.h"
    
)

set (RUNTIME_SOURCES
    
    "source/Application/properties.rc"
#    "source/Application/ApplicationLayer.cpp"
#    "source/Application/winproc.cpp"
    "source/Application/main.cpp"
    
#    "source/application.cpp"
    
#    "source/Audio/AudioSystem.cpp"
    
#    "source/Engine/Engine.cpp"
#    "source/Engine/components/component.cpp"
#    "source/Engine/components/gameobject.cpp"
    
#    "source/Engine/types/bufferlayout.cpp"
#    "source/Engine/types/color.cpp"
#    "source/Engine/types/transform.cpp"
#    "source/Engine/types/viewport.cpp"
    
#    "source/AI/ActorSystem.cpp"
#    "source/AI/genetics.cpp"
#    "source/AI/components/actor.cpp"
    
#    "source/Serialization/Serialization.cpp"
    
#    "source/Math/Math.cpp"
    
#    "source/Input/InputSystem.cpp"
    
#    "source/Physics/PhysicsSystem.cpp"
#    "source/Physics/components/meshcollider.cpp"
    
#    "source/Renderer/RenderSystem.cpp"
#    "source/Renderer/components/camera.cpp"
#    "source/Renderer/components/meshrenderer.cpp"
#    "source/Renderer/components/material.cpp"
#    "source/Renderer/components/mesh.cpp"
#    "source/Renderer/components/scene.cpp"
#    "source/Renderer/components/light.cpp"
#    "source/Renderer/components/shader.cpp"
    
#    "source/Resources/ResourceManager.cpp"
#    "source/Resources/assets/colliderTag.cpp"
#    "source/Resources/assets/meshTag.cpp"
#    "source/Resources/assets/shaderTag.cpp"
#    "source/Resources/assets/textureTag.cpp"
    
#    "source/Scripting/ScriptSystem.cpp"
#    "source/Scripting/components/script.cpp"
    
#    "tests/framework.cpp"
#    "tests/units/testComponents.cpp"
#    "tests/units/testEngineFunctionality.cpp"
#    "tests/units/testGameObjects.cpp"
#    "tests/units/testRenderSystem.cpp"
#    "tests/units/testScriptSystem.cpp"
#    "tests/units/testPhysicsSystem.cpp"
#    "tests/units/testSerializer.cpp"
#    "tests/units/testTransform.cpp"
    
#    "vendor/CodeBaseLibrary/directorysearch.cpp"
#    "vendor/CodeBaseLibrary/fileloader.cpp"
#    "vendor/CodeBaseLibrary/logging.cpp"
#    "vendor/CodeBaseLibrary/random.cpp"
#    "vendor/CodeBaseLibrary/strings.cpp"
#    "vendor/CodeBaseLibrary/timer.cpp"
#    "vendor/CodeBaseLibrary/types.cpp"
    
)







set (GAME_HEADERS
    
    "source/configuration.h"
    
    "source/Application/applicationlayer.h"
    "source/Application/winproc.h"
    "source/Application/main.h"
    
    "source/Audio/AudioSystem.h"
    
    "source/AI/ActorSystem.h"
    "source/AI/genetics.h"
    "source/AI/components/actor.h"
    
    "source/Serialization/Serialization.h"
    
    "source/Math/Math.h"
    
    "source/Engine/Engine.h"
    "source/Engine/components/component.h"
    "source/Engine/components/gameobject.h"
    
    "source/Engine/types/bufferlayout.h"
    "source/Engine/types/color.h"
    "source/Engine/types/transform.h"
    "source/Engine/types/viewport.h"
    
    "source/Engine/UI/text.h"
    "source/Engine/UI/sprite.h"
    "source/Engine/UI/canvas.h"
    
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
    
#    "vendor/CodeBaseLibrary/directorysearch.h"
#    "vendor/CodeBaseLibrary/fileloader.h"
#    "vendor/CodeBaseLibrary/logging.h"
#    "vendor/CodeBaseLibrary/poolallocator.h"
#    "vendor/CodeBaseLibrary/random.h"
#    "vendor/CodeBaseLibrary/strings.h"
#    "vendor/CodeBaseLibrary/timer.h"
#    "vendor/CodeBaseLibrary/types.h"
    
#    "vendor/stb/stb_image.h"
#    "vendor/stb/stb_perlin.h"
#    "vendor/stb/stb_truetype.h"
    
)

set (GAME_SOURCES
    
    "source/application.cpp"
    
)








set (CORE_HEADERS
    
#    "source/configuration.h"
    
    "source/Application/applicationlayer.h"
    "source/Application/winproc.h"
#    "source/Application/main.h"
    
    "source/Audio/AudioSystem.h"
    
    "source/AI/ActorSystem.h"
    "source/AI/genetics.h"
    "source/AI/components/actor.h"
    
    "source/Serialization/Serialization.h"
    
    "source/Math/Math.h"
    
    "source/Engine/Engine.h"
    "source/Engine/components/component.h"
    "source/Engine/components/gameobject.h"
    
    "source/Engine/types/bufferlayout.h"
    "source/Engine/types/color.h"
    "source/Engine/types/transform.h"
    "source/Engine/types/viewport.h"
    
    "source/Engine/UI/text.h"
    "source/Engine/UI/sprite.h"
    "source/Engine/UI/canvas.h"
    
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

set (CORE_SOURCES
    
#    "source/Application/properties.rc"
    "source/Application/ApplicationLayer.cpp"
    "source/Application/winproc.cpp"
#    "source/Application/main.cpp"
    
#    "source/application.cpp"
    
    "source/Audio/AudioSystem.cpp"
    
    "source/Engine/Engine.cpp"
    "source/Engine/components/component.cpp"
    "source/Engine/components/gameobject.cpp"
    
    "source/Engine/types/bufferlayout.cpp"
    "source/Engine/types/color.cpp"
    "source/Engine/types/transform.cpp"
    "source/Engine/types/viewport.cpp"
    
    "source/AI/ActorSystem.cpp"
    "source/AI/genetics.cpp"
    "source/AI/components/actor.cpp"
    
    "source/Serialization/Serialization.cpp"
    
    "source/Math/Math.cpp"
    
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
# Build application executable runtime
#

add_executable(runtime ${RUNTIME_HEADERS} ${RUNTIME_SOURCES})

target_compile_features(runtime PUBLIC cxx_std_11)

set_target_properties(runtime PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(runtime PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

target_include_directories(runtime PUBLIC
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

target_link_libraries(runtime glew32s.lib)
target_link_libraries(runtime opengl32.a)
target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/libcore.dll.a)
target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/libgame.dll.a)




#
# Build application code library
#

add_library(game SHARED ${GAME_HEADERS} ${GAME_SOURCES})

target_compile_features(game PUBLIC cxx_std_11)

set_target_properties(game PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(game PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

target_include_directories(game PUBLIC
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

target_link_libraries(game glew32s.lib)
target_link_libraries(game opengl32.a)
target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/libcore.dll.a)




#
# Build core engine library
#

add_library(core SHARED ${CORE_HEADERS} ${CORE_SOURCES})

target_compile_features(core PUBLIC cxx_std_11)

set_target_properties(core PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

target_include_directories(core PUBLIC
              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
              $<INSTALL_INTERFACE:include>
)

target_link_libraries(core glew32s.lib)
target_link_libraries(core opengl32.a)
target_link_libraries(core ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)



