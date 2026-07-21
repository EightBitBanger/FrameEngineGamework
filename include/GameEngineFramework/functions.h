#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>
#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>

void MainMenuEnable(void);
void MainMenuDisable(void);

void keyBindFullscreen(void);       // Toggle full screen
void keyBindEscape(void);           // Toggle pause/console
void keyBindF4();                   // Toggle debug stats

void FuncList(std::vector<std::string> args);     // List worlds
void FuncSave(std::vector<std::string> args);     // Save world
void FuncLoad(std::vector<std::string> args);     // Load world
void FuncRemove(std::vector<std::string> args);   // Remove a world
void FuncClear(std::vector<std::string> args);    // Clear world
void FuncSeed(std::vector<std::string> args);     // Set the world seed
void FuncSummon(std::vector<std::string> args);   // Summon an actor
void FuncTime(std::vector<std::string> args);     // Time control
void FuncWeather(std::vector<std::string> args);  // Weather control

void EventLostFocus(void);                        // Window lost focus callback
void EventOnResize(void);                         // Window resized callback
