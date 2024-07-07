#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::BindShader(Shader* shaderPtr) {
    
    if (mCurrentShader == shaderPtr) 
        return false;
    
    mCurrentShader = shaderPtr;
    
    mCurrentShader->Bind();
    
    mCurrentShader->SetTextureSampler(0);
    
    // Send in the light list
    mCurrentShader->SetLightCount(mNumberOfLights);
    mCurrentShader->SetLightPositions(mNumberOfLights, mLightPosition);
    mCurrentShader->SetLightDirections(mNumberOfLights, mLightDirection);
    mCurrentShader->SetLightAttenuation(mNumberOfLights, mLightAttenuation);
    mCurrentShader->SetLightColors(mNumberOfLights, mLightColor);
    
    return true;
}

