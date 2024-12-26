#ifndef __WEATHER_SYSTEM_
#define __WEATHER_SYSTEM_

#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>


class ENGINE_API WeatherSystem {
    
public:
    
    void SetPlayerObject(GameObject* player);
    
    void Initiate(void);
    
    void Update(void);
    
    void SetSkyAmbientColor(Color skyColor);
    
    
private:
    
    /// Sun
    GameObject* sunObject;
    Light* sunLight;
    
    /// Sky
    GameObject* skyObject;
    Material* skyMaterial;
    
};

#endif
