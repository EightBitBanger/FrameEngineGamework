#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::accumulateSceneFogLayers(Scene* currentScene) {
    
    // Accumulate fog layers
    mNumberOfFogLayers = 0;
    
    unsigned int numberOfLayers = currentScene->mFogLayers.size();
    
    for (unsigned int i=0; i < numberOfLayers; i++) {
        
        Fog* fogLayer = currentScene->mFogLayers[i];
        
        if (!fogLayer->fogActive) 
            continue;
        
        mFogDensity[mNumberOfFogLayers]       = fogLayer->fogDensity;
        mFogHeightCutoff[mNumberOfFogLayers]  = fogLayer->fogHeightCutoff;
        mFogBegin[mNumberOfFogLayers]         = fogLayer->fogBegin;
        mFogEnd[mNumberOfFogLayers]           = fogLayer->fogEnd;
        mFogColorBegin[mNumberOfFogLayers]    = glm::vec3(fogLayer->fogColorBegin.r, fogLayer->fogColorBegin.g, fogLayer->fogColorBegin.b);
        mFogColorEnd[mNumberOfFogLayers]      = glm::vec3(fogLayer->fogColorEnd.r, fogLayer->fogColorEnd.g, fogLayer->fogColorEnd.b);
        
        mNumberOfFogLayers++;
        
        continue;
    }
    
    return;
}

