cmake_minimum_required(VERSION 3.8)

project(framework VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

set(CMAKE_SHARED_LIBRARY_PREFIX "")





set (RUNTIME_HEADERS
    
    "src/configuration.h"
    
    "src/Application/applicationlayer.h"
    "src/Application/winproc.h"
    "src/Application/main.h"
    
    "src/Audio/AudioSystem.h"
    
    "src/ActorAI/ActorSystem.h"
    "src/ActorAI/genetics.h"
    "src/ActorAI/components/actor.h"
    
    "src/Serialization/Serialization.h"
    
    "src/Math/Math.h"
    
    "src/Engine/Engine.h"
    "src/Engine/components/component.h"
    "src/Engine/components/gameobject.h"
    
    "src/Engine/types/bufferlayout.h"
    "src/Engine/types/color.h"
    "src/Engine/types/transform.h"
    "src/Engine/types/viewport.h"
    
    "src/Engine/UI/text.h"
    "src/Engine/UI/sprite.h"
    "src/Engine/UI/canvas.h"
    "src/Engine/UI/panel.h"
    
    "src/Input/InputSystem.h"
    "src/Input/keys.h"
    
    "src/Physics/PhysicsSystem.h"
    "src/Physics/components/meshcollider.h"
    
    "src/Renderer/enumerators.h"
    "src/Renderer/RenderSystem.h"
    "src/Renderer/components/camera.h"
    "src/Renderer/components/meshrenderer.h"
    "src/Renderer/components/material.h"
    "src/Renderer/components/mesh.h"
    "src/Renderer/components/scene.h"
    "src/Renderer/components/light.h"
    "src/Renderer/components/shader.h"
    
    "src/Resources/ResourceManager.h"
    "src/Resources/assets/colliderTag.h"
    "src/Resources/assets/meshTag.h"
    "src/Resources/assets/shaderTag.h"
    "src/Resources/assets/textureTag.h"
    
    "src/Scripting/ScriptSystem.h"
    "src/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
)

set (RUNTIME_SOURCES
    
    "src/Application/properties.rc"
    "src/Application/main.cpp"
    
)







set (GAME_HEADERS
    
    "src/configuration.h"
    
    "src/Application/applicationlayer.h"
    "src/Application/winproc.h"
    "src/Application/main.h"
    
    "src/Audio/AudioSystem.h"
    
    "src/ActorAI/ActorSystem.h"
    "src/ActorAI/genetics.h"
    "src/ActorAI/components/actor.h"
    
    "src/Serialization/Serialization.h"
    
    "src/Math/Math.h"
    
    "src/Engine/Engine.h"
    "src/Engine/components/component.h"
    "src/Engine/components/gameobject.h"
    
    "src/Engine/types/bufferlayout.h"
    "src/Engine/types/color.h"
    "src/Engine/types/transform.h"
    "src/Engine/types/viewport.h"
    
    "src/Engine/UI/text.h"
    "src/Engine/UI/sprite.h"
    "src/Engine/UI/canvas.h"
    "src/Engine/UI/panel.h"
    
    "src/Input/InputSystem.h"
    "src/Input/keys.h"
    
    "src/Physics/PhysicsSystem.h"
    "src/Physics/components/meshcollider.h"
    
    "src/Renderer/enumerators.h"
    "src/Renderer/RenderSystem.h"
    "src/Renderer/components/camera.h"
    "src/Renderer/components/meshrenderer.h"
    "src/Renderer/components/material.h"
    "src/Renderer/components/mesh.h"
    "src/Renderer/components/scene.h"
    "src/Renderer/components/light.h"
    "src/Renderer/components/shader.h"
    
    "src/Resources/ResourceManager.h"
    "src/Resources/assets/colliderTag.h"
    "src/Resources/assets/meshTag.h"
    "src/Resources/assets/shaderTag.h"
    "src/Resources/assets/textureTag.h"
    
    "src/Scripting/ScriptSystem.h"
    "src/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
)

set (GAME_SOURCES
    
    "src/application.cpp"
    
)








set (CORE_HEADERS
    
    "src/Application/applicationlayer.h"
    "src/Application/winproc.h"
    
    "src/Audio/AudioSystem.h"
    
    "src/ActorAI/ActorSystem.h"
    "src/ActorAI/genetics.h"
    "src/ActorAI/components/actor.h"
    
    "src/Serialization/Serialization.h"
    
    "src/Math/Math.h"
    
    "src/Engine/Engine.h"
    "src/Engine/components/component.h"
    "src/Engine/components/gameobject.h"
    
    "src/Engine/types/bufferlayout.h"
    "src/Engine/types/color.h"
    "src/Engine/types/transform.h"
    "src/Engine/types/viewport.h"
    
    "src/Engine/UI/text.h"
    "src/Engine/UI/sprite.h"
    "src/Engine/UI/canvas.h"
    "src/Engine/UI/panel.h"
    
    "src/Input/InputSystem.h"
    "src/Input/keys.h"
    
    "src/Physics/PhysicsSystem.h"
    "src/Physics/components/meshcollider.h"
    
    "src/Renderer/enumerators.h"
    "src/Renderer/RenderSystem.h"
    "src/Renderer/components/camera.h"
    "src/Renderer/components/meshrenderer.h"
    "src/Renderer/components/material.h"
    "src/Renderer/components/mesh.h"
    "src/Renderer/components/scene.h"
    "src/Renderer/components/light.h"
    "src/Renderer/components/shader.h"
    
    "src/Resources/ResourceManager.h"
    "src/Resources/assets/colliderTag.h"
    "src/Resources/assets/meshTag.h"
    "src/Resources/assets/shaderTag.h"
    "src/Resources/assets/textureTag.h"
    
    "src/Scripting/ScriptSystem.h"
    "src/Scripting/components/script.h"
    
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
    
    "src/Application/ApplicationLayer.cpp"
    "src/Application/winproc.cpp"
    
    "src/Audio/AudioSystem.cpp"
    
    "src/Engine/Engine.cpp"
    "src/Engine/EngineGameObjects.cpp"
    "src/Engine/EngineComponents.cpp"
    "src/Engine/components/component.cpp"
    "src/Engine/components/gameobject.cpp"
    
    "src/Engine/types/bufferlayout.cpp"
    "src/Engine/types/color.cpp"
    "src/Engine/types/transform.cpp"
    "src/Engine/types/viewport.cpp"
    
    "src/ActorAI/ActorSystem.cpp"
    "src/ActorAI/genetics.cpp"
    "src/ActorAI/components/actor.cpp"
    
    "src/Serialization/Serialization.cpp"
    
    "src/Math/Math.cpp"
    
    "src/Input/InputSystem.cpp"
    
    "src/Physics/PhysicsSystem.cpp"
    "src/Physics/components/meshcollider.cpp"
    
    "src/Renderer/RenderSystem.cpp"
    "src/Renderer/components/camera.cpp"
    "src/Renderer/components/meshrenderer.cpp"
    "src/Renderer/components/material.cpp"
    "src/Renderer/components/mesh.cpp"
    "src/Renderer/components/scene.cpp"
    "src/Renderer/components/light.cpp"
    "src/Renderer/components/shader.cpp"
    
    "src/Resources/ResourceManager.cpp"
    "src/Resources/assets/colliderTag.cpp"
    "src/Resources/assets/meshTag.cpp"
    "src/Resources/assets/shaderTag.cpp"
    "src/Resources/assets/textureTag.cpp"
    
    "src/Scripting/ScriptSystem.cpp"
    "src/Scripting/components/script.cpp"
    
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



