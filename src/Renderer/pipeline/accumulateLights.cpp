#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


void RenderSystem::accumulateSceneLights(Scene* currentScene, glm::vec3 eye) {
    
    std::vector<Light*>& lightList = currentScene->mLightList;
    unsigned int totalNumberOfLights = lightList.size();
    
    unsigned int i;
    for (i=0; i < totalNumberOfLights; i++) {
        
        // Check light list max
        if (mNumberOfLights < RENDER_NUMBER_OF_LIGHTS) {
            
            Light* lightPtr = lightList[i];
            
            // Draw distance
            
            if (lightPtr->type != LIGHT_TYPE_DIRECTIONAL) {
                
                if (glm::distance(eye, lightPtr->position) > lightPtr->renderDistance) 
                    continue;
            }
            
            if (!lightPtr->isActive) 
                continue;
            
            // Add the light to the light list
            mLightPosition[mNumberOfLights]  = lightPtr->position + lightPtr->offset;
            mLightDirection[mNumberOfLights] = lightPtr->direction;
            
            mLightAttenuation[mNumberOfLights].r = lightPtr->intensity;
            mLightAttenuation[mNumberOfLights].g = lightPtr->range;
            mLightAttenuation[mNumberOfLights].b = lightPtr->attenuation;
            mLightAttenuation[mNumberOfLights].a = lightPtr->type;
            
            mLightColor[mNumberOfLights].r = lightPtr->color.r;
            mLightColor[mNumberOfLights].g = lightPtr->color.g;
            mLightColor[mNumberOfLights].b = lightPtr->color.b;
            
            mNumberOfLights++;
            
            // Add directional light to the shadow list
            if (lightPtr->type == LIGHT_TYPE_DIRECTIONAL) {
                
                if (lightPtr->doCastShadow) {
                    
                    if (mNumberOfShadows < RENDER_NUMBER_OF_SHADOWS) {
                        
                        mShadowPosition[mNumberOfShadows]  = lightPtr->offset;
                        mShadowDirection[mNumberOfShadows] = lightPtr->direction;
                        
                        mShadowAttenuation[mNumberOfShadows].r = lightPtr->intensity;
                        mShadowAttenuation[mNumberOfShadows].g = lightPtr->range;
                        mShadowAttenuation[mNumberOfShadows].b = lightPtr->attenuation;
                        mShadowAttenuation[mNumberOfShadows].a = lightPtr->type;
                        
                        mShadowColor[mNumberOfShadows].r = lightPtr->color.r;
                        mShadowColor[mNumberOfShadows].g = lightPtr->color.g;
                        mShadowColor[mNumberOfShadows].b = lightPtr->color.b;
                        
                        mNumberOfShadows++;
                    }
                    
                }
                
                continue;
            }
            
        }
        
        continue;
    }
    
    return;
}

