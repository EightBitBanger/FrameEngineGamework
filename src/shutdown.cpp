#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>


void Shutdown(void) {
    
    //if (GameWorld.world.doGenerateChunks) 
    //    GameWorld.SaveWorld();
    
    GameWorld.isChunkGenerationActive = false;
    
    return;
}
