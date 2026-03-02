#ifndef __WEATHER_EFFECT_
#define __WEATHER_EFFECT_

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>

class ENGINE_API Weather {
public:
    
    float fogDensity;
    float fogNear;
    float fogFar;
    Color fogColorNear;
    Color fogColorFar;
    
    
    
    Weather();
    
};

#endif
