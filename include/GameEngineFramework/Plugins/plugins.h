#ifndef __PLUGIN_SYSTEMS_
#define __PLUGIN_SYSTEMS_

#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>
#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>
#include <GameEngineFramework/Plugins/InventorySystem/InventorySystem.h>

extern ChunkManager     GameWorld;
extern WeatherSystem    Weather;
extern ParticleSystem   Particle;
extern InventorySystem  Inventory;

#endif
