#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

extern ParticleSystem  particle;
extern ChunkManager    chunkManager;

WeatherSystem::WeatherSystem() : 
    weatherStateCounter(120),
    
    mWorldTime(9000),
    
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
    mSkyMaterial->ambient = Colors.white;
    
    
    // Rain emitter
    
    mRainEmitter = particle.CreateEmitter();
    
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
    
    mRainEmitter->heightMinimum = chunkManager.world.waterLevel;
    
    
    // Snow emitter
    
    mSnowEmitter = particle.CreateEmitter();
    
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
    
    mSnowEmitter->heightMinimum = chunkManager.world.waterLevel;
    
    // Zero the weather systems
    SetWeather( WeatherType::Clear );
    
    return;
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
    mSunLight->intensity = Float.Lerp(0.0f, 0.87f, mLightIntensity);
    
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
    float skyColor = Math.Lerp(0.0087f, 0.87f, mLightIntensity);
    SetSkyAmbientColor( Colors.MakeGrayScale(skyColor) );
    
    
    // Update weather cycle
    
    if (mNextWeather != mCurrentWeather) {
        
        // Begin shift
        
        if (mWeatherShiftCounter == 0) {
            
            AddWeather(mNextWeather);
            
        }
        
        mWeatherShiftCounter++;
        if (mWeatherShiftCounter > weatherStateCounter) {
            mWeatherShiftCounter = 0;
            
            // Finish shift
            
            SetWeather(mNextWeather);
            
            mCurrentWeather = mNextWeather;
        }
        
    }
    
    
    return;
}

void WeatherSystem::SetSkyAmbientColor(Color skyColor) {
    
    mSkyMaterial->ambient = skyColor;
    
    return;
}

void WeatherSystem::SetPlayerObject(GameObject* player) {
    
    if (mSkyObject == nullptr) 
        return;
    
    mSkyObject->GetComponent<Transform>()->parent = player->GetComponent<Transform>();
    
    return;
}

void WeatherSystem::SetWorldMaterial(Material* materialPtr) {
    
    mWorldMaterial = materialPtr;
    
    return;
}

void WeatherSystem::SetStaticMaterial(Material* materialPtr) {
    
    mStaticMaterial = materialPtr;
    
    return;
}

void WeatherSystem::SetWaterMaterial(Material* materialPtr) {
    
    mWaterMaterial = materialPtr;
    
    return;
}

void WeatherSystem::SetTime(float newTime) {
    
    mWorldTime = newTime;
    
    return;
}

float WeatherSystem::GetTime(void) {
    
    return mWorldTime;
}

void WeatherSystem::SetWeather(WeatherType type) {
    
    if (type == WeatherType::Clear) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Deactivate();
        return;
    }
    
    if (type == WeatherType::Cloudy) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Deactivate();
        return;
    }
    
    if (type == WeatherType::Rain) {
        mRainEmitter->Activate();
        mSnowEmitter->Deactivate();
        return;
    }
    
    if (type == WeatherType::Snow) {
        mRainEmitter->Deactivate();
        mSnowEmitter->Activate();
        return;
    }
    
    return;
}

void WeatherSystem::SetWeatherNextCycle(WeatherType type) {
    
    mNextWeather = type;
    
    return;
}

void WeatherSystem::AddWeather(WeatherType type) {
    
    // Only add in weather of the appropriate type
    
    if (type == WeatherType::Clear) {
        return;
    }
    
    if (type == WeatherType::Cloudy) {
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
    
    return;
}

WeatherType WeatherSystem::GetWeather(void) {
    
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
    
    return;
}

