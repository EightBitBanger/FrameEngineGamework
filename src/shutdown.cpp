#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>


void Shutdown(void) {
    
    return;
    
    if (chunkManager.world.doGenerateChunks) 
        chunkManager.SaveWorld();
    
    chunkManager.world.doGenerateChunks = false;
    
    return;
}

