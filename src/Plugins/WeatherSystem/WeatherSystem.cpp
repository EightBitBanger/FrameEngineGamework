#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/plugins.h>

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>


WeatherSystem::WeatherSystem() : 
    weatherStateCounter(320),
    
    mWorldTime(0000),
    
    mWorldLightLow(0.0087f),
    mWorldLightHigh(0.87f),
    
    mLightIntensity(1.0f),
    mLightAngle(glm::vec3(0)),
    
    mSunObject(nullptr),
    mSunLight(nullptr),
    mLightTransform(nullptr),
    
    mSkyObject(nullptr),
    mSkyMaterial(nullptr),
    
    mWorldMaterial(nullptr),
    mStaticMaterial(nullptr),
    mWaterMaterial(nullptr),
    
    mNextWeather(WeatherType::Clear),
    mCurrentWeather(WeatherType::Clear),
    
    mWeatherMasterCounter(0),
    mWeatherShiftCounter(0),
    mWeatherFogCounter(1),
    
    mFogWorld(nullptr),
    
    mWorldFogDensity(0.0f),
    mWorldFogNear(0.0f),
    mWorldFogFar(0.0f),
    mWorldFogColorNear(Colors.ltgray),
    mWorldFogColorFar(Colors.gray),
    
    mFogLightBias(0.0f),
    
    mRainEmitter(nullptr),
    mSnowEmitter(nullptr)
{
}

void WeatherSystem::Initiate(void) {
    
    //
    // Sun
    
    // Initiate sun angle
    mLightAngle.y = -15.0f;
    
    mSunObject = Engine.Create<GameObject>();
    mSunObject->AddComponent( Engine.CreateComponent<Light>() );
    
    mLightTransform = mSunObject->GetComponent<Transform>();
    
    // Starting direction
    mLightTransform->RotateEuler(0.0f, -1.0f, 0.0f);
    
    // Set light parameters
    mSunLight = mSunObject->GetComponent<Light>();
    
    mSunLight->type       = LIGHT_TYPE_DIRECTIONAL;
    mSunLight->intensity  = 0.7f;
    mSunLight->color      = Colors.white;
    
    Engine.sceneMain->AddLightToSceneRoot( mSunLight );
    
    
    //
    // Sky
    
    // Amount of fade bias from the color "skyHigh" to "skyLow".
    float colorBias = 1.0f;
    
    // Generate the sky which will be returned as a game object.
    // This game object will contain a mesh renderer to draw the sky.
    mSkyObject = Engine.CreateSky("sky", Colors.blue, Colors.blue, colorBias);
    
    // Add the sky's mesh renderer to the main scene.
    Engine.sceneMain->AddMeshRendererToSceneRoot( mSkyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY );
    
    // Sky rendering colors
    MeshRenderer* skyRenderer = mSkyObject->GetComponent<MeshRenderer>();
    mSkyMaterial = skyRenderer->material;
    mSkyMaterial->diffuse = Colors.dkgray;
    mSkyMaterial->ambient = Colors.black;
    
    // World fog
    
    mFogWorld = Renderer.CreateFog();
    
    Engine.sceneMain->AddFogLayerToScene(mFogWorld);
    
    mFogWorld->fogActive = true;
    
    mFogWorld->fogDensity = mWorldFogDensity;
    mFogWorld->fogBegin = mWorldFogNear;
    mFogWorld->fogEnd = mWorldFogFar;
    mFogWorld->fogColorBegin = mWorldFogColorNear;
    mFogWorld->fogColorEnd = mWorldFogColorFar;
    
    
    // Rain emitter
    
    mRainEmitter = Particle.CreateEmitter();
    
    mRainEmitter->type = EmitterType::AreaEffector;
    mRainEmitter->position = glm::vec3(0, 0, 0);
    mRainEmitter->direction = glm::vec3(0, -0.9f, 0);
    mRainEmitter->scale = glm::vec3(0.018f, 0.4f, 0.018f);
    
    mRainEmitter->velocity = glm::vec3(0.0f, -0.9f, 0.0f);
    
    mRainEmitter->width = 40.0f;
    mRainEmitter->height = 70.0f;
    
    mRainEmitter->colorBegin = Colors.Lerp(Colors.blue, Colors.ltgray, 0.2f);
    mRainEmitter->colorEnd = Colors.Lerp(Colors.blue, Colors.ltgray, 0.7);
    
    mRainEmitter->maxParticles = 2000;
    
    mRainEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    // Snow emitter
    
    mSnowEmitter = Particle.CreateEmitter();
    
    mSnowEmitter->type = EmitterType::AreaEffector;
    mSnowEmitter->position = glm::vec3(0, 0, 0);
    mSnowEmitter->direction = glm::vec3(0, -0.3f, 0);
    mSnowEmitter->scale = glm::vec3(0.05f, 0.02f, 0.05f);
    
    mSnowEmitter->velocity = glm::vec3(0.0f, -0.03f, 0.0f);
    
    mSnowEmitter->width = 30.0f;
    mSnowEmitter->height = 70.0f;
    
    mSnowEmitter->colorBegin = Colors.white;
    mSnowEmitter->colorEnd = Colors.white;
    
    mSnowEmitter->maxParticles = 2000;
    
    mSnowEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    // Zero the weather systems
    SetWeather( WeatherType::Clear );
}



void WeatherSystem::Update(void) {
    
    // Advance world time
    mWorldTime += 0.25f;
    
    if (mWorldTime > 24000.0f) 
        mWorldTime = 0.0f;
    
    float fullDayRange = mWorldTime / 24000.0f;
    float lightRiseFall;
    
    if (fullDayRange > 0.5f) {
        
        lightRiseFall = Float.Lerp(1.0f, 0.0f, fullDayRange);
        
    } else {
        
        lightRiseFall = Float.Lerp(0.0f, 1.0f, fullDayRange);
    }
    
    float mLightIntensity = lightRiseFall - 0.224f;
    
    if (mLightIntensity < 0.0f) 
        mLightIntensity = 0.0f;
    
    mLightIntensity *= 8.0f;
    
    if (mLightIntensity > 1.0f) 
        mLightIntensity = 1.0f;
    
    // Calculate sun angle
    mLightAngle.z = Float.Lerp(-90.0f, 90.0f, fullDayRange);
    
    mLightTransform->SetIdentity();
    mLightTransform->RotateEuler(mLightAngle.x, mLightAngle.y, mLightAngle.z);
    
    // Sun brightness
    mSunLight->intensity = Float.Lerp(mWorldLightLow, mWorldLightHigh, mLightIntensity);
    
    // World lighting
    mWorldMaterial->ambient  = Colors.Lerp(Colors.MakeGrayScale(0.2f), Colors.MakeGrayScale(0.45f), mLightIntensity);
    mStaticMaterial->ambient = Colors.Lerp(Colors.MakeGrayScale(0.2f), Colors.MakeGrayScale(0.45f), mLightIntensity);
    
    // Water lighting
    Color waterColorLow  = Colors.blue;
    Color waterColorHigh = Colors.blue;
    
    waterColorLow  *= Colors.MakeGrayScale(0.2f);
    waterColorHigh *= Colors.MakeGrayScale(0.7f);
    
    mWaterMaterial->diffuse  = Colors.Lerp(waterColorLow, waterColorHigh, mLightIntensity);
    
    // Set sky color
    float skyColor = Math.Lerp(mWorldLightLow, mWorldLightHigh, mLightIntensity);
    SetSkyAmbientColor( Colors.MakeGrayScale(skyColor) );
    
    // Interpolate fog cycle
    float fogBiasMax = 36000.0f;
    
    if (mWeatherFogCounter != 0) {
        
        mWeatherFogCounter++;
        
        if (mWeatherFogCounter > fogBiasMax) 
            mWeatherFogCounter = 0;
        
    }
    
    mFogLightBias = Float.Lerp(mFogLightBias, mLightIntensity, 0.18f);
    
    float fogBias = mWeatherFogCounter / fogBiasMax;
    
    // Calculate fog color from time
    
    Color finalColorNear = Colors.Lerp(mFogWorld->fogColorBegin, mWorldFogColorNear, fogBias);
    Color finalColorFar  = Colors.Lerp(mFogWorld->fogColorEnd, mWorldFogColorFar, fogBias);
    
    finalColorNear = Colors.Lerp(Colors.black, finalColorNear, mFogLightBias);
    finalColorFar  = Colors.Lerp(Colors.black, finalColorFar, mFogLightBias);
    
    mFogWorld->fogDensity = Float.Lerp(mFogWorld->fogDensity, mWorldFogDensity, fogBias);
    mFogWorld->fogBegin = Float.Lerp(mFogWorld->fogBegin, mWorldFogNear, fogBias);
    mFogWorld->fogEnd = Float.Lerp(mFogWorld->fogEnd, mWorldFogFar, fogBias);
    mFogWorld->fogColorBegin = finalColorNear;
    mFogWorld->fogColorEnd = finalColorFar;
    
    // Shift to the next weather cycle
    if (mNextWeather != mCurrentWeather) {
        // Begin shift
        if (mWeatherShiftCounter == 0) {
            AddWeather(mNextWeather);
            
            // Trigger the fog to begin shifting
            mWeatherFogCounter = 1;
        }
        
        mWeatherShiftCounter++;
        if (mWeatherShiftCounter > weatherStateCounter) {
            mWeatherShiftCounter = 0;
            
            // Finish shift
            SetWeather(mNextWeather);
            mCurrentWeather = mNextWeather;
        }
    }
}

void WeatherSystem::SetSkyAmbientColor(Color skyColor) {
    mSkyMaterial->ambient = skyColor;
}

void WeatherSystem::SetPlayerObject(GameObject* player) {
    if (mSkyObject == nullptr) 
        return;
    mSkyObject->GetComponent<Transform>()->parent = player->GetComponent<Transform>();
}

void WeatherSystem::SetWorldMaterial(Material* materialPtr) {
    mWorldMaterial = materialPtr;
}

void WeatherSystem::SetStaticMaterial(Material* materialPtr) {
    mStaticMaterial = materialPtr;
}

void WeatherSystem::SetWaterMaterial(Material* materialPtr) {
    mWaterMaterial = materialPtr;
}

void WeatherSystem::SetTime(float newTime) {
    mWorldTime = newTime;
}

float WeatherSystem::GetTime(void) {
    return mWorldTime;
}

void WeatherSystem::SetWorldLightLevel(float low, float high) {
    mWorldLightLow = low;
    mWorldLightHigh = high;
}

void WeatherSystem::SetWeather(WeatherType type) {
    
    if (type == WeatherType::Clear) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Deactivate();
        
        FogClear();
        
        return;
    }
    
    if (type == WeatherType::Rain) {
        mRainEmitter->Activate();
        mSnowEmitter->Deactivate();
        
        mWeatherFogCounter = 1;
        
        mWorldFogDensity   = 0.87f;
        mWorldFogNear      = 30.0f;
        mWorldFogFar       = 200.0f;
        mWorldFogColorNear = Colors.gray;
        mWorldFogColorFar  = Colors.gray;
        
        return;
    }
    
    if (type == WeatherType::Snow) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Activate();
        
        mWeatherFogCounter = 1;
        
        mWorldFogDensity   = 1.1f;
        mWorldFogNear      = 20.0f;
        mWorldFogFar       = 400.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.white;
        
        return;
    }
}

void WeatherSystem::SetWeatherNext(WeatherType type) {
    mNextWeather = type;
    if (type == WeatherType::Clear) {
        FogClear();
        return;
    }
    
    if (type == WeatherType::Rain) {
        mWeatherFogCounter = 1;
        
        mWorldFogDensity   = 1.0f;
        mWorldFogNear      = 100.0f;
        mWorldFogFar       = 200.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.gray;
        return;
    }
    
    if (type == WeatherType::Snow) {
        mWeatherFogCounter = 1;
        
        mWorldFogDensity   = 8.0f;
        mWorldFogNear      = 100.0f;
        mWorldFogFar       = 200.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.white;
        
        return;
    }
}

void WeatherSystem::AddWeather(WeatherType type) {
    
    // Only add in weather of the appropriate type
    
    if (type == WeatherType::Clear) {
        return;
    }
    
    if (type == WeatherType::Rain) {
        mRainEmitter->Activate();
        return;
    }
    
    if (type == WeatherType::Snow) {
        mSnowEmitter->Activate();
        return;
    }
}

WeatherType WeatherSystem::GetWeatherCurrent(void) {
    return mCurrentWeather;
}

WeatherType WeatherSystem::GetWeatherNext(void) {
    return mNextWeather;
}

float WeatherSystem::GetWeatherCycleCounter(void) {
    return mWeatherMasterCounter;
}

void WeatherSystem::SetWeatherCycleCounter(float counter) {
    mWeatherMasterCounter = counter;
}

void WeatherSystem::FogClear(void) {
    
    // Trigger the fog to shift
    mWeatherFogCounter = 1;
    
    mWorldFogDensity = 10.0f;
    mWorldFogNear = 80.0f;
    mWorldFogFar = 8000.0f;
    
    mWorldFogColorNear = Colors.ltgray;
    mWorldFogColorFar = Colors.Lerp(Colors.blue, Colors.gray, 0.7f);
}

