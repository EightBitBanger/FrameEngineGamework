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



#
# Build configuration
#

if(EVENT_LOG_DETAILED)
    add_compile_definitions(EVENT_LOG_DETAILED)
endif()

if(RUN_UNIT_TESTS)
    add_compile_definitions(RUN_UNIT_TESTS)
endif()






if(BUILD_RUNTIME_EXECUTABLE)

if(BUILD_CORE_ENGINE OR BUILD_APPLICATION_LIBRARY)
    message(FATAL_ERROR "Multiple project builds are set. Cmake can only build one project at a time.")
endif()

set (RUNTIME_HEADERS
    
    "include/GameEngineFramework/configuration.h"
    
    "include/GameEngineFramework/Application/Platform.h"
    "include/GameEngineFramework/Application/winproc.h"
    "include/GameEngineFramework/Application/main.h"
    
    "include/GameEngineFramework/Audio/AudioSystem.h"
    
    "include/GameEngineFramework/ActorAI/ActorSystem.h"
    "include/GameEngineFramework/ActorAI/ActorStates.h"
    "include/GameEngineFramework/ActorAI/GeneticPresets.h"
    "include/GameEngineFramework/ActorAI/genetics/gene.h"
    "include/GameEngineFramework/ActorAI/genetics/base.h"
    "include/GameEngineFramework/ActorAI/components/actor.h"
    "include/GameEngineFramework/ActorAI/neurons/NeuralLayer.h"
    "include/GameEngineFramework/ActorAI/neurons/WeightedLayer.h"
    
    "include/GameEngineFramework/Serialization/Serialization.h"
    
    "include/GameEngineFramework/Transform/Transform.h"
    
    "include/GameEngineFramework/Math/Math.h"
    "include/GameEngineFramework/Math/Random.h"
    
    "include/GameEngineFramework/Networking/NetworkSystem.h"
    
    "include/GameEngineFramework/Engine/Engine.h"
    "include/GameEngineFramework/Engine/EngineSystems.h"
    "include/GameEngineFramework/Engine/ComponentTypes.h"
    
    "include/GameEngineFramework/Engine/components/component.h"
    "include/GameEngineFramework/Engine/components/gameobject.h"
    
    "include/GameEngineFramework/Engine/types/bufferlayout.h"
    "include/GameEngineFramework/Engine/types/color.h"
    "include/GameEngineFramework/Engine/types/viewport.h"
    
    "include/GameEngineFramework/Engine/UI/text.h"
    "include/GameEngineFramework/Engine/UI/sprite.h"
    "include/GameEngineFramework/Engine/UI/canvas.h"
    "include/GameEngineFramework/Engine/UI/panel.h"
    "include/GameEngineFramework/Engine/UI/button.h"
    
    "include/GameEngineFramework/Input/InputSystem.h"
    "include/GameEngineFramework/Input/keys.h"
    
    "include/GameEngineFramework/Physics/PhysicsSystem.h"
    "include/GameEngineFramework/Physics/components/meshcollider.h"
    
    "include/GameEngineFramework/Profiler/Profiler.h"
    "include/GameEngineFramework/Types/Types.h"
    "include/GameEngineFramework/Logging/Logging.h"
    "include/GameEngineFramework/Timer/Timer.h"
    "include/GameEngineFramework/MemoryAllocation/PoolAllocator.h"
    
    "include/GameEngineFramework/Renderer/enumerators.h"
    "include/GameEngineFramework/Renderer/RenderSystem.h"
    "include/GameEngineFramework/Renderer/components/camera.h"
    "include/GameEngineFramework/Renderer/components/meshrenderer.h"
    "include/GameEngineFramework/Renderer/components/material.h"
    "include/GameEngineFramework/Renderer/components/mesh.h"
    "include/GameEngineFramework/Renderer/components/submesh.h"
    "include/GameEngineFramework/Renderer/components/scene.h"
    "include/GameEngineFramework/Renderer/components/light.h"
    "include/GameEngineFramework/Renderer/components/shader.h"
    "include/GameEngineFramework/Renderer/components/texture.h"
    "include/GameEngineFramework/Renderer/components/framebuffer.h"
    
    "include/GameEngineFramework/Resources/FileLoader.h"
    "include/GameEngineFramework/Resources/FileSystem.h"
    "include/GameEngineFramework/Resources/ResourceManager.h"
    "include/GameEngineFramework/Resources/assets/colliderTag.h"
    "include/GameEngineFramework/Resources/assets/meshTag.h"
    "include/GameEngineFramework/Resources/assets/shaderTag.h"
    "include/GameEngineFramework/Resources/assets/textureTag.h"
    
    "include/GameEngineFramework/Scripting/ScriptSystem.h"
    "include/GameEngineFramework/Scripting/components/script.h"
    
    "tests/framework.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
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








if(BUILD_APPLICATION_LIBRARY)

if(BUILD_CORE_ENGINE OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

set (GAME_HEADERS
    
    "include/GameEngineFramework/configuration.h"
    
    "include/GameEngineFramework/Application/Platform.h"
    "include/GameEngineFramework/Application/winproc.h"
    "include/GameEngineFramework/Application/main.h"
    
    "include/GameEngineFramework/plugins/ChunkSpawner/ChunkManager.h"
    "include/GameEngineFramework/plugins/ChunkSpawner/Chunk.h"
    "include/GameEngineFramework/plugins/ChunkSpawner/Perlin.h"
    
    "include/GameEngineFramework/Audio/AudioSystem.h"
    
    "include/GameEngineFramework/ActorAI/ActorSystem.h"
    "include/GameEngineFramework/ActorAI/ActorStates.h"
    "include/GameEngineFramework/ActorAI/GeneticPresets.h"
    "include/GameEngineFramework/ActorAI/genetics/gene.h"
    "include/GameEngineFramework/ActorAI/genetics/base.h"
    "include/GameEngineFramework/ActorAI/components/actor.h"
    "include/GameEngineFramework/ActorAI/neurons/NeuralLayer.h"
    "include/GameEngineFramework/ActorAI/neurons/WeightedLayer.h"
    
    "include/GameEngineFramework/Serialization/Serialization.h"
    
    "include/GameEngineFramework/Transform/Transform.h"
    
    "include/GameEngineFramework/Math/Math.h"
    "include/GameEngineFramework/Math/Random.h"
    
    "include/GameEngineFramework/Networking/NetworkSystem.h"
    
    "include/GameEngineFramework/Engine/Engine.h"
    "include/GameEngineFramework/Engine/EngineSystems.h"
    "include/GameEngineFramework/Engine/ComponentTypes.h"
    
    "include/GameEngineFramework/Engine/components/component.h"
    "include/GameEngineFramework/Engine/components/gameobject.h"
    
    "include/GameEngineFramework/Engine/types/bufferlayout.h"
    "include/GameEngineFramework/Engine/types/color.h"
    "include/GameEngineFramework/Engine/types/viewport.h"
    
    "include/GameEngineFramework/Engine/UI/text.h"
    "include/GameEngineFramework/Engine/UI/sprite.h"
    "include/GameEngineFramework/Engine/UI/canvas.h"
    "include/GameEngineFramework/Engine/UI/panel.h"
    "include/GameEngineFramework/Engine/UI/button.h"
    
    "include/GameEngineFramework/Input/InputSystem.h"
    "include/GameEngineFramework/Input/keys.h"
    
    "include/GameEngineFramework/Physics/PhysicsSystem.h"
    "include/GameEngineFramework/Physics/components/meshcollider.h"
    
    "include/GameEngineFramework/Profiler/Profiler.h"
    
    "include/GameEngineFramework/Types/Types.h"
    "include/GameEngineFramework/Logging/Logging.h"
    "include/GameEngineFramework/Timer/Timer.h"
    "include/GameEngineFramework/MemoryAllocation/PoolAllocator.h"
    
    "include/GameEngineFramework/Renderer/enumerators.h"
    "include/GameEngineFramework/Renderer/RenderSystem.h"
    "include/GameEngineFramework/Renderer/components/camera.h"
    "include/GameEngineFramework/Renderer/components/meshrenderer.h"
    "include/GameEngineFramework/Renderer/components/material.h"
    "include/GameEngineFramework/Renderer/components/mesh.h"
    "include/GameEngineFramework/Renderer/components/submesh.h"
    "include/GameEngineFramework/Renderer/components/scene.h"
    "include/GameEngineFramework/Renderer/components/light.h"
    "include/GameEngineFramework/Renderer/components/shader.h"
    "include/GameEngineFramework/Renderer/components/texture.h"
    "include/GameEngineFramework/Renderer/components/framebuffer.h"
    
    "include/GameEngineFramework/Resources/FileLoader.h"
    "include/GameEngineFramework/Resources/FileSystem.h"
    "include/GameEngineFramework/Resources/ResourceManager.h"
    "include/GameEngineFramework/Resources/assets/colliderTag.h"
    "include/GameEngineFramework/Resources/assets/meshTag.h"
    "include/GameEngineFramework/Resources/assets/shaderTag.h"
    "include/GameEngineFramework/Resources/assets/textureTag.h"
    
    "include/GameEngineFramework/Scripting/ScriptSystem.h"
    "include/GameEngineFramework/Scripting/components/script.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
)

set (GAME_SOURCES
    
    "src/application.cpp"
    
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











if(BUILD_CORE_ENGINE)

if(BUILD_APPLICATION_LIBRARY OR BUILD_RUNTIME_EXECUTABLE)
    message(FATAL_ERROR "Multiple build options are set. Please only select one build option at a time.")
endif()

set (CORE_HEADERS
    
    "include/GameEngineFramework/Application/Platform.h"
    "include/GameEngineFramework/Application/winproc.h"
    
    "include/GameEngineFramework/Audio/AudioSystem.h"
    
    "include/GameEngineFramework/ActorAI/ActorSystem.h"
    "include/GameEngineFramework/ActorAI/ActorStates.h"
    "include/GameEngineFramework/ActorAI/GeneticPresets.h"
    "include/GameEngineFramework/ActorAI/genetics/gene.h"
    "include/GameEngineFramework/ActorAI/genetics/base.h"
    "include/GameEngineFramework/ActorAI/components/actor.h"
    "include/GameEngineFramework/ActorAI/neurons/NeuralLayer.h"
    "include/GameEngineFramework/ActorAI/neurons/WeightedLayer.h"
    
    "include/GameEngineFramework/Serialization/Serialization.h"
    
    "include/GameEngineFramework/Transform/Transform.h"
    
    "include/GameEngineFramework/Math/Math.h"
    "include/GameEngineFramework/Math/Random.h"
    
    "include/GameEngineFramework/Networking/NetworkSystem.h"
    
    "include/GameEngineFramework/Engine/Engine.h"
    "include/GameEngineFramework/Engine/EngineSystems.h"
    "include/GameEngineFramework/Engine/ComponentTypes.h"
    
    "include/GameEngineFramework/Engine/components/component.h"
    "include/GameEngineFramework/Engine/components/gameobject.h"
    
    "include/GameEngineFramework/Engine/types/bufferlayout.h"
    "include/GameEngineFramework/Engine/types/color.h"
    "include/GameEngineFramework/Engine/types/viewport.h"
    
    "include/GameEngineFramework/Engine/UI/text.h"
    "include/GameEngineFramework/Engine/UI/sprite.h"
    "include/GameEngineFramework/Engine/UI/canvas.h"
    "include/GameEngineFramework/Engine/UI/panel.h"
    "include/GameEngineFramework/Engine/UI/button.h"
    
    "include/GameEngineFramework/Input/InputSystem.h"
    "include/GameEngineFramework/Input/keys.h"
    
    "include/GameEngineFramework/Physics/PhysicsSystem.h"
    "include/GameEngineFramework/Physics/components/meshcollider.h"
    
    "include/GameEngineFramework/Profiler/Profiler.h"
    
    "include/GameEngineFramework/Types/Types.h"
    "include/GameEngineFramework/Logging/Logging.h"
    "include/GameEngineFramework/Timer/Timer.h"
    "include/GameEngineFramework/MemoryAllocation/PoolAllocator.h"
    
    "include/GameEngineFramework/Renderer/enumerators.h"
    "include/GameEngineFramework/Renderer/RenderSystem.h"
    "include/GameEngineFramework/Renderer/components/camera.h"
    "include/GameEngineFramework/Renderer/components/meshrenderer.h"
    "include/GameEngineFramework/Renderer/components/material.h"
    "include/GameEngineFramework/Renderer/components/mesh.h"
    "include/GameEngineFramework/Renderer/components/submesh.h"
    "include/GameEngineFramework/Renderer/components/scene.h"
    "include/GameEngineFramework/Renderer/components/light.h"
    "include/GameEngineFramework/Renderer/components/shader.h"
    "include/GameEngineFramework/Renderer/components/texture.h"
    "include/GameEngineFramework/Renderer/components/framebuffer.h"
    
    "include/GameEngineFramework/Resources/FileLoader.h"
    "include/GameEngineFramework/Resources/FileSystem.h"
    "include/GameEngineFramework/Resources/ResourceManager.h"
    "include/GameEngineFramework/Resources/assets/colliderTag.h"
    "include/GameEngineFramework/Resources/assets/meshTag.h"
    "include/GameEngineFramework/Resources/assets/shaderTag.h"
    "include/GameEngineFramework/Resources/assets/textureTag.h"
    
    "include/GameEngineFramework/Scripting/ScriptSystem.h"
    "include/GameEngineFramework/Scripting/components/script.h"
    
    "vendor/Bly7/OBJ_Loader.h"
    
    "vendor/stb/stb_image.h"
    "vendor/stb/stb_perlin.h"
    "vendor/stb/stb_truetype.h"
    
)

set (CORE_SOURCES
    
    "src/Application/Platform.cpp"
    "src/Application/winproc.cpp"
    
    "src/Audio/AudioSystem.cpp"
    
    "src/Networking/NetworkSystem.cpp"
    
    "src/Engine/Engine.cpp"
    "src/Engine/EngineUpdate.cpp"
    "src/Engine/EngineConsole.cpp"
    "src/Engine/EngineGameObjects.cpp"
    "src/Engine/EngineComponents.cpp"
    "src/Engine/EngineDeferredDeletion.cpp"
    "src/Engine/EngineUpdateComponentStream.cpp"
    
    "src/Engine/componentUpdate/EngineUpdateUI.cpp"
    "src/Engine/componentUpdate/EngineUpdateAI.cpp"
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
    "src/Engine/types/viewport.cpp"
    
    "src/ActorAI/ActorSystem.cpp"
    "src/ActorAI/ActorUpdate.cpp"
    "src/ActorAI/GeneticPresets.cpp"
    "src/ActorAI/genetics/gene.cpp"
    "src/ActorAI/genetics/base.cpp"
    "src/ActorAI/components/actor.cpp"
    "src/ActorAI/neurons/NeuralLayer.cpp"
    "src/ActorAI/neurons/WeightedLayer.cpp"
    
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
    "src/Renderer/components/scene.cpp"
    "src/Renderer/components/light.cpp"
    "src/Renderer/components/shader.cpp"
    "src/Renderer/components/texture.cpp"
    "src/Renderer/components/framebuffer.cpp"
    
    "src/Renderer/pipeline/accumulateLights.cpp"
    "src/Renderer/pipeline/setCamera.cpp"
    
    "src/Renderer/pipeline/meshBinding.cpp"
    "src/Renderer/pipeline/materialBinding.cpp"
    "src/Renderer/pipeline/shaderBinding.cpp"
    
    "src/Renderer/pipeline/passGeometry.cpp"
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

endif()


