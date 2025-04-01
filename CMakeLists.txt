cmake_minimum_required(VERSION 3.8)

project(framework VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

option(BUILD_APPLICATION_LIBRARY "Project will build the user application library." ON)
option(BUILD_RUNTIME_EXECUTABLE "Project will build the runtime executable." OFF)
option(BUILD_CORE_ENGINE "Project will build the core engine library" OFF)

option(EVENT_LOG_DETAILED   "Log events out to the event log file." OFF)
option(RUN_UNIT_TESTS       "Run unit tests at application start." OFF)


# ==========================================================
# Build configuration
# 

if(EVENT_LOG_DETAILED)
    add_compile_definitions(EVENT_LOG_DETAILED)
endif()

if(RUN_UNIT_TESTS)
    add_compile_definitions(RUN_UNIT_TESTS)
endif()


function(auto_sources var)
    foreach(pattern IN LISTS ARGN)
        file(GLOB_RECURSE matched_files ${pattern})
        list(APPEND ${var} ${matched_files})
    endforeach()
    set(${var} ${${var}} PARENT_SCOPE)
endfunction()


# ==========================================================
# ==========================================================
# ==========================================================
# 
# Build executable
# 
# ==========================================================
# ==========================================================

if(BUILD_RUNTIME_EXECUTABLE)

if(BUILD_CORE_ENGINE OR BUILD_APPLICATION_LIBRARY)
    message(FATAL_ERROR "Multiple project builds are set. Cmake can only build one project at a time.")
endif()

auto_sources(RUNTIME_HEADERS
    "include/**/*.h"
    "tests/framework.h"
    "vendor/**/*.h"
)

set (RUNTIME_SOURCES
    
    "tests/framework.cpp"
    "tests/units/testComponents.cpp"
    "tests/units/testEngineFunctionality.cpp"
    "tests/units/testGameObjects.cpp"
    "tests/units/testRenderSystem.cpp"
    "tests/units/testScriptSystem.cpp"
    "tests/units/testPhysicsSystem.cpp"
    "tests/units/testSerializer.cpp"
    "tests/units/testTransform.cpp"
    
    "src/Application/properties.rc"
    "src/Application/main.cpp"
    
)


add_executable(runtime ${RUNTIME_HEADERS} ${RUNTIME_SOURCES})

add_compile_definitions(BUILD_RUNTIME)

target_compile_features(runtime PUBLIC cxx_std_11)

set_target_properties(runtime PROPERTIES CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O2")

set_target_properties(runtime PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_include_directories(runtime PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(runtime glew32s.lib)
target_link_libraries(runtime Ws2_32.lib)
target_link_libraries(runtime opengl32.a)

target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/libcore.dll.a)
target_link_libraries(runtime ${PROJECT_SOURCE_DIR}/vendor/libgame.dll.a)

endif()



# ==========================================================
# ==========================================================
# ==========================================================
# 
# Build application dll
# 
# ==========================================================
# ==========================================================


if(BUILD_APPLICATION_LIBRARY)

if(BUILD_CORE_ENGINE OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

auto_sources(GAME_SOURCES
    "include/**/*.h"
    "vendor/**/*.h"
)

set (GAME_SOURCES
    
    "src/plugins/ChunkSpawner/WorldLoad.cpp"
    "src/plugins/ChunkSpawner/WorldSave.cpp"
    "src/plugins/ChunkSpawner/ChunkLoad.cpp"
    "src/plugins/ChunkSpawner/ChunkSave.cpp"
    "src/plugins/ChunkSpawner/ChunkCreate.cpp"
    "src/plugins/ChunkSpawner/ChunkDestroy.cpp"
    "src/plugins/ChunkSpawner/ChunkManager.cpp"
    "src/plugins/ChunkSpawner/ChunkManagerUpdate.cpp"
    "src/plugins/ChunkSpawner/ChunkManagerDecorate.cpp"
    "src/plugins/ChunkSpawner/Chunk.cpp"
    
    "src/plugins/WeatherSystem/WeatherSystem.cpp"
    
    "src/plugins/ParticleSystem/Emitter.cpp"
    "src/plugins/ParticleSystem/ParticleSystem.cpp"
    
    "src/start.cpp"
    "src/run.cpp"
    "src/shutdown.cpp"
    "src/functions.cpp"
    "src/plugins.cpp"
    
)


add_library(game SHARED ${GAME_HEADERS} ${GAME_SOURCES})

add_compile_definitions(BUILD_APPLICATION)

target_compile_features(game PUBLIC cxx_std_11)

set_target_properties(game PROPERTIES CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O2")

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
target_link_libraries(game Ws2_32.lib)

target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(game ${PROJECT_SOURCE_DIR}/vendor/libcore.dll.a)

endif()



# ==========================================================
# ==========================================================
# ==========================================================
# 
# Build engine core dll
# 
# ==========================================================
# ==========================================================

if(BUILD_CORE_ENGINE)

if(BUILD_APPLICATION_LIBRARY OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

auto_sources(CORE_SOURCES
    "include/**/*.h"
    "vendor/**/*.h"
)

set (CORE_SOURCES
    
    "src/Application/Platform.cpp"
    "src/Application/winproc.cpp"
    
    "src/Audio/AudioSystem.cpp"
    "src/Audio/components/sound.cpp"
    "src/Audio/components/samplebuffer.cpp"
    
    "src/Networking/NetworkSystem.cpp"
    
    "src/Engine/Engine.cpp"
    "src/Engine/EngineSystems.cpp"
    "src/Engine/EngineUpdate.cpp"
    "src/Engine/EngineConsole.cpp"
    "src/Engine/EngineGameObjects.cpp"
    "src/Engine/EngineComponents.cpp"
    "src/Engine/EngineUpdateStream.cpp"
    
    "src/Engine/componentUpdate/EngineUpdateUI.cpp"
    "src/Engine/componentUpdate/EngineUpdateAI.cpp"
    "src/Engine/componentUpdate/EngineUpdateAIGenetics.cpp"
    "src/Engine/componentUpdate/EngineUpdateAITargeting.cpp"
    "src/Engine/componentUpdate/EngineUpdateAIAnimations.cpp"
    "src/Engine/componentUpdate/EngineUpdateAIPhysics.cpp"
    "src/Engine/componentUpdate/EngineUpdateCamera.cpp"
    "src/Engine/componentUpdate/EngineUpdatePanel.cpp"
    "src/Engine/componentUpdate/EngineUpdateText.cpp"
    "src/Engine/componentUpdate/EngineUpdateRigidBody.cpp"
    "src/Engine/componentUpdate/EngineUpdateTransforms.cpp"
    "src/Engine/componentUpdate/EngineUpdateLight.cpp"
    "src/Engine/componentUpdate/EngineUpdateMeshRenderer.cpp"
    
    "src/Engine/components/component.cpp"
    "src/Engine/components/gameobject.cpp"
    
    "src/Engine/types/bufferlayout.cpp"
    "src/Engine/types/color.cpp"
    "src/Engine/types/nulltype.cpp"
    "src/Engine/types/viewport.cpp"
    
    "src/ActorAI/ActorSystem.cpp"
    "src/ActorAI/ActorUpdate.cpp"
    "src/ActorAI/NeuralNetwork.cpp"
    "src/ActorAI/GeneticPresets.cpp"
    "src/ActorAI/genetics/gene.cpp"
    "src/ActorAI/genetics/codon.cpp"
    "src/ActorAI/genetics/phen.cpp"
    "src/ActorAI/genetics/bio.cpp"
    "src/ActorAI/components/actor.cpp"
    
    "src/Serialization/Serialization.cpp"
    
    "src/Transform/Transform.cpp"
    
    "src/Math/Math.cpp"
    "src/Math/Random.cpp"
    
    "src/Input/InputSystem.cpp"
    
    "src/Physics/PhysicsSystem.cpp"
    "src/Physics/components/meshcollider.cpp"
    
    "src/Profiler/Profiler.cpp"
    "src/Types/Types.cpp"
    "src/Logging/Logging.cpp"
    "src/Timer/Timer.cpp"
    
    "src/Renderer/RenderSystem.cpp"
    "src/Renderer/Pipeline.cpp"
    "src/Renderer/components/camera.cpp"
    "src/Renderer/components/meshrenderer.cpp"
    "src/Renderer/components/material.cpp"
    "src/Renderer/components/mesh.cpp"
    "src/Renderer/components/fog.cpp"
    "src/Renderer/components/scene.cpp"
    "src/Renderer/components/light.cpp"
    "src/Renderer/components/shader.cpp"
    "src/Renderer/components/texture.cpp"
    "src/Renderer/components/framebuffer.cpp"
    
    "src/Renderer/pipeline/accumulateFogLayers.cpp"
    "src/Renderer/pipeline/accumulateLights.cpp"
    "src/Renderer/pipeline/setCamera.cpp"
    
    "src/Renderer/pipeline/meshBinding.cpp"
    "src/Renderer/pipeline/materialBinding.cpp"
    "src/Renderer/pipeline/shaderBinding.cpp"
    
    "src/Renderer/pipeline/passGeometry.cpp"
    "src/Renderer/pipeline/passLevelOfDetail.cpp"
    "src/Renderer/pipeline/passShadowVolume.cpp"
    "src/Renderer/pipeline/passSorting.cpp"
    "src/Renderer/pipeline/passCulling.cpp"
    
    "src/Resources/FileLoader.cpp"
    "src/Resources/FileSystem.cpp"
    "src/Resources/ResourceManager.cpp"
    "src/Resources/assets/colliderTag.cpp"
    "src/Resources/assets/meshTag.cpp"
    "src/Resources/assets/shaderTag.cpp"
    "src/Resources/assets/textureTag.cpp"
    
    "src/Scripting/ScriptSystem.cpp"
    "src/Scripting/components/script.cpp"
    
)



add_library(core SHARED ${CORE_HEADERS} ${CORE_SOURCES})

add_compile_definitions(BUILD_CORE)

target_compile_features(core PUBLIC cxx_std_11)

set_target_properties(core PROPERTIES CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O2")

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
target_link_libraries(core Ws2_32.lib)

target_link_libraries(core ${PROJECT_SOURCE_DIR}/vendor/librp3d.a)
target_link_libraries(core ${PROJECT_SOURCE_DIR}/vendor/OpenAL32.lib)

endif()


