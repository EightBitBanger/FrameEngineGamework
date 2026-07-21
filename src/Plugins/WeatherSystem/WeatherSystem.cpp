#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/plugins.h>

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>

WeatherSystem::WeatherSystem() :
    weatherStateCounter(320),
    
    mWorldTime(0.0f),
    
    mWorldLightLow(0.0087f),
    mWorldLightHigh(0.87f),
    
    mSkyLightLow(0.0087f),
    mSkyLightHigh(0.87f),
    
    mLightIntensity(1.0f),
    mLightAngle(glm::vec3(0)),
    
    mPlayerTransform(nullptr),
    
    mSunObject(nullptr),
    mSunLight(nullptr),
    mLightTransform(nullptr),
    
    mSkyObject(nullptr),
    mSkyMaterial(nullptr),
    
    mWorldMaterial(nullptr),
    mStaticMaterial(nullptr),
    mWaterMaterial(nullptr),
    
    mWorldAmbientBase(Colors.ltgray),
    mStaticAmbientBase(Colors.ltgray),
    mCapturedAmbientBase(false),
    
    mNextWeather(WeatherType::Clear),
    mCurrentWeather(WeatherType::Clear),
    
    mWeatherMasterCounter(0.0f),
    mWeatherShiftCounter(0.0f),
    mWeatherFogCounter(1.0f),
    
    mFogWorld(nullptr),
    
    mWorldFogDensity(0.0f),
    mWorldFogNear(0.0f),
    mWorldFogFar(0.0f),
    mWorldFogColorNear(Colors.ltgray),
    mWorldFogColorFar(Colors.gray),
    
    mFogLightBias(0.0f),
    
    mWorldWaterLevel(-10.0f),
    mFogWater(nullptr),
    
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
    mSunObject->AddComponent(Engine.CreateComponent<Light>());
    
    mLightTransform = mSunObject->GetComponent<Transform>();
    
    // Starting direction
    mLightTransform->RotateEuler(0.0f, -1.0f, 0.0f);
    
    // Set light parameters
    mSunLight = mSunObject->GetComponent<Light>();
    
    mSunLight->type      = LIGHT_TYPE_DIRECTIONAL;
    mSunLight->intensity = 0.7f;
    mSunLight->color     = Colors.white;
    
    Engine.sceneMain->AddLightToSceneRoot(mSunLight);
    
    //
    // Sky
    
    // Amount of fade bias from the color "skyHigh" to "skyLow".
    float colorBias = 1.0f;
    
    // Generate the sky which will be returned as a game object.
    // This game object will contain a mesh renderer to draw the sky.
    mSkyObject = Weather.CreateSky("sky", Colors.blue, Colors.blue, colorBias);
    
    // Add the sky's mesh renderer to the main scene.
    Engine.sceneMain->AddMeshRendererToSceneRoot(mSkyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY);
    
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
    mRainEmitter->scale = glm::vec3(0.012f, 0.4f, 0.012f);
    mRainEmitter->velocity = glm::vec3(0.0f, -0.8f, 0.0f);
    mRainEmitter->width = 40.0f;
    mRainEmitter->height = 70.0f;
    mRainEmitter->colorBegin = Colors.Lerp(Colors.blue, Colors.ltgray, 0.2f);
    mRainEmitter->colorEnd = Colors.Lerp(Colors.blue, Colors.ltgray, 0.7f);
    mRainEmitter->maxParticles = 2000;
    mRainEmitter->heightMinimum = 0.0f;
    
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
    mSnowEmitter->heightMinimum = 0.0f;
    
    // Water fog layer
    mFogWater = Renderer.CreateFog();
    //Engine.sceneMain->AddFogLayerToScene(mFogWater);
    
    // Zero the weather systems
    SetWeather(WeatherType::Clear);
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
    
    // NOTE: do not shadow the member variable; keep state coherent for debugging.
    mLightIntensity = lightRiseFall - 0.224f;
    
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
    if (mWorldMaterial != nullptr && mStaticMaterial != nullptr) {
        
        // Capture base ambient targets once (or the first time both materials are valid).
        if (!mCapturedAmbientBase) {
            mWorldAmbientBase    = mWorldMaterial->ambient;
            mStaticAmbientBase   = mStaticMaterial->ambient;
            mCapturedAmbientBase = true;
        }
        
        Color ambientWorld  = Colors.Lerp(Colors.black, mWorldAmbientBase,  mLightIntensity);
        Color ambientStatic = Colors.Lerp(Colors.black, mStaticAmbientBase, mLightIntensity);
        
        Renderer.SetAmbientIllumination(ambientWorld);
        
        mWorldMaterial->ambient  = ambientWorld;
        mStaticMaterial->ambient = ambientStatic;
    }
    
    // Water lighting
    Color waterColorLow  = Colors.blue;
    Color waterColorHigh = Colors.blue;
    
    waterColorLow  *= Colors.MakeGrayScale(0.2f);
    waterColorHigh *= Colors.MakeGrayScale(0.7f);
    
    if (mWaterMaterial != nullptr) {
        mWaterMaterial->diffuse = Colors.Lerp(waterColorLow, waterColorHigh, mLightIntensity);
    }
    
    // Set sky color
    float skyColor = Math.Lerp(mSkyLightLow, mSkyLightHigh, mLightIntensity);
    SetSkyAmbientColor(Colors.MakeGrayScale(skyColor));
    
    // Interpolate fog cycle
    float fogBiasMax = 36000.0f;
    
    if (mWeatherFogCounter != 0.0f) {
        mWeatherFogCounter++;
        
        if (mWeatherFogCounter > fogBiasMax)
            mWeatherFogCounter = 0.0f;
    }
    
    mFogLightBias = Float.Lerp(mFogLightBias, mLightIntensity, 0.18f);
    
    float fogBias = mWeatherFogCounter / fogBiasMax;
    
    // Calculate fog color from time
    
    Color finalColorNear = Colors.Lerp(mFogWorld->fogColorBegin, mWorldFogColorNear, fogBias);
    Color finalColorFar  = Colors.Lerp(mFogWorld->fogColorEnd,   mWorldFogColorFar,  fogBias);
    
    finalColorNear = Colors.Lerp(Colors.black, finalColorNear, mFogLightBias);
    finalColorFar  = Colors.Lerp(Colors.black, finalColorFar,  mFogLightBias);
    
    mFogWorld->fogDensity = Float.Lerp(mFogWorld->fogDensity, mWorldFogDensity, fogBias);
    mFogWorld->fogBegin = Float.Lerp(mFogWorld->fogBegin, mWorldFogNear, fogBias);
    mFogWorld->fogEnd = Float.Lerp(mFogWorld->fogEnd, mWorldFogFar, fogBias);
    mFogWorld->fogColorBegin = finalColorNear;
    mFogWorld->fogColorEnd = finalColorFar;
    
    // Shift to the next weather cycle
    if (mNextWeather != mCurrentWeather) {
        // Begin shift
        if (mWeatherShiftCounter == 0.0f) {
            AddWeather(mNextWeather);
            
            // Trigger the fog to begin shifting
            mWeatherFogCounter = 1.0f;
        }
        
        mWeatherShiftCounter++;
        if (mWeatherShiftCounter > weatherStateCounter) {
            mWeatherShiftCounter = 0.0f;
            
            // Finish shift
            SetWeather(mNextWeather);
            mCurrentWeather = mNextWeather;
        }
    }
    
    // Water effect
    
    if (mPlayerTransform != nullptr) {
        if (mPlayerTransform->position.y < mWorldWaterLevel) {
            mFogWater->fogHeightCutoff = 1000.0f;
            mFogWater->fogDensity = 0.8f;
            mFogWater->fogBegin = 0.0f;
            mFogWater->fogEnd = 24.0f;
            mFogWater->fogColorBegin = Colors.blue;
            mFogWater->fogColorEnd = Colors.blue;
        } else {
            mFogWater->fogHeightCutoff = mWorldWaterLevel;
            mFogWater->fogDensity = 0.8f;
            mFogWater->fogBegin = 0.0f;
            mFogWater->fogEnd = 1.0f;
            mFogWater->fogColorBegin = Colors.blue;
            mFogWater->fogColorEnd = Colors.blue;
        }
    }
}

void WeatherSystem::SetSkyAmbientColor(Color skyColor) {
    mSkyMaterial->ambient = skyColor;
}

void WeatherSystem::SetPlayerObject(GameObject* player) {
    if (mSkyObject == nullptr)
        return;
    mPlayerTransform = player->GetComponent<Transform>();
    mSkyObject->GetComponent<Transform>()->parent = mPlayerTransform;
}

void WeatherSystem::SetWorldMaterial(Material* materialPtr) {
    mWorldMaterial = materialPtr;

    if (mWorldMaterial != nullptr) {
        mWorldAmbientBase    = mWorldMaterial->ambient;
        mCapturedAmbientBase = (mStaticMaterial != nullptr);
    }
}

void WeatherSystem::SetStaticMaterial(Material* materialPtr) {
    mStaticMaterial = materialPtr;

    if (mStaticMaterial != nullptr) {
        mStaticAmbientBase   = mStaticMaterial->ambient;
        mCapturedAmbientBase = (mWorldMaterial != nullptr);
    }
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
    mWorldLightLow  = low;
    mWorldLightHigh = high;
}

void WeatherSystem::SetSkyLightLevel(float low, float high) {
    mSkyLightLow  = low;
    mSkyLightHigh = high;
}

void WeatherSystem::SetFogDensity(float density) {
    mWorldFogDensity = density;
}

void WeatherSystem::SetFogLightBias(float bias) {
    mFogLightBias = bias;
}

void WeatherSystem::SetFogRange(float nearRange, float farRange) {
    mWorldFogNear = nearRange;
    mWorldFogFar  = farRange;
}

void WeatherSystem::SetFogRangeColor(Color nearColor, Color farColor) {
    mWorldFogColorNear = nearColor;
    mWorldFogColorFar  = farColor;
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

        mWeatherFogCounter = 1.0f;

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

        mWeatherFogCounter = 1.0f;

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
        mWeatherFogCounter = 1.0f;

        mWorldFogDensity   = 40.0f;
        mWorldFogNear      = 0.0f;
        mWorldFogFar       = 10.0f;
        mWorldFogColorNear = Colors.ltgray;
        mWorldFogColorFar  = Colors.gray;
        return;
    }

    if (type == WeatherType::Snow) {
        mWeatherFogCounter = 1.0f;

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
    mWeatherFogCounter = 1.0f;

    mWorldFogDensity = 20.0f;
    mWorldFogNear    = 100.0f;
    mWorldFogFar     = 8000.0f;

    mWorldFogColorNear = Colors.ltgray * 0.7f;
    mWorldFogColorFar  = Colors.Lerp(Colors.blue, Colors.gray, 0.8f);
}

GameObject* WeatherSystem::CreateSky(const std::string& meshTagName, Color low, Color high, float biasMul) {
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Color skyHigh = low;
    Color skyLow  = low;
    
    // Set the base range
    skyHigh += Colors.MakeGrayScale(0.997f);
    skyLow  += Colors.MakeGrayScale(0.799f);
    
    // Tweak the values
    skyHigh *= Colors.MakeGrayScale(0.961f);
    skyLow  *= Colors.MakeGrayScale(1.01f);
    
    // Uniform scale down
    skyHigh *= Colors.MakeGrayScale(0.24f);
    skyLow  *= Colors.MakeGrayScale(0.24f);
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->ambient = Colors.MakeGrayScale(0.0f);
    skyMaterial->diffuse = Colors.MakeGrayScale(0.0f);
    
    skyMaterial->shader  = Resources.shaders.sky;
    
    skyMaterial->DisableDepthTest();
    skyMaterial->DisableShadowVolumePass();
    
    for (unsigned int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        if (vertex.y > 0) {
            vertex.r = Math.Lerp(skyHigh.r, skyLow.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(skyHigh.g, skyLow.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(skyHigh.b, skyLow.b, vertex.y * biasMul);
        } else {
            vertex.r = skyLow.r;
            vertex.g = skyLow.g;
            vertex.b = skyLow.b;
        }
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->Load();
    
    GameObject* skyObject = Engine.Create<GameObject>();
    skyObject->name = "sky";
    
    Component* skyRendererComponent = Engine.CreateComponent<MeshRenderer>();
    skyObject->AddComponent( skyRendererComponent );
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    skyRenderer->mesh = skyMesh;
    skyRenderer->material = skyMaterial;
    
    skyObject->renderDistance = -1;
    
    Transform* transformCache = skyObject->GetComponent<Transform>();
    transformCache->SetScale(10000, 2000, 10000);
    
    return skyObject;
}
