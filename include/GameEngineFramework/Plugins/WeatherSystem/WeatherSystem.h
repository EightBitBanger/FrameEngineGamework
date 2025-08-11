#ifndef __WEATHER_SYSTEM_
#define __WEATHER_SYSTEM_

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/ParticleSystem/ParticleSystem.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


enum class WeatherType {
    Clear,
    Rain,
    Snow
};


class ENGINE_API WeatherSystem {
    
public:
    
    float weatherStateCounter;
    
    WeatherSystem();
    
    void Initiate(void);
    
    void Update(void);
    
    void SetPlayerObject(GameObject* player);
    
    void SetSkyAmbientColor(Color skyColor);
    
    void SetWorldMaterial(Material* materialPtr);
    void SetStaticMaterial(Material* materialPtr);
    void SetWaterMaterial(Material* materialPtr);
    
    void SetTime(float newTime);
    float GetTime(void);
    
    /// Set the universal light level.
    void SetWorldLightLevel(float min, float max);
    
    /// Set the current weather cycle.
    void SetWeather(WeatherType type);
    
    /// Set the next weather cycle to shift to after the current one has finished.
    void SetWeatherNext(WeatherType type);
    
    /// Add a weather cycle to any currently active weather cycles.
    void AddWeather(WeatherType type);
    
    /// Return the current weather cycle.
    WeatherType GetWeatherCurrent(void);
    
    /// Return the next weather cycle.
    WeatherType GetWeatherNext(void);
    
    /// Get the state of the master weather counter.
    float GetWeatherCycleCounter(void);
    
    /// Set the state of the master weather counter.
    void SetWeatherCycleCounter(float counter);
    
    /// Reset the world fog.
    void FogClear(void);
    
private:
    
    float mWorldTime;
    
    float mWorldLightLow;
    float mWorldLightHigh;
    
    float mLightIntensity;
    glm::vec3 mLightAngle;
    
    // Sun
    GameObject* mSunObject;
    Light* mSunLight;
    Transform* mLightTransform;
    
    // Sky
    GameObject* mSkyObject;
    Material* mSkyMaterial;
    
    // World materials used for lighting
    Material* mWorldMaterial;
    Material* mStaticMaterial;
    Material* mWaterMaterial;
    
    // Which weather type should be shifted into next after the mCurrentWeather weather cycle
    WeatherType mNextWeather;
    // Current weather cycle taking place in the world
    WeatherType mCurrentWeather;
    
    // Weather cycle counters
    float mWeatherMasterCounter;
    float mWeatherShiftCounter;
    float mWeatherFogCounter;
    
    // World fog
    Fog* mFogWorld;
    
    float mWorldFogDensity;
    float mWorldFogNear;
    float mWorldFogFar;
    Color mWorldFogColorNear;
    Color mWorldFogColorFar;
    
    float mFogLightBias;
    
    // Effect emitters
    Emitter* mRainEmitter;
    Emitter* mSnowEmitter;
    
};

#endif
