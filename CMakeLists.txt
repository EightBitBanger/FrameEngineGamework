cmake_minimum_required(VERSION 3.8)

project(framework VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

option(BUILD_APPLICATION_LIBRARY "" ON)
option(BUILD_RUNTIME_EXECUTABLE "" OFF)
option(BUILD_CORE_ENGINE "" OFF)



if(BUILD_RUNTIME_EXECUTABLE)

if(BUILD_CORE_ENGINE OR BUILD_APPLICATION_LIBRARY)
    message(FATAL_ERROR "Multiple project builds are set. Cmake can only build one project at a time.")
endif()

set (RUNTIME_HEADERS
    
    "include/configuration.h"
    
    "include/Application/applicationlayer.h"
    "include/Application/winproc.h"
    "include/Application/main.h"
    
    "include/Audio/AudioSystem.h"
    
    "include/ActorAI/ActorSystem.h"
    "include/ActorAI/genetics.h"
    "include/ActorAI/components/actor.h"
    "include/ActorAI/components/neuron.h"
    
    "include/Serialization/Serialization.h"
    
    "include/Math/Math.h"
    
    "include/Engine/Engine.h"
    "include/Engine/components/component.h"
    "include/Engine/components/gameobject.h"
    
    "include/Engine/types/bufferlayout.h"
    "include/Engine/types/color.h"
    "include/Engine/types/transform.h"
    "include/Engine/types/viewport.h"
    
    "include/Engine/UI/text.h"
    "include/Engine/UI/sprite.h"
    "include/Engine/UI/canvas.h"
    "include/Engine/UI/panel.h"
    "include/Engine/UI/button.h"
    
    "include/Input/InputSystem.h"
    "include/Input/keys.h"
    
    "include/Physics/PhysicsSystem.h"
    "include/Physics/components/meshcollider.h"
    
    "include/Profiler/Profiler.h"
    
    "include/Renderer/enumerators.h"
    "include/Renderer/RenderSystem.h"
    "include/Renderer/components/camera.h"
    "include/Renderer/components/meshrenderer.h"
    "include/Renderer/components/material.h"
    "include/Renderer/components/mesh.h"
    "include/Renderer/components/scene.h"
    "include/Renderer/components/light.h"
    "include/Renderer/components/shader.h"
    
    "include/Resources/ResourceManager.h"
    "include/Resources/assets/colliderTag.h"
    "include/Resources/assets/meshTag.h"
    "include/Resources/assets/shaderTag.h"
    "include/Resources/assets/textureTag.h"
    
    "include/Scripting/ScriptSystem.h"
    "include/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
)

set (RUNTIME_SOURCES
    
    "src/Application/properties.rc"
    "src/Application/main.cpp"
    
)


add_executable(runtime ${RUNTIME_HEADERS} ${RUNTIME_SOURCES})

target_compile_features(runtime PUBLIC cxx_std_11)

set_target_properties(runtime PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(runtime PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
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

endif()








#
# Build application code library
#

if(BUILD_APPLICATION_LIBRARY)

if(BUILD_CORE_ENGINE OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

set (GAME_HEADERS
    
    "include/configuration.h"
    
    "include/Application/applicationlayer.h"
    "include/Application/winproc.h"
    "include/Application/main.h"
    
    "include/Audio/AudioSystem.h"
    
    "include/ActorAI/ActorSystem.h"
    "include/ActorAI/genetics.h"
    "include/ActorAI/components/actor.h"
    "include/ActorAI/components/neuron.h"
    
    "include/Serialization/Serialization.h"
    
    "include/Math/Math.h"
    
    "include/Engine/Engine.h"
    "include/Engine/components/component.h"
    "include/Engine/components/gameobject.h"
    
    "include/Engine/types/bufferlayout.h"
    "include/Engine/types/color.h"
    "include/Engine/types/transform.h"
    "include/Engine/types/viewport.h"
    
    "include/Engine/UI/text.h"
    "include/Engine/UI/sprite.h"
    "include/Engine/UI/canvas.h"
    "include/Engine/UI/panel.h"
    "include/Engine/UI/button.h"
    
    "include/Input/InputSystem.h"
    "include/Input/keys.h"
    
    "include/Physics/PhysicsSystem.h"
    "include/Physics/components/meshcollider.h"
    
    "include/Profiler/Profiler.h"
    
    "include/Renderer/enumerators.h"
    "include/Renderer/RenderSystem.h"
    "include/Renderer/components/camera.h"
    "include/Renderer/components/meshrenderer.h"
    "include/Renderer/components/material.h"
    "include/Renderer/components/mesh.h"
    "include/Renderer/components/scene.h"
    "include/Renderer/components/light.h"
    "include/Renderer/components/shader.h"
    
    "include/Resources/ResourceManager.h"
    "include/Resources/assets/colliderTag.h"
    "include/Resources/assets/meshTag.h"
    "include/Resources/assets/shaderTag.h"
    "include/Resources/assets/textureTag.h"
    
    "include/Scripting/ScriptSystem.h"
    "include/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
)

set (GAME_SOURCES
    
    "src/application.cpp"
    
)


add_library(game SHARED ${GAME_HEADERS} ${GAME_SOURCES})

target_compile_features(game PUBLIC cxx_std_11)

set_target_properties(game PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(game PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
	ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/vendor"
)

target_include_directories(game PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(game glew32s.lib)
target_link_libraries(game opengl32.a)
target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/libcore.dll.a)

endif()








#
# Build core engine library
#

if(BUILD_CORE_ENGINE)

if(BUILD_APPLICATION_LIBRARY OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

set (CORE_HEADERS
    
    "include/Application/applicationlayer.h"
    "include/Application/winproc.h"
    
    "include/Audio/AudioSystem.h"
    
    "include/ActorAI/ActorSystem.h"
    "include/ActorAI/genetics.h"
    "include/ActorAI/components/actor.h"
    "include/ActorAI/components/neuron.h"
    
    "include/Serialization/Serialization.h"
    
    "include/Math/Math.h"
    
    "include/Engine/Engine.h"
    "include/Engine/components/component.h"
    "include/Engine/components/gameobject.h"
    
    "include/Engine/types/bufferlayout.h"
    "include/Engine/types/color.h"
    "include/Engine/types/transform.h"
    "include/Engine/types/viewport.h"
    
    "include/Engine/UI/text.h"
    "include/Engine/UI/sprite.h"
    "include/Engine/UI/canvas.h"
    "include/Engine/UI/panel.h"
    "include/Engine/UI/button.h"
    
    "include/Input/InputSystem.h"
    "include/Input/keys.h"
    
    "include/Physics/PhysicsSystem.h"
    "include/Physics/components/meshcollider.h"
    
    "include/Profiler/Profiler.h"
    
    "include/Renderer/enumerators.h"
    "include/Renderer/RenderSystem.h"
    "include/Renderer/components/camera.h"
    "include/Renderer/components/meshrenderer.h"
    "include/Renderer/components/material.h"
    "include/Renderer/components/mesh.h"
    "include/Renderer/components/scene.h"
    "include/Renderer/components/light.h"
    "include/Renderer/components/shader.h"
    
    "include/Resources/ResourceManager.h"
    "include/Resources/assets/colliderTag.h"
    "include/Resources/assets/meshTag.h"
    "include/Resources/assets/shaderTag.h"
    "include/Resources/assets/textureTag.h"
    
    "include/Scripting/ScriptSystem.h"
    "include/Scripting/components/script.h"
    
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
    "src/Engine/EngineUpdate.cpp"
    "src/Engine/EngineUpdateUI.cpp"
    
    "src/Engine/components/component.cpp"
    "src/Engine/components/gameobject.cpp"
    
    "src/Engine/types/bufferlayout.cpp"
    "src/Engine/types/color.cpp"
    "src/Engine/types/transform.cpp"
    "src/Engine/types/viewport.cpp"
    
    "src/ActorAI/ActorSystem.cpp"
    "src/ActorAI/genetics.cpp"
    "src/ActorAI/components/actor.cpp"
    "src/ActorAI/components/neuron.cpp"
    
    "src/Serialization/Serialization.cpp"
    
    "src/Math/Math.cpp"
    
    "src/Input/InputSystem.cpp"
    
    "src/Physics/PhysicsSystem.cpp"
    "src/Physics/components/meshcollider.cpp"
    
    "src/Profiler/Profiler.cpp"
    
    "src/Renderer/RenderSystem.cpp"
    "src/Renderer/Pipeline.cpp"
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


add_library(core SHARED ${CORE_HEADERS} ${CORE_SOURCES})

target_compile_features(core PUBLIC cxx_std_11)

set_target_properties(core PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(core PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
	ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/vendor"
)

target_include_directories(core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(core glew32s.lib)
target_link_libraries(core opengl32.a)
target_link_libraries(core ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)

endif()


